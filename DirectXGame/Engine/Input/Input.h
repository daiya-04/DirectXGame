#pragma once
#include <Windows.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"

#include <XInput.h>
#define DIRECTINPUT_VERSION    0x0800
#include <dinput.h>

class Input {
private:

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	enum class Button {
		DPAD_UP = 0x0001,
		DPAD_DOWN = 0x0002,
		DPAD_LEFT = 0x0004,
		DPAD_RIGHT = 0x0008,
		START = 0x0010,
		BACK = 0x0020,
		LEFT_THUMB = 0x0040,
		RIGHT_THUMB = 0x0080,
		LEFT_SHOULDER = 0x0100,
		RIGHT_SHOULDER = 0x0200,
		A = 0x1000,
		B = 0x2000,
		X = 0x4000,
		Y = 0x8000,
	};

	enum class Trigger {
		Left,
		Right,
	};

	enum class Stick {
		Left,
		Right,
		Up,
		Down,
		All,
	};

private:

	ComPtr<IDirectInput8> directInput;
	ComPtr<IDirectInputDevice8> keyBoard;
	BYTE key[256] = {};
	BYTE preKey[256] = {};
	XINPUT_STATE joyState{};
	XINPUT_STATE preJoyState{};
	SHORT deadZone_ = 10000;

public:

	static Input* GetInstance();

	///使用禁止
	//初期化
	void Initialize();
	//更新
	void Update();
	//パッドの状態の取得
	bool GetJoystickState();
	///


	//キーの押下をチェック
	bool PushKey(BYTE keyNumber) const;

	//キーのトリガーをチェック
	bool TriggerKey(BYTE keyNumber) const;

	bool ReleaseKey(BYTE keyNumber) const;

	bool GetJoystickLState();

	Vector3 GetMoveXZ() {
		float len = Vector2(static_cast<float>(joyState.Gamepad.sThumbLX), static_cast<float>(joyState.Gamepad.sThumbLY)).Length();
		if (len < deadZone_) { return Vector3(); }
		return { (float)joyState.Gamepad.sThumbLX, 0.0f, (float)joyState.Gamepad.sThumbLY };
	}

	Vector3 GetCameraRotate() {
		return { (float)joyState.Gamepad.sThumbRY / SHRT_MAX,(float)joyState.Gamepad.sThumbRX / SHRT_MAX,0.0f };
	}

	//Lスティックの傾きチェック
	bool TriggerLStick(Stick direction) const;
	//Rスティック
	bool TriggerRStick(Stick derection) const;

	bool ReleaseLStick(Stick direction) const;

	bool ReleaseRStick(Stick direction) const;

	bool TiltLStick(Stick direction) const;

	bool TiltRStick(Stick direction) const;

	//パッドボタンのトリガーをチェック
	bool TriggerButton(Button button) const;
	//パッドボタンの押下をチェック
	bool PushButton(Button button) const;
	//パッドボタンのリリース(離し)のチェック
	bool ReleaseButton(Button button) const;
	//RT,LTの押下のチェック
	bool PushTrigger(Trigger trigger) const;
	//RT,LTのリリース(離し)のチェック
	bool ReleaseTrigger(Trigger trigger) const;

	bool LeftTrigger() const;

	bool RightTrigger() const;

	/// <summary>
	/// パッドの振動の大きさの設定
	/// </summary>
	/// <param name="value">0.0f ~ 1.0f</param>
	void Vibration(float value);

private:
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	bool IsLStickRight() const;

	bool IsLStickLeft() const;

	bool IsLStickUp() const;

	bool IsLStickDown() const;

	bool IsRStickRight() const;

	bool IsRStickLeft() const;

	bool IsRStickUp() const;

	bool IsRStickDown() const;

	bool IsLTiltRight() const;

	bool IsLTiltLeft() const;

	bool IsLTiltUp() const;

	bool IsLTiltDown() const;

	bool IsRTiltRight() const;

	bool IsRTiltLeft() const;

	bool IsRTiltUp() const;

	bool IsRTiltDown() const;
};
