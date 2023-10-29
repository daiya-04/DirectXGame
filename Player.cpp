#include "Player.h"
#include "externals/imgui/imgui.h"

void Player::Initialize(const std::vector<Object3d*>& models) {

	models_ = models;
	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };
	partsWorldTransform_[Head].translation_ = { 0.0f,6.5f,0.0f };

	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];

	worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Player::Update() {

	//XINPUT_STATE joyState;
	Vector3 move{};
	Vector3 zeroVector{};

	if (Input::GetInstance()->GetJoystickState()) {

		
		move = Input::GetInstance()->GetMoveXZ();
		move = move / SHRT_MAX * speed;

		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A) && isJamp_ == false) {
			isJamp_ = true;
			velocity_.y = 1.0f;
		}
		velocity_.y += -0.05f;
		
		move = TransformNormal(move, MakeRotateYMatrix(viewProjection_->rotation_.y));

		worldTransform_.translation_ += move + velocity_;

		if (move != zeroVector) {
			rotate_ = move;
		}

		worldTransform_.rotation_.y = std::atan2(rotate_.x, rotate_.z);

	}

	if (worldTransform_.translation_.y<= -20.0f) {
		worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
	}




	//行列更新
	worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Player::Draw(const ViewProjection& viewProjection) {

	for (size_t index = 0; index < models_.size(); index++) {
		models_[index]->Draw(partsWorldTransform_[index], viewProjection);
	}

}
void Player::OnGround() {
	velocity_ = {};
	worldTransform_.translation_.y = 0.0f;
	isJamp_ = false;

}

void Player::ReStart() {
	worldTransform_.translation_ = {};
	worldTransform_.translation_ = {};
}

Vector3 Player::GetWorldPos() const{
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}