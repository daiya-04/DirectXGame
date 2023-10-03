#include "Input.h"
#include <assert.h>

#pragma comment(lib,"dinput8.lib")

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT hr;

	//DirectInputの初期化
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));
	//キーボードデバイスの生成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyBoard, NULL);
	assert(SUCCEEDED(hr));
	//入力データの	形式セット
	hr = keyBoard->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyBoard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Input::Update() {

	memcpy(preKey, key, sizeof(key));

	//キーボードの情報の取得開始
	keyBoard->Acquire();
	//全キーの入力状態を取得する
	keyBoard->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber) {

	if (key[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {

	if (key[keyNumber] && !preKey[keyNumber]) {
		return true;
	}

	return false;
}