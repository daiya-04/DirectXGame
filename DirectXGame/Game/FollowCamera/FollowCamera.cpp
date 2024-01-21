#include "FollowCamera.h"

#include "Game/Player/Player.h"

void FollowCamera::Init() {
	viewProjection_.Init();

	input = Input::GetInstance();

	workInter.interParameter_ = 0.2f;

	const float kRadian = 0.02f;

	viewProjection_.rotation_.x = 10 * kRadian;
	viewProjection_.UpdateMatrix();
}


void FollowCamera::Update() {
	
	if (target_) {
		Vector3 pos = target_->translation_;
		//もしペアレントを結んでいるなら
		if (target_->parent_) {
			pos = target_->translation_ + target_->parent_->translation_;
		}
		//追従座標の補間
		workInter.interTarget_ = Lerp(workInter.interParameter_,workInter.interTarget_, pos);

		Vector3 offset = OffsetCalc();
		//オフセット分と追従座標の補間分ずらす
		viewProjection_.translation_ = workInter.interTarget_ + offset;
	}
	if (Behavior::kGrap == player_->GetBehavior()) {
		viewProjection_.rotation_.x = 0.0f;
		//ロックオン座標
		Vector3 lockOnPos = player_->GetPosition();
		if (GrapBehavior::kLeft == player_->GetGrapBehavior()) {
			lockOnPos.z += 1.0f;
			lockOnPos.x -= 2.0f;
		}
		else if (GrapBehavior::kRight == player_->GetGrapBehavior()) {
			lockOnPos.z += 1.0f;
			lockOnPos.x += 2.0f;
		}

		Vector3 sub = lockOnPos - workInter.interTarget_;

		if (sub.z != 0.0) {
			rotate_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

			if (sub.z < 0.0) {
				rotate_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> -rotate_ : -std::numbers::pi_v<float> -rotate_;
			}
		}
		else {
			rotate_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
		}

		parameter_t = 0.1f;
	}
	else {
		rotate_ = 0.0f;

		parameter_t = 0.1f;

		const float kRadian = 0.02f;

		viewProjection_.rotation_.x = 10 * kRadian;
	}
	viewProjection_.rotation_.y = LerpShortAngle(viewProjection_.rotation_.y, rotate_, parameter_t);
	viewProjection_.UpdateMatrix();
}

void FollowCamera::SetTarget(const WorldTransform* target)
{
	target_ = target;
	Reset();
}

void FollowCamera::Reset()
{
	//追従対象がいれば
	if (target_) {
		//追従座標・角度の初期化
		workInter.interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}
	workInter.targetAngleY_ = viewProjection_.rotation_.y;

	//追従大賞からのオフセット
	Vector3 offset = OffsetCalc();
	viewProjection_.translation_ = workInter.interTarget_ + offset;
}

Vector3 FollowCamera::OffsetCalc()
{
	Vector3 offset = { 0.0f, 5.0f, -25.0f };
	//回転行列の合成
	Matrix4x4 rotateMatrix = MakeRotateXMatrix(viewProjection_.rotation_.x) * MakeRotateYMatrix(viewProjection_.rotation_.y) * MakeRotateZMatrix(viewProjection_.rotation_.z);

	// オフセットをカメラの回転に合わせて回転
	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

float FollowCamera::LerpShortAngle(float a, float b, float t)
{
	//角度差分を求める
	float diff = b - a;
	diff = std::fmod(diff, 2 * (float)std::numbers::pi);
	if (diff < 2 * -(float)std::numbers::pi) {
		diff += 2 * (float)std::numbers::pi;
	}
	else if (diff >= 2 * (float)std::numbers::pi) {
		diff -= 2 * (float)std::numbers::pi;
	}

	diff = std::fmod(diff, 2 * (float)std::numbers::pi);
	if (diff < -(float)std::numbers::pi) {
		diff += 2 * (float)std::numbers::pi;
	}
	else if (diff >= (float)std::numbers::pi) {
		diff -= 2 * (float)std::numbers::pi;
	}

	return a + diff * t;
}