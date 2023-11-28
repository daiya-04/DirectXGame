#pragma once
#include <xaudio2.h>
#include <wrl.h>

#include <fstream>
#include <vector>

class Audio{
private:

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static const size_t kMaxNumPlayHandles = 128;

	//チャンクヘッダー
	struct ChunkHeader {
		char id[4]; //チャンク毎のID
		uint32_t size; //チャンクファイル
	};

	//RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk; // "RIFF"
		char type[4]; // "WAVE"
	};

	//FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; // "fmt"
		WAVEFORMATEX fmt; //波形フォーマット
	};
	//音声データ
	struct SoundData {
		std::string filename;
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		std::vector<BYTE> pBuffer;
		//バッファのサイズ
		uint32_t bufferSize;
	};
public:
	static Audio* GetInstance();
private:

	size_t FindUnUsedPlayHandle();

	void DestroyPlayHandle(size_t playHandle);

public:
	void Initialize();
	void Update();
	//音声データの解放
	void SoundUnload(size_t soundHandle);
	//音声再生
	size_t SoundPlayWave(size_t soundHandle);

	size_t SoundPlayLoopStart(size_t soundHandle);
	void SoundPlayLoopEnd(size_t playhandle);
	//音声ロード
	size_t SoundLoadWave(const std::string& filename);

	void StopSound(size_t playhandle);
	void SetPitch(size_t playHandle, float pitch);
	void SetValume(size_t playHandle, float volume);
	bool IsValidPlayhandle(size_t playHandle);

private:

	ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	std::vector<SoundData> soundData_;
	IXAudio2SourceVoice* sourceVoices_[kMaxNumPlayHandles]{ nullptr };

private:
	Audio() = default;
	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
	~Audio();

};

