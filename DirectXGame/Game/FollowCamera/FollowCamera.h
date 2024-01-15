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

class FollowCamera {
public:
	void Init();
	void Update();

	void SetTarget(const WorldTransform* target);
	const Camera& GetViewProjection() { return viewProjection_; }

private:
	//ビュープロジェクション
	Camera viewProjection_;
	//追従対象
	const WorldTransform* target_ = nullptr;
	// ゲームパッド
	Input* input = nullptr;

	Vector2 rotate_ = { 0.0f,0.0f };

	float parameter_t = 0.0f;
	float GrapParameter_t = 0.0f;

	//追従対象の座標・角度を再設定
	void Reset();
	WorkInterpolation workInter;
	//追従対象からのオフセットを計算する
	Vector3 OffsetCalc();

	//球面線形補間
	float LerpShortAngle(float a, float b, float t);
};