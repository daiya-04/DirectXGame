#include "FollowCamera.h"
#include <cmath>

void FollowCamera::Initialize() {

	

}

void FollowCamera::Update() {

	const float rotateSpeed = 0.02f;

	if (Input::GetInstance()->GetJoystickState()) {

		viewProjection_.rotation_ += Input::GetInstance()->GetCameraRotate() * rotateSpeed;

	}

	if (target_) {

		Vector3 offset = { 0.0f, 5.0f, -30.0f };

		Matrix4x4 rotateMatrix = MakeRotateXMatrix(viewProjection_.rotation_.x) *
			                     MakeRotateYMatrix(viewProjection_.rotation_.y) *
			                     MakeRotateZMatrix(viewProjection_.rotation_.z);

		offset = TransformNormal(offset, rotateMatrix);

		viewProjection_.translation_ = target_->translation_ + offset;

		

	}

	viewProjection_.UpdateViewMatrix();
}
