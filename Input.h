#pragma once
#include <Windows.h>
#include <wrl.h>
#include "WinApp.h"
#include "Vec3.h"

#include <XInput.h>
#define DIRECTINPUT_VERSION    0x0800
#include <dinput.h>

class Input{
private:

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	ComPtr<IDirectInput8> directInput;
	ComPtr<IDirectInputDevice8> keyBoard;
	BYTE key[256] = {};
	BYTE preKey[256] = {};
	XINPUT_STATE joyState;
	XINPUT_STATE preJoyState;

	WinApp* win_ = nullptr;

public:

	static Input* GetInstance();

	//初期化
	void Initialize(WinApp* win);
	//更新
	void Update();

	//キーの押下をチェック
	bool PushKey(BYTE keyNumber) const;

	//キーのトリガーをチェック
	bool TriggerKey(BYTE keyNumber) const;
	//ゲームパッドの状態取得
	bool GetJoystickState();

	Vector3 GetMoveXZ() {
		return { (float)joyState.Gamepad.sThumbLX, 0.0f, (float)joyState.Gamepad.sThumbLY };
	}

	Vector3 GetCameraRotate() {
		return { (float)joyState.Gamepad.sThumbRY / SHRT_MAX,(float)joyState.Gamepad.sThumbRX / SHRT_MAX,0.0f };
	}

	bool TriggerButton(int button) const;

	bool LeftTrigger() const;

	bool RightTrigger() const;

private:
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
};

