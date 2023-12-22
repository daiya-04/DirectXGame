#include "FollowCamera.h"
#include "Input.h"
#include <cmath>
#include <numbers>
#include "ImGuiManager.h"

void FollowCamera::Init(){

	camera_.Init();

}

void FollowCamera::Update(){

	const float rotateSpeed = 0.02f;

	camera_.rotation_ += Input::GetInstance()->GetCameraRotate() * rotateSpeed;

	camera_.rotation_.x = min(camera_.rotation_.x, 89.99f * (float)std::numbers::pi / 180.0f);
	camera_.rotation_.x = max(camera_.rotation_.x, -5.0f * (float)std::numbers::pi / 180.0f);

	if (target_) {

		Vector3 offset = { 0.0f, 5.0f, -30.0f };
		Matrix4x4 rotateMatrix;

		rotateMatrix = MakeRotateXMatrix(camera_.rotation_.x) *
			MakeRotateYMatrix(camera_.rotation_.y) *
			MakeRotateZMatrix(camera_.rotation_.z);


		offset = TransformNormal(offset, rotateMatrix);


		camera_.translation_ = target_->translation_ + offset;
	}

	camera_.UpdateViewMatrix();
}
