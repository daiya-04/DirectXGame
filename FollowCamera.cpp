#include "FollowCamera.h"
#include <cmath>
#include <numbers>
#include "Easing.h"
#include "GlobalVariables.h"
#include "LockOn.h"
#include "ImGuiManager.h"

void FollowCamera::Initialize() {

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->AddItem(groupName, "delayTime", (int)delayTime_);

}

void FollowCamera::Update() {

	ApplyGlobalVariables();

	ImGui::Begin("Camera");

	ImGui::DragFloat3("translation", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &viewProjection_.rotation_.x, 0.01f);

	ImGui::End();

	

	if (lockOn_->ExistTarget()) {
		
		Vector3 lockOnPos = lockOn_->GetTargetPos();

		Vector3 sub = lockOnPos - target_->translation_;
		sub.y = 0.0f;

		//viewProjection_.rotation_.y = std::atan2(sub.x, sub.z);
		rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, sub);

		//Vector3 offset = OffsetCalc();
		Vector3 offset = { 0.0f, 5.0f, -30.0f };
		Matrix4x4 rotateMatrix;
		rotateMatrix = MakeRotateXMatrix(viewProjection_.rotation_.x) *
			          DirectionToDirection({ 0.0f,0.0f,-1.0f }, -sub) *
			           MakeRotateZMatrix(viewProjection_.rotation_.z);


		offset = TransformNormal(offset, rotateMatrix);

		viewProjection_.translation_ = target_->translation_ + offset;

	} else {
		const float rotateSpeed = 0.02f;

		viewProjection_.rotation_ += Input::GetInstance()->GetCameraRotate() * rotateSpeed;

		viewProjection_.rotation_.x = min(viewProjection_.rotation_.x, 89.99f * (float)std::numbers::pi / 180.0f);
		viewProjection_.rotation_.x = max(viewProjection_.rotation_.x, -5.0f * (float)std::numbers::pi / 180.0f);

		if (target_) {

			num += (float)1 / delayTime_;

			if (num >= 1.0f) {
				num = 1.0f;
			}

			float T = Easing::easeInSine(num);

			interTarget_ = Lerp(T, interTarget_, target_->translation_);

			Vector3 offset = OffsetCalc();


			viewProjection_.translation_ = interTarget_ + offset;
		}
	}

	
	Matrix4x4 S = MakeScaleMatrix({ 1.0f,1.0f,1.0f });
	Matrix4x4 R;
	if (lockOn_->ExistTarget()) {
		R = MakeRotateXMatrix(viewProjection_.rotation_.x) * rotateMat_ * MakeRotateZMatrix(viewProjection_.rotation_.z);
	}else {
		R = MakeRotateXMatrix(viewProjection_.rotation_.x) * MakeRotateYMatrix(viewProjection_.rotation_.y) * MakeRotateZMatrix(viewProjection_.rotation_.z);
	}
	
	Matrix4x4 T = MakeTranslateMatrix(viewProjection_.translation_);
	viewProjection_.matView_ = (S * R * T).Inverse();
	//viewProjection_.UpdateViewMatrix();
}

void FollowCamera::Reset() {

	if (target_) {
		interTarget_ = target_->translation_;
		//viewProjection_.rotation_.y = target_->rotation_.y;
	}

	Vector3 offset = OffsetCalc();
	viewProjection_.translation_ = interTarget_ + offset;
	num = 0.0f;

}

void FollowCamera::ApplyGlobalVariables() {

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	delayTime_ = globalVariables->GetIntValue(groupName, "delayTime");

}

Vector3 FollowCamera::OffsetCalc() const {

	Vector3 offset = { 0.0f, 5.0f, -30.0f };
	Matrix4x4 rotateMatrix;
	
	rotateMatrix = MakeRotateXMatrix(viewProjection_.rotation_.x) *
		MakeRotateYMatrix(viewProjection_.rotation_.y) *
		MakeRotateZMatrix(viewProjection_.rotation_.z);

	

	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
}