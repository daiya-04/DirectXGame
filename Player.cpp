#include "Player.h"
#include "externals/imgui/imgui.h"

void (Player::* Player::BehaviorTable[])() = {
	&Player::RootUpdate,
	&Player::AttackUpdate,
	&Player::JampUpdate,
	&Player::DashUpdate,
};

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

	

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		    case Behavior::kRoot:
			default:
				RootInitialize();
				break;
			case Behavior::kAttack:
				AttackInitialize();
				break;
			case Behavior::kJamp:
				JampInitialize();
				break;
			case Behavior::kDash:

				break;
		}

		behaviorRequest_ = std::nullopt;
	}
	
	(this->*BehaviorTable[static_cast<size_t>(behavior_)])();
	
	velocity_.y += -0.05f;
	

	if (worldTransform_.translation_.y <= -20.0f) {
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
	//velocity_ = {};
	worldTransform_.translation_.y = 0.0f;
	if (behavior_ == Behavior::kJamp) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Player::ReStart() {
	worldTransform_.translation_ = {};
	worldTransform_.translation_ = {};
}

void Player::RootInitialize() {



}

void Player::RootUpdate() {

	Vector3 move{};
	Vector3 zeroVector{};

	velocity_ = Input::GetInstance()->GetMoveXZ();
	velocity_ = velocity_ / SHRT_MAX * speed;

	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {
		behaviorRequest_ = Behavior::kJamp;
	}
	

	velocity_ = TransformNormal(velocity_, MakeRotateYMatrix(viewProjection_->rotation_.y));

	//velocity_ = move;

	worldTransform_.translation_ += velocity_;

	if (velocity_ != zeroVector) {
		//worldTransform_.rotation_ = move;
		rotate_ = velocity_;
	}

	worldTransform_.rotation_.y = std::atan2(rotate_.x, rotate_.z);

	

}

void Player::AttackInitialize() {



}

void Player::AttackUpdate() {



}

void Player::JampInitialize() {

	worldTransform_.translation_.y = 0.0f;

	velocity_.y = 1.0f;

}

void Player::JampUpdate() {

	worldTransform_.translation_ += velocity_;

}

void Player::DashInitialize() {



}

void Player::DashUpdate() {



}


Vector3 Player::GetWorldPos() const{
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}