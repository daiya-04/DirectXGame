#pragma once
#include <Windows.h>
#include <wrl.h>
#include "WinApp.h"

#define DIRECTINPUT_VERSION    0x0800
#include <dinput.h>

class Input{
public:

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	ComPtr<IDirectInput8> directInput;
	ComPtr<IDirectInputDevice8> keyBoard;
	BYTE key[256] = {};
	BYTE preKey[256] = {};

	WinApp* win_ = nullptr;

public:

	static Input* GetInstance();

	//初期化
	void Initialize(WinApp* win);
	//更新
	void Update();

	//キーの押下をチェック
	bool PushKey(BYTE keyNumber);

	//キーのトリガーをチェック
	bool TriggerKey(BYTE keyNumber);

};

