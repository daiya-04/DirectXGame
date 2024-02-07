#pragma once

#include "Camera.h"
#include "WorldTransform.h"
#include "Quaternion.h"
#include "Engine/Input/input.h"
#include "Vec2.h"

struct WorkInterpolation {
	//追従対象の残像座標
	Vector3 interTarget_ = {};
	//追従対象のY軸
	float targetAngleY_;
	//カメラ補間の媒介変数
	float interParameter_;
};
class Player;

class FollowCamera {
public:
	void Init();
	void Update();
#pragma region
	void SetTarget(const WorldTransform* target);
	void SetFocusTarget() {};
	void SetPlayer(Player* player) { player_ = player; }
#pragma endregion Setter
#pragma region
	const Camera& GetViewProjection() { return viewProjection_; }
#pragma endregion Getter
	void ImGui();
private:
	//ビュープロジェクション
	Camera viewProjection_;
	//追従対象
	const WorldTransform* target_ = nullptr;
	// ゲームパッド
	Input* input = nullptr;

	float rotate_ = { 0.0f };

	float parameter_t = 0.0f;
	float GrapParameter_t = 0.0f;

	//追従対象の座標・角度を再設定
	void Reset();
	WorkInterpolation workInter;
	//追従対象からのオフセットを計算する
	Vector3 OffsetCalc();

	Vector3 offset = { 0.0f, 0.0f, -25.0f };
	//球面線形補間
	float LerpShortAngle(float a, float b, float t);
	bool IsGrap = false;
	Player* player_;
};