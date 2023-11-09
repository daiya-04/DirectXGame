#include "FollowCamera.h"
#include <cmath>
#include "Easing.h"

void FollowCamera::Initialize() {

	

}

void FollowCamera::Update() {

	const float rotateSpeed = 0.02f;

	viewProjection_.rotation_ += Input::GetInstance()->GetCameraRotate() * rotateSpeed;

	if (target_) {

		num += 0.05f;

		if (num >= 1.0f) {
			num = 1.0f;
		}

		float T = Easing::easeInSine(num);

		interTarget_ = Lerp(T, interTarget_, target_->translation_);

		Vector3 offset = OffsetCalc();
		

		viewProjection_.translation_ = interTarget_ + offset;
	}



	viewProjection_.UpdateViewMatrix();
}

void FollowCamera::Reset() {

	if (target_) {
		interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}

	Vector3 offset = OffsetCalc();
	viewProjection_.translation_ = interTarget_ + offset;
	num = 0.0f;

}

Vector3 FollowCamera::OffsetCalc() const {

	Vector3 offset = { 0.0f, 5.0f, -30.0f };

	Matrix4x4 rotateMatrix = MakeRotateXMatrix(viewProjection_.rotation_.x) *
		                     MakeRotateYMatrix(viewProjection_.rotation_.y) *
		                     MakeRotateZMatrix(viewProjection_.rotation_.z);

	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}