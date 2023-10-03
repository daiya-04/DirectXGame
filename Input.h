#pragma once
#include <Windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION    0x0800
#include <dinput.h>

class Input{
public:

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	ComPtr<IDirectInput8> directInput = nullptr;
	ComPtr<IDirectInputDevice8> keyBoard = nullptr;
	BYTE key[256] = {};
	BYTE preKey[256] = {};

public:

	//初期化
	void Initialize(HINSTANCE hInstance,HWND hwnd);
	//更新
	void Update();

	//キーの押下をチェック
	bool PushKey(BYTE keyNumber);

	//キーのトリガーをチェック
	bool TriggerKey(BYTE keyNumber);

};

