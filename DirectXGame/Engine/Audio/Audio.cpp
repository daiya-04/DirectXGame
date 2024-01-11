#include "Audio.h"
#include <cassert>

#pragma comment(lib,"xaudio2.lib")

Audio* Audio::GetInstance() {
	static Audio instance;

	return &instance;
}

Audio::~Audio() {
	for (size_t index = 0; index < kMaxNumPlayHandles; ++index) {
		DestroyPlayHandle(index);
	}

	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
}

void Audio::Initialize() {
	HRESULT hr;
	//XAudioエンジンのインスタンスを作成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	//マスターボイスを作成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	for (size_t index = 0; index < kMaxNumPlayHandles; ++index) {
		DestroyPlayHandle(index);
	}
}

void Audio::Update() {
	for (size_t index = 0; index < kMaxNumPlayHandles; ++index) {
		if (sourceVoices_[index]) {
			XAUDIO2_VOICE_STATE state{};
			sourceVoices_[index]->GetState(&state);
			if (state.BuffersQueued == 0) {
				DestroyPlayHandle(index);
			}
		}
	}
}

size_t Audio::SoundPlayWave(size_t soundHandle) {
	HRESULT hr;
	const SoundData& soundData = soundData_.at(soundHandle);

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer.data();
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	size_t playhandle = FindUnUsedPlayHandle();
	//プレイハンドルがいっぱい
	assert(playhandle < kMaxNumPlayHandles);

	//sourceVoiceの作成
	IXAudio2SourceVoice* pSourcVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourcVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	hr = pSourcVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));

	hr = pSourcVoice->Start();
	assert(SUCCEEDED(hr));

	sourceVoices_[playhandle] = pSourcVoice;
	return playhandle;
}

size_t Audio::SoundPlayLoopStart(size_t soundHandle) {
	HRESULT hr;
	const SoundData& soundData = soundData_.at(soundHandle);

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer.data();
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;

	size_t playhandle = FindUnUsedPlayHandle();
	//プレイハンドルがいっぱい
	assert(playhandle < kMaxNumPlayHandles);

	//sourceVoiceの作成
	IXAudio2SourceVoice* pSourcVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourcVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	hr = pSourcVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));

	hr = pSourcVoice->Start();
	assert(SUCCEEDED(hr));

	sourceVoices_[playhandle] = pSourcVoice;
	return playhandle;
}

void Audio::SoundPlayLoopEnd(size_t playHandle) {
	if (IsValidPlayhandle(playHandle)) {
		sourceVoices_[playHandle]->Stop();
		DestroyPlayHandle(playHandle);
	}
}

size_t Audio::SoundLoadWave(const std::string& filename) {
	auto iter = std::find_if(soundData_.begin(), soundData_.end(), [&](const SoundData& soundData) {
		return soundData.filename == filename;
		});
	if (iter != soundData_.end()) {
		return std::distance(soundData_.begin(), iter);
	}

#pragma region ファイルオープン
	//directoryPath
	std::string directoryPath = "Resources/sounds/";
	//ファイル出力ストリームのインスタンス
	std::ifstream file;
	//wavファイルをバイナリモードで開く
	file.open(directoryPath + filename, std::ios_base::binary);
	assert(file.is_open());

#pragma endregion
#pragma region wavデータの読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRiffかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//Formatチャンクの読み込み
	FormatChunk format{};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//読み取りチャンクを検出した場合	
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//LISTチャンクを検出した場合
	if (strncmp(data.id, "LIST", 4) == 0) {
		//読み取りチャンクを検出した場合	
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	// bextチャンクを検出した場合
	if (strncmp(data.id, "bext", 4) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//INFOチャンクを検出した場合
	if (strncmp(data.id, "INFO", 4) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//REAPERチャンクを検出した場合
	if (strncmp(data.id, "REAPER", 6) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "junk", 4) == 0) {
		// 読み取りチャンクを検出した場合
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	//Dataチャンクのデータ部(波形データ)の読み込み
	std::vector<BYTE> pBuffer(data.size);
	file.read(reinterpret_cast<char*>(pBuffer.data()), data.size);

	//Waveファイルを閉じる
	file.close();
#pragma endregion
#pragma region 読み込んだ音声データのreturn
	//returnする為の音声データ
	SoundData soundData{};
	soundData.filename = filename;
	soundData.wfex = format.fmt;
	soundData.pBuffer = std::move(pBuffer);
	soundData.bufferSize = data.size;
#pragma endregion
	soundData_.emplace_back(soundData);

	return soundData_.size() - 1;
}

void Audio::StopSound(size_t playHandle) {
	assert(playHandle < kMaxNumPlayHandles);
	DestroyPlayHandle(playHandle);
}

void Audio::SetPitch(size_t playHandle, float pitch) {
	assert(playHandle < kMaxNumPlayHandles);
	sourceVoices_[playHandle]->SetFrequencyRatio(pitch);
}

void Audio::SetValume(size_t playHandle, float volume) {
	assert(playHandle < kMaxNumPlayHandles);
	sourceVoices_[playHandle]->SetVolume(volume);
}

bool Audio::IsValidPlayhandle(size_t playHandle) {
	return playHandle < kMaxNumPlayHandles && sourceVoices_[playHandle] != nullptr;
}

size_t Audio::FindUnUsedPlayHandle() {
	for (size_t index = 0; index < kMaxNumPlayHandles; ++index) {
		if (sourceVoices_[index] == nullptr) {
			return index;
		}
	}
	return size_t(-1);
}

void Audio::DestroyPlayHandle(size_t playHandle) {
	assert(playHandle < kMaxNumPlayHandles);
	if (sourceVoices_[playHandle]) {
		sourceVoices_[playHandle]->DestroyVoice();
		sourceVoices_[playHandle] = nullptr;
	}
}

void Audio::SoundUnload(size_t soundHandle) {
	//バッファのメモリの解放
	soundData_.at(soundHandle).pBuffer.clear();
	soundData_.at(soundHandle).bufferSize = 0;
	soundData_.at(soundHandle).wfex = {};
	soundData_.erase(soundData_.begin() + soundHandle);
}