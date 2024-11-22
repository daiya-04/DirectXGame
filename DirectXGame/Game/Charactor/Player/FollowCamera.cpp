#include "FollowCamera.h"
#include "Input.h"
#include <cmath>
#include <numbers>
#include "ImGuiManager.h"
#include "Easing.h"

void FollowCamera::Init(){

	camera_.Init();

	camera_.UpdateViewMatrix();
}

void FollowCamera::Update(){

	const float rotateSpeed = 0.02f;

	camera_.rotation_ += Input::GetInstance()->GetCameraRotate() * rotateSpeed;

	camera_.rotation_.x = min(camera_.rotation_.x, 45.0f * (float)std::numbers::pi / 180.0f);
	camera_.rotation_.x = max(camera_.rotation_.x, -5.0f * (float)std::numbers::pi / 180.0f);

	if (target_) {

		delayParam_ += (float)1 / (float)delayTime_;

		if (delayParam_ >= 1.0f) {
			delayParam_ = 1.0f;
		}

		float T = Easing::easeInSine(delayParam_);

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

	rotateMatrix = MakeRotateXMatrix(camera_.rotation_.x) *
		           MakeRotateYMatrix(camera_.rotation_.y) *
		           MakeRotateZMatrix(camera_.rotation_.z);

	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;

	Vector3 offset = OffsetCalc();
	camera_.translation_ = target_->translation_ + offset;
	camera_.UpdateViewMatrix();
}
