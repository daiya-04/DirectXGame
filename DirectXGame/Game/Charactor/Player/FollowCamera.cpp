#include "FollowCamera.h"
#include "Input.h"
#include <cmath>
#include <numbers>
#include "ImGuiManager.h"
#include "Easing.h"
#include "LockOn.h"

void FollowCamera::Init(){
	//カメラ初期化
	camera_.Init();
	//行列更新
	camera_.UpdateViewMatrixRotate(rotateMat_);
}

void FollowCamera::Update(){

	Matrix4x4 subRMat = MakeIdentity44();

	if (lockOn_->ExistTarget()) {

		Vector3 lockOnPos = lockOn_->GetTargetPos();

		Vector3 sub = lockOnPos - target_->translation_;

		halfPos_ = target_->translation_ + (sub.GetXZ() / 2.0f);

		rotateMat_ = DirectionToDirection(Vector3(0.0f, 0.0f, 1.0f), sub.GetXZ());

		Vector3 offset = baseOffset_;
		if (isZoom_) {
			zoomParam_ += zoomSpeed_;

			Vector3 ltoc = lockOnPos - camera_.translation_;
			Vector3 htoc = halfPos_ - camera_.translation_;

			subRMat = DirectionToDirection(ltoc.GetXZ(), htoc.GetXZ());
		}
		else {
			zoomParam_ -= zoomSpeed_;
		}

		zoomParam_ = std::clamp(zoomParam_, 0.0f, 1.0f);
		float T = Easing::easeInOutQuart(zoomParam_);
		offset = Lerp(T, baseOffset_, attackOffset_);

		Matrix4x4 rotateMatrix;
		rotateMatrix = MakeRotateXMatrix(rotation_.x) *
					   DirectionToDirection({ 0.0f,0.0f,-1.0f }, -sub.GetXZ()) *
					   MakeRotateZMatrix(rotation_.z);

		offset = TransformNormal(offset, rotateMatrix);

		//ダッシュしたときにカメラを遅延させる
		delayParam_ += 1.0f / static_cast<float>(delayTime_);

		delayParam_ = std::clamp(delayParam_, 0.0f, 1.0f);

		T = Easing::easeInSine(delayParam_);
		uint32_t dampingRate = baseDampingRate_;
		if (isZoom_) {
			dampingRate = zoomDampingRate_;
		}else if (isDash_) {
			dampingRate = dashDampingRate_;
		}
		//補間
		interTarget_ = Lerp(deltaTime_ * static_cast<float>(dampingRate), interTarget_, target_->translation_);


		camera_.translation_ = interTarget_ + offset;

		
	}
	else {
		const float rotateSpeed = 0.02f;
		const float rotateMax = 45.0f;
		const float rotateMin = -5.0f;

		rotation_ += DaiEngine::Input::GetInstance()->GetCameraRotate() * rotateSpeed;
		//角度制限
		rotation_.x = min(rotation_.x, rotateMax * std::numbers::pi_v<float> / 180.0f);
		rotation_.x = max(rotation_.x, -rotateMin * std::numbers::pi_v<float> / 180.0f);

		if (target_) {
			//ダッシュしたときにカメラを遅延させる
			delayParam_ += 1.0f / static_cast<float>(delayTime_);

			delayParam_ = std::clamp(delayParam_, 0.0f, 1.0f);

			float T = Easing::easeInSine(delayParam_);
			uint32_t dampingRate = baseDampingRate_;
			if (isZoom_) {
				dampingRate = zoomDampingRate_;
			}
			else if (isDash_) {
				dampingRate = dashDampingRate_;
			}
			//補間
			interTarget_ = Lerp(deltaTime_ * static_cast<float>(dampingRate), interTarget_, target_->translation_);

			Vector3 offset = OffsetCalc();


			camera_.translation_ = interTarget_ + offset;
		}
	}

	camera_.UpdateViewMatrixRotate(rotateMat_ * subRMat);
}

void FollowCamera::Reset() {

	if (target_) {
		interTarget_ = target_->translation_;
	}

	Vector3 offset = OffsetCalc();
	camera_.translation_ = interTarget_ + offset;
	delayParam_ = 0.0f;

}

Vector3 FollowCamera::OffsetCalc() {
	Vector3 offset = baseOffset_;

	//回転行列計算
	rotateMat_ = MakeRotateXMatrix(rotation_.x) *
		           MakeRotateYMatrix(rotation_.y) *
		           MakeRotateZMatrix(rotation_.z);

	offset = TransformNormal(offset, rotateMat_);

	return offset;
}

void FollowCamera::SetTarget(const DaiEngine::WorldTransform* target) {
	target_ = target;
	//オフセット計算
	Vector3 offset = OffsetCalc();
	camera_.translation_ = target_->translation_ + offset;
	//行列更新
	camera_.UpdateViewMatrix();
}

void FollowCamera::NormalInit() {



}

void FollowCamera::NormalUpdate() {

	

}

void FollowCamera::AttackInit() {

	workAttack_.param_ = 0.0f;

}

void FollowCamera::AttackUpdate() {

	

}
