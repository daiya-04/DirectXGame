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

bool Input::PushKey(BYTE keyNumber) const {

	if (key[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::TriggerKey(BYTE keyNumber) const {

	if (key[keyNumber] && !preKey[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::ReleaseKey(BYTE keyNumber) const {
	if (!key[keyNumber] && preKey[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::GetJoystickState() {

	DWORD dwResult;

	preJoyState = joyState;

	dwResult = XInputGetState(0, &joyState);

	if (dwResult == ERROR_SUCCESS) {
		if (fabs(joyState.Gamepad.sThumbLX) < deadZone_ && fabs(joyState.Gamepad.sThumbLY) < deadZone_) {
			joyState.Gamepad.sThumbLX = 0;
			joyState.Gamepad.sThumbLY = 0;

		}
		if (fabs(joyState.Gamepad.sThumbRX) < deadZone_ && fabs(joyState.Gamepad.sThumbRY) < deadZone_) {
			joyState.Gamepad.sThumbRX = 0;
			joyState.Gamepad.sThumbRY = 0;

		}

		return true;
	}

	return false;
}

bool Input::GetJoystickLState() {
	DWORD dwResult;

	dwResult = XInputGetState(0, &joyState);

	if (dwResult == ERROR_SUCCESS) {
		if (fabs(joyState.Gamepad.sThumbLX) < 10000 && fabs(joyState.Gamepad.sThumbLY) < 10000) {

			return false;
		}


		return true;
	}

	return false;
}

bool Input::TriggerLStick(Stick direction) const {

	if (direction == Stick::Right) {
		return IsLStickRight();
	}
	if (direction == Stick::Left) {
		return IsLStickLeft();
	}
	if (direction == Stick::Up) {
		return IsLStickUp();
	}
	if (direction == Stick::Down) {
		return IsLStickDown();
	}
	if (direction == Stick::All) {
		float len = Vector2(static_cast<float>(joyState.Gamepad.sThumbLX), static_cast<float>(joyState.Gamepad.sThumbLY)).Length();
		float preLen = Vector2(static_cast<float>(preJoyState.Gamepad.sThumbLX), static_cast<float>(preJoyState.Gamepad.sThumbLY)).Length();
		return (len >= deadZone_ && preLen < deadZone_);
	}
	return false;
}

bool Input::TriggerRStick(Stick direction) const {

	if (direction == Stick::Right) {
		return IsRStickRight();
	}
	if (direction == Stick::Left) {
		return IsRStickLeft();
	}
	if (direction == Stick::Up) {
		return IsRStickUp();
	}
	if (direction == Stick::Down) {
		return IsRStickDown();
	}
	if (direction == Stick::All) {
		float len = Vector2(static_cast<float>(joyState.Gamepad.sThumbRX), static_cast<float>(joyState.Gamepad.sThumbRY)).Length();
		float preLen = Vector2(static_cast<float>(preJoyState.Gamepad.sThumbRX), static_cast<float>(preJoyState.Gamepad.sThumbRY)).Length();
		return (len >= deadZone_ && preLen < deadZone_);
	}

	return false;
}

bool Input::ReleaseLStick(Stick direction) const {

	if (direction == Stick::Right) {
		return (float)(joyState.Gamepad.sThumbLX) <= deadZone_ && (float)(preJoyState.Gamepad.sThumbLX) > deadZone_;
	}
	if (direction == Stick::Left) {
		return (float)(joyState.Gamepad.sThumbLX) >= -deadZone_ && (float)(preJoyState.Gamepad.sThumbLX) < -deadZone_;
	}
	if (direction == Stick::Up) {
		return (float)(joyState.Gamepad.sThumbLY) <= deadZone_ && (float)(preJoyState.Gamepad.sThumbLY) > deadZone_;
	}
	if (direction == Stick::Down) {
		return (float)(joyState.Gamepad.sThumbLY) >= -deadZone_ && (float)(preJoyState.Gamepad.sThumbLY) < -deadZone_;
	}
	if (direction == Stick::All) {
		float len = Vector2(static_cast<float>(joyState.Gamepad.sThumbLX), static_cast<float>(joyState.Gamepad.sThumbLY)).Length();
		float preLen = Vector2(static_cast<float>(preJoyState.Gamepad.sThumbLX), static_cast<float>(preJoyState.Gamepad.sThumbLY)).Length();
		return (len < deadZone_ && deadZone_ <= preLen);
	}

	return false;

}

bool Input::ReleaseRStick(Stick direction) const {

	if (direction == Stick::Right) {
		return (float)(joyState.Gamepad.sThumbRX) <= deadZone_ && (float)(preJoyState.Gamepad.sThumbRX) > deadZone_;
	}
	if (direction == Stick::Left) {
		return (float)(joyState.Gamepad.sThumbRX) >= -deadZone_ && (float)(preJoyState.Gamepad.sThumbRX) < -deadZone_;
	}
	if (direction == Stick::Up) {
		return (float)(joyState.Gamepad.sThumbRY) <= deadZone_ && (float)(preJoyState.Gamepad.sThumbRY) > deadZone_;
	}
	if (direction == Stick::Down) {
		return (float)(joyState.Gamepad.sThumbRY) >= -deadZone_ && (float)(preJoyState.Gamepad.sThumbRY) < -deadZone_;
	}
	if (direction == Stick::All) {
		float len = Vector2(static_cast<float>(joyState.Gamepad.sThumbRX), static_cast<float>(joyState.Gamepad.sThumbRY)).Length();
		float preLen = Vector2(static_cast<float>(preJoyState.Gamepad.sThumbRX), static_cast<float>(preJoyState.Gamepad.sThumbRY)).Length();
		return (len < deadZone_ && deadZone_ <= preLen);
	}

	return false;

}

bool Input::TiltLStick(Stick direction) const {
	if (direction == Stick::Right) {
		return IsLTiltRight();
	}
	if (direction == Stick::Left) {
		return IsLTiltLeft();
	}
	if (direction == Stick::Up) {
		return IsLTiltUp();
	}
	if (direction == Stick::Down) {
		return IsLTiltDown();
	}
	return false;
}

bool Input::TiltRStick(Stick direction) const {
	if (direction == Stick::Right) {
		return IsRTiltRight();
	}
	if (direction == Stick::Left) {
		return IsRTiltLeft();
	}
	if (direction == Stick::Up) {
		return IsRTiltUp();
	}
	if (direction == Stick::Down) {
		return IsRTiltDown();
	}
	return false;
}

bool Input::TriggerButton(Button button) const {
	if ((joyState.Gamepad.wButtons & (WORD)button) && !(preJoyState.Gamepad.wButtons & (WORD)button)) {
		return true;
	}
	return false;
}

bool Input::PushButton(Button button) const {
	if ((joyState.Gamepad.wButtons & (WORD)button)) {
		return true;
	}
	return false;
}

bool Input::ReleaseButton(Button button) const {
	if (!(joyState.Gamepad.wButtons & (WORD)button) && (preJoyState.Gamepad.wButtons & (WORD)button)) {
		return true;
	}
	return false;
}

bool Input::PushTrigger(Trigger trigger) const {

	switch (trigger) {
	case Trigger::Left:
		if (joyState.Gamepad.bLeftTrigger) {
			return true;
		}
		break;
	case Trigger::Right:
		if (joyState.Gamepad.bRightTrigger) {
			return true;
		}
		break;
	}

	return false;
}

bool Input::ReleaseTrigger(Trigger trigger) const {

	switch (trigger) {
	case Trigger::Left:
		if (!joyState.Gamepad.bLeftTrigger && preJoyState.Gamepad.bLeftTrigger) {
			return true;
		}
		break;
	case Trigger::Right:
		if (!joyState.Gamepad.bRightTrigger && preJoyState.Gamepad.bRightTrigger) {
			return true;
		}
		break;
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

void Input::Vibration(float value) {

	value = std::clamp(value, 0.0f, 1.0f);

	XINPUT_VIBRATION vib{};

	//低周波
	vib.wLeftMotorSpeed = WORD(USHRT_MAX * value);
	//高周波
	vib.wRightMotorSpeed = WORD(USHRT_MAX * value);

	XInputSetState(0, &vib);
}

bool Input::IsLStickRight() const {

	if ((float)(joyState.Gamepad.sThumbLX) > deadZone_ && (float)(preJoyState.Gamepad.sThumbLX) <= deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsLStickLeft() const {
	if ((float)(joyState.Gamepad.sThumbLX) < -deadZone_ && (float)(preJoyState.Gamepad.sThumbLX) >= -deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsLStickUp() const {
	if ((float)(joyState.Gamepad.sThumbLY) > deadZone_ && (float)(preJoyState.Gamepad.sThumbLY) <= deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsLStickDown() const {
	if ((float)(joyState.Gamepad.sThumbLY) < -deadZone_ && (float)(preJoyState.Gamepad.sThumbLY) >= -deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRStickRight() const {
	if (float(joyState.Gamepad.sThumbRX) > deadZone_ && float(preJoyState.Gamepad.sThumbRX) <= deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRStickLeft() const {
	if (float(joyState.Gamepad.sThumbRX) < -deadZone_ && float(preJoyState.Gamepad.sThumbRX) >= -deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRStickUp() const {
	if (float(joyState.Gamepad.sThumbRY) > deadZone_ && float(preJoyState.Gamepad.sThumbRY) <= deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRStickDown() const {
	if (float(joyState.Gamepad.sThumbRY) < -deadZone_ && float(preJoyState.Gamepad.sThumbRY) >= -deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsLTiltRight() const {
	if ((float)(joyState.Gamepad.sThumbLX) > deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsLTiltLeft() const {
	if ((float)(joyState.Gamepad.sThumbLX) < -deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsLTiltUp() const {
	if ((float)(joyState.Gamepad.sThumbLY) > deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsLTiltDown() const {
	if ((float)(joyState.Gamepad.sThumbLY) < -deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRTiltRight() const {
	if (float(joyState.Gamepad.sThumbRX) > deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRTiltLeft() const {
	if (float(joyState.Gamepad.sThumbRX) < -deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRTiltUp() const {
	if (float(joyState.Gamepad.sThumbRY) > deadZone_) {
		return true;
	}
	return false;
}

bool Input::IsRTiltDown() const {
	if (float(joyState.Gamepad.sThumbRY) < deadZone_) {
		return true;
	}
	return false;
}