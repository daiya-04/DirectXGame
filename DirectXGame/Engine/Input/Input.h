#pragma once
///---------------------------------------------------------------------------------------------
//
// Input
// キーボードとパッドの入力
//
///---------------------------------------------------------------------------------------------

#include <Windows.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"

#include <XInput.h>
#define DIRECTINPUT_VERSION    0x0800
#include <dinput.h>

namespace DaiEngine {
	//Inputクラス
	class Input {
	private:

		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:

		enum class Button {
			//十字上
			DPAD_UP = 0x0001,
			//十字下
			DPAD_DOWN = 0x0002,
			//十字左
			DPAD_LEFT = 0x0004,
			//十字右
			DPAD_RIGHT = 0x0008,
			//スタート(三本線のやつ)
			START = 0x0010,
			//バック(四角が重なってるやつ)
			BACK = 0x0020,
			//左スティック押し込み
			LEFT_THUMB = 0x0040,
			//右スティック押し込み
			RIGHT_THUMB = 0x0080,
			//左肩
			LEFT_SHOULDER = 0x0100,
			//右肩
			RIGHT_SHOULDER = 0x0200,
			//Aボタン
			A = 0x1000,
			//Bボタン
			B = 0x2000,
			//Xボタン
			X = 0x4000,
			//Yボタン
			Y = 0x8000,
		};
		//トリガー
		enum class Trigger {
			Left,
			Right,
		};
		//スティック
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
		//キーのリリースチェック
		bool ReleaseKey(BYTE keyNumber) const;
		//Lスティックの状態の取得
		bool GetJoystickLState();
		//XZ方向の移動ベクトル取得
		Vector3 GetMoveXZ() {
			float len = Vector2(static_cast<float>(joyState.Gamepad.sThumbLX), static_cast<float>(joyState.Gamepad.sThumbLY)).Length();
			if (len < deadZone_) { return Vector3(); }
			return { static_cast<float>(joyState.Gamepad.sThumbLX), 0.0f, static_cast<float>(joyState.Gamepad.sThumbLY) };
		}
		//カメラの回転取得
		Vector3 GetCameraRotate() {
			return { static_cast<float>(joyState.Gamepad.sThumbRY) / SHRT_MAX, static_cast<float>(joyState.Gamepad.sThumbRX) / SHRT_MAX, 0.0f };
		}

		//Lスティックのトリガーチェック
		bool TriggerLStick(Stick direction) const;
		//Rスティックのトリガーチェック
		bool TriggerRStick(Stick derection) const;
		//Lスティックのリリースチェック
		bool ReleaseLStick(Stick direction) const;
		//Rスティックのリリースチェック
		bool ReleaseRStick(Stick direction) const;
		//Lスティックの傾きーチェック
		bool TiltLStick(Stick direction) const;
		//Rスティックの傾きチェック
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
		//パッドの左トリガー入力瞬間
		bool LeftTrigger() const;
		//パッドの右トリガー入力瞬間
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

		//Lスティックの入力瞬間判定
		bool IsLStickRight() const;
		bool IsLStickLeft() const;
		bool IsLStickUp() const;
		bool IsLStickDown() const;

		//Rスティックの入力瞬間判定
		bool IsRStickRight() const;
		bool IsRStickLeft() const;
		bool IsRStickUp() const;
		bool IsRStickDown() const;

		//Lスティックの入力判定
		bool IsLTiltRight() const;
		bool IsLTiltLeft() const;
		bool IsLTiltUp() const;
		bool IsLTiltDown() const;

		//Rスティックの入力判定
		bool IsRTiltRight() const;
		bool IsRTiltLeft() const;
		bool IsRTiltUp() const;
		bool IsRTiltDown() const;
	};

}
