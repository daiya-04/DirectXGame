#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"

void (Player::* Player::BehaviorTable[])() = {
	& Player::RootUpdate,
	& Player::AttackUpdate,
	& Player::DashUpdate,
};

void (Player::* Player::RequestTable[])() = {
	&Player::RootInit,
	& Player::AttackInit,
	& Player::DashInit,
};

void Player::Init(std::vector<uint32_t> modelHandles){

	worldTransform_.Init();
	partsWorldTransform_[Body].Init();
	partsWorldTransform_[Head].Init();

	obj_.resize(modelHandles.size());
	for (size_t index = 0; index < obj_.size(); ++index) {
		obj_[index] = std::make_unique<Object3d>();
		obj_[index].reset(Object3d::Create(modelHandles[index]));
	}

	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };
	partsWorldTransform_[Head].translation_ = { 0.0f,6.5f,0.0f };	

	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];

}

void Player::Update(){

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		(this->*RequestTable[static_cast<size_t>(behavior_)])();

		behaviorRequest_ = std::nullopt;
	}

	(this->*BehaviorTable[static_cast<size_t>(behavior_)])();

	

	//行列更新
	Matrix4x4 S = MakeScaleMatrix(worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.matWorld_ = S * rotateMat_ * T;
	//worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Player::Draw(const Camera& camera){

	obj_[Body]->Draw(partsWorldTransform_[Body], camera);
	obj_[Head]->Draw(partsWorldTransform_[Head], camera);

}

void Player::RootInit() {



}

void Player::RootUpdate() {

	Vector3 move{};
	Vector3 zeroVector{};
	const float speed = 0.3f;

	move = Input::GetInstance()->GetMoveXZ();
	move = move / SHRT_MAX * speed;

	//ダッシュ
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {
		behaviorRequest_ = Behavior::kDash;
	}

	move = TransformNormal(move, MakeRotateYMatrix(followCamera_->GetCamera().rotation_.y));

	worldTransform_.translation_ += move;

	if (move != zeroVector) {
		rotate_ = move;
	}

	//worldTransform_.rotation_.y = std::atan2(rotate_.x, rotate_.z);

	rotateMat_ = DirectionToDirection(from_, rotate_);

}

void Player::AttackInit() {

	

}

void Player::AttackUpdate() {



}

void Player::DashInit() {

	workDash_.dashParam_ = 0;
	workDash_.dashDirection_ = rotate_;
	followCamera_->Reset();

}

void Player::DashUpdate() {

	float dashSpeed = 1.5f;
	Vector3 zeroVector{};

	if (rotate_ == zeroVector) {
		workDash_.dashDirection_ = { 0.0f,0.0f,1.0f };
	}

	worldTransform_.translation_ += workDash_.dashDirection_.Normalize() * dashSpeed;

	if (++workDash_.dashParam_ >= workDash_.dashTime_) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

Vector3 Player::GetWorldPos() const{

	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
