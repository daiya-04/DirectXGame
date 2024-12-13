#pragma once
#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl.h>

#include <fstream>
#include <vector>

class Audio{
private:

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static const size_t kMaxNumPlayHandles = 256;

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

	static size_t Load(const std::string& filename);
private:

	size_t FindUnUsedPlayHandle();

	void DestroyPlayHandle(size_t playHandle);

public:
	void Initialize();
	void Update();
	//音声データの解放
	void SoundUnload(size_t soundHandle);
	//音声再生
	size_t Play(size_t soundHandle, float volume = 1.0f,bool loop = false);

	void SoundPlayLoopEnd(size_t playhandle);
	//音声ロード

	size_t LoadInternal(const std::string& filename);
	//再生停止
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

