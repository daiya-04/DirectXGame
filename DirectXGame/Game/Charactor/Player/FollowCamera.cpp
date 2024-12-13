#include "FollowCamera.h"
#include "Input.h"
#include <cmath>
#include <numbers>
#include "ImGuiManager.h"
#include "Easing.h"

void FollowCamera::Init(){
	//カメラ初期化
	camera_.Init();
	//行列更新
	camera_.UpdateViewMatrix();
}

void FollowCamera::Update(){

	const float rotateSpeed = 0.02f;
	const float rotateMax = 45.0f;
	const float rotateMin = -5.0f;

	camera_.rotation_ += Input::GetInstance()->GetCameraRotate() * rotateSpeed;
	//角度制限
	camera_.rotation_.x = min(camera_.rotation_.x, rotateMax * std::numbers::pi_v<float> / 180.0f);
	camera_.rotation_.x = max(camera_.rotation_.x, -rotateMin * std::numbers::pi_v<float> / 180.0f);

	if (target_) {
		//ダッシュしたときにカメラを遅延させる
		delayParam_ += 1.0f / static_cast<float>(delayTime_);

		delayParam_ = std::clamp(delayParam_, 0.0f, 1.0f);

		float T = Easing::easeInSine(delayParam_);
		//補間
		interTarget_ = Lerp(T, interTarget_, target_->translation_);

		Vector3 offset = OffsetCalc();


		camera_.translation_ = interTarget_ + offset;
	}

	camera_.UpdateViewMatrix();
}

void FollowCamera::Reset() {

	if (target_) {
		interTarget_ = target_->translation_;
	}

	Vector3 offset = OffsetCalc();
	camera_.translation_ = interTarget_ + offset;
	delayParam_ = 0.0f;

}

Vector3 FollowCamera::OffsetCalc() const {

	Vector3 offset = { 0.0f,2.0f,-5.0f };
	Matrix4x4 rotateMatrix;
	//回転行列計算
	rotateMatrix = MakeRotateXMatrix(camera_.rotation_.x) *
		           MakeRotateYMatrix(camera_.rotation_.y) *
		           MakeRotateZMatrix(camera_.rotation_.z);

	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	//オフセット計算
	Vector3 offset = OffsetCalc();
	camera_.translation_ = target_->translation_ + offset;
	//行列更新
	camera_.UpdateViewMatrix();
}
