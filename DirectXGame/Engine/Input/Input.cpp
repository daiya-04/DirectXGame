#include "Input.h"

#include "WinApp.h"
#include <assert.h>
#include <cmath>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"XInput.lib")

Input* Input::GetInstance() {
	static Input instance;

	return &instance;
}

void Input::Initialize() {

	HRESULT hr;

	//DirectInputの初期化
	hr = DirectInput8Create(WinApp::GetInstance()->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));
	//キーボードデバイスの生成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyBoard, NULL);
	assert(SUCCEEDED(hr));
	//入力データの	形式セット
	hr = keyBoard->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyBoard->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Input::Update() {

	memcpy(preKey, key, sizeof(key));

	//キーボードの情報の取得開始
	keyBoard->Acquire();
	//全キーの入力状態を取得する
	keyBoard->GetDeviceState(sizeof(key), key);

	GetJoystickState();

}

bool Input::PushKey(BYTE keyNumber) const{

	if (key[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::TriggerKey(BYTE keyNumber) const{

	if (key[keyNumber] && !preKey[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::GetTiltJoyStickRight(){
	if ((float)(joyState.Gamepad.sThumbLX) > 10000 && (float)(preJoyState.Gamepad.sThumbLX) <= 10000) {
		return true;
	}
	else {
		return false;
	}
}

bool Input::GetTiltJoyStickLeft()
{
	if ((float)(joyState.Gamepad.sThumbLX) < -10000 && (float)(preJoyState.Gamepad.sThumbLX) >= -10000) {
		return true;
	}
	else {
		return false;
	}
}

bool Input::GetTiltJoyStickUp()
{
	if ((float)(joyState.Gamepad.sThumbLY) > 10000 && (float)(preJoyState.Gamepad.sThumbLY) <= 10000) {
		return true;
	}
	else {
		return false;
	}
}

bool Input::GetTiltJoyStickDown()
{
	if ((float)(joyState.Gamepad.sThumbLY) < -10000 && (float)(preJoyState.Gamepad.sThumbLY) >= -10000) {
		return true;
	}
	else {
		return false;
	}
}


bool Input::GetJoystickState() {

	DWORD dwResult;

	preJoyState = joyState;

	dwResult = XInputGetState(0, &joyState);

	if (dwResult == ERROR_SUCCESS) {
		if (fabs(joyState.Gamepad.sThumbLX) < 2500) {
			joyState.Gamepad.sThumbLX = 0;
		}
		if (fabs(joyState.Gamepad.sThumbLY) < 2500) {
			joyState.Gamepad.sThumbLY = 0;
		}
		if (fabs(joyState.Gamepad.sThumbRX) < 2500) {
			joyState.Gamepad.sThumbRX = 0;
		}
		if (fabs(joyState.Gamepad.sThumbRY) < 2500) {
			joyState.Gamepad.sThumbRY = 0;
		}
		return true;
	}

	return false;
}

bool Input::LeftTrigger() const {
	if (joyState.Gamepad.bLeftTrigger && !preJoyState.Gamepad.bLeftTrigger) {
		return true;
	}
	return false;
}

bool Input::RightTrigger() const {
	if (joyState.Gamepad.bRightTrigger && !preJoyState.Gamepad.bRightTrigger) {
		return true;
	}
	return false;
}
