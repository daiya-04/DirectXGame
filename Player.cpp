#include "Player.h"
#include "imgui.h"
#include "Easing.h"
#include <cmath>
#include <numbers>
#include "GlobalVariables.h"

void (Player::* Player::BehaviorTable[])() = {
	&Player::RootUpdate,
	&Player::AttackUpdate,
	&Player::DashUpdate,
};

void Player::Initialize(const std::vector<Object3d*>& models) {

	models_ = models;
	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };
	partsWorldTransform_[Head].translation_ = { 0.0f,6.5f,0.0f };

	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	
	globalVariables->AddItem(groupName, "Dash Time", (int)workDash_.dashTime_);

	worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Player::Update() {

	ApplyGlobalVariables();

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
			case Behavior::kDash:
				DashInitialize();
				break;
		}

		behaviorRequest_ = std::nullopt;
	}
	
	(this->*BehaviorTable[static_cast<size_t>(behavior_)])();
	
//#ifdef _DEBUG
//	ImGui::Begin("player");
//
//	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
//	ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
//
//	ImGui::DragFloat3("stamptrnslation", &weaponCollision_.translation_.x, 0.01f);
//	ImGui::Text("stamp pos : %f %f %f", weaponCollision_.matWorld_.m[3][0], weaponCollision_.matWorld_.m[3][1], weaponCollision_.matWorld_.m[3][2]);
//	ImGui::DragFloat3("stamp rotate", &weaponCollision_.rotation_.x, 0.01f);
//
//	ImGui::End();
//#endif // _DEBUG

	
	

	if (worldTransform_.translation_.y <= -20.0f) {
		worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
	}


	//行列更新
	worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
	weaponCollision_.UpdateMatrix();
	weaponWorldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {

	for (size_t index = 0; index < models_.size() - 1; index++) {
		models_[index]->Draw(partsWorldTransform_[index], viewProjection);
	}

	if (behavior_ == Behavior::kAttack) {
		models_[models_.size() - 1]->Draw(weaponWorldTransform_, viewProjection);
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
	behaviorRequest_ = Behavior::kRoot;
	followCamera_->Reset();
}

void Player::RootInitialize() {



}

void Player::RootUpdate() {

	Vector3 move{};
	Vector3 zeroVector{};

	move = Input::GetInstance()->GetMoveXZ();
	move = move / SHRT_MAX * speed;
	//ジャンプ
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A) && isJamp_ == false) {
		isJamp_ = true;
		velocity_.y = 1.0f;
	}
	//攻撃
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_X) && isJamp_ == false) {
		behaviorRequest_ = Behavior::kAttack;
	}
	//ダッシュ
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && isJamp_ == false) {
		behaviorRequest_ = Behavior::kDash;
	}
	
	velocity_.y += -0.05f;

	move = TransformNormal(move, MakeRotateYMatrix(viewProjection_->rotation_.y));

	worldTransform_.translation_ += move + velocity_;

	if (move != zeroVector) {
		rotate_ = move;
	}

	worldTransform_.rotation_.y = std::atan2(rotate_.x, rotate_.z);
	

}

void Player::AttackInitialize() {

	weaponWorldTransform_.translation_ = worldTransform_.translation_;
	weaponCollision_.translation_ = worldTransform_.translation_;
	weaponWorldTransform_.rotation_ = worldTransform_.rotation_;
	weaponCollision_.rotation_ = worldTransform_.rotation_;
	weaponWorldTransform_.translation_.y = 3.0f;
	weaponCollision_.translation_.y = 6.0f;
	workAttack_.attackParameter_ = 0.0f;
	workAttack_.coolTime_ = 0;
	workAttack_.theta_ = 0.0f;
	workAttack_.isAttack_ = true;

}

void Player::AttackUpdate() {

	workAttack_.attackParameter_ += 0.05f;
	float T = Easing::easeInSine(workAttack_.attackParameter_);
	weaponWorldTransform_.rotation_.x = Lerp(T, 0.0f, (float)std::numbers::pi / 2.0f);
	workAttack_.theta_ = Lerp(T, 0.0f, (float)std::numbers::pi / 2.0f);

	Vector3 distance = { 0.0f,9.0f + weaponWorldTransform_.translation_.y ,0.0f };

	Vector3 move = {
		0.0f,
		distance.y * std::cosf(workAttack_.theta_) - distance.z * std::sinf(workAttack_.theta_),
		distance.y * std::sinf(workAttack_.theta_) + distance.z * std::cosf(workAttack_.theta_),
	};

	move = TransformNormal(move, MakeRotateYMatrix(weaponCollision_.rotation_.y));

	weaponCollision_.translation_ = move + worldTransform_.translation_;

	if (workAttack_.attackParameter_ >= 1.0f) {
		workAttack_.attackParameter_ = 1.0f;
		workAttack_.coolTime_++;
	}

	if (workAttack_.coolTime_ == 15) {
		behaviorRequest_ = Behavior::kRoot;
		workAttack_.isAttack_ = false;
	}

}

void Player::DashInitialize() {

	workDash_.dashParameter_ = 0;
	workDash_.dashDirection_ = rotate_;
	followCamera_->Reset();

}

void Player::DashUpdate() {

	float dashSpeed = 2.0f;
	Vector3 zeroVector{};

	if (rotate_ == zeroVector) {
		workDash_.dashDirection_ = { 0.0f,0.0f,1.0f };
	}

	worldTransform_.translation_ += workDash_.dashDirection_.Normalize() * dashSpeed;

	

	if (++workDash_.dashParameter_ >= workDash_.dashTime_) {
		behaviorRequest_ = Behavior::kRoot;
	}

}


void Player::ApplyGlobalVariables(){

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	workDash_.dashTime_ = globalVariables->GetIntValue(groupName, "Dash Time");

}

Vector3 Player::GetWorldPos() const{
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Player::GetWeaponWorldPos() const {

	Vector3 worldPos;

	worldPos.x = weaponCollision_.matWorld_.m[3][0];
	worldPos.y = weaponCollision_.matWorld_.m[3][1];
	worldPos.z = weaponCollision_.matWorld_.m[3][2];

	return worldPos;

}