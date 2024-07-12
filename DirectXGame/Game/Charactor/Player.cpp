#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"
#include "TextureManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "GameScene.h"

#include "Enemy.h"

const std::array<Player::ComboAttack, Player::comboNum_> Player::kComboAttacks_ = {
	{
		{30,20,5},
		{30,20,5},
		{30,20,10}
	}
};

void Player::Init(std::vector<std::shared_ptr<Model>> models){

	animationModels_ = models;
	
	obj_.reset(SkinningObject::Create(animationModels_[action_]));
	skinClusters_.resize(animationModels_.size());
	for (size_t index = 0; index < Action::kActionNum; index++) {
		animations_.emplace_back(AnimationManager::Load(animationModels_[index]->name_));
		skeletons_.emplace_back(Skeleton::Create(animationModels_[index]->rootNode_));
		skinClusters_[index].Create(skeletons_[index], animationModels_[index]);
	}
	obj_->SetSkinCluster(&skinClusters_[action_]);

	behaviorRequest_ = Behavior::kRoot;

	ColliderUpdate();

	//行列更新
	Matrix4x4 S = MakeScaleMatrix(obj_->worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(obj_->worldTransform_.translation_);
	obj_->worldTransform_.matWorld_ = S * rotateMat_ * T;
}

void Player::Update(){

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		    case Behavior::kRoot:
				RootInit();
			    break;
			case Behavior::kAttack:
				AttackInit();
				break;
			case Behavior::kDash:
				DashInit();
				break;
		}
		
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
		RootUpdate();
		break;
	case Behavior::kAttack:
		AttackUpdate();
		break;
	case Behavior::kDash:
		DashUpdate();
		break;
	}


	//行列更新
	Matrix4x4 S = MakeScaleMatrix(obj_->worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(obj_->worldTransform_.translation_);
	obj_->worldTransform_.matWorld_ = S * rotateMat_ * T;

	animations_[action_].Play(skeletons_[action_]);
	skeletons_[action_].Update();
	skinClusters_[action_].Update(skeletons_[action_]);

	ColliderUpdate();
}

void Player::Draw(const Camera& camera){

#ifdef _DEBUG
	ShapesDraw::DrawAABB(collider_, camera);
#endif // _DEBUG


	obj_->Draw(camera);
	skeletons_[action_].Draw(obj_->worldTransform_, camera);

}

void Player::DrawParticle(const Camera& camera) {

}

void Player::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	size_ = data.colliderSize;


	Matrix4x4 S = MakeScaleMatrix(obj_->worldTransform_.scale_);
	rotateMat_ = MakeRotateXMatrix(data.rotation.x) * MakeRotateYMatrix(data.rotation.y) * MakeRotateZMatrix(data.rotation.z);
	Matrix4x4 T = MakeTranslateMatrix(obj_->worldTransform_.translation_);
	obj_->worldTransform_.matWorld_ = S * rotateMat_ * T;
	rotate_ = Transform(rotate_, rotateMat_);
}

void Player::OnCollision() {
	life_--;
}

void Player::RootInit() {

	isAttack_ = false;
	action_ = Action::Standing;
	obj_->SetSkinCluster(&skinClusters_[action_]);

}

void Player::RootUpdate() {

	Vector3 move{};
	Vector3 zeroVector{};
	const float speed = 0.3f;

	move = Input::GetInstance()->GetMoveXZ();
	move = move / SHRT_MAX * speed;

	//攻撃
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_X)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	////ダッシュ
	//if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
	//	behaviorRequest_ = Behavior::kDash;
	//}

	move = TransformNormal(move, MakeRotateYMatrix(followCamera_->GetCamera().rotation_.y));

	obj_->worldTransform_.translation_ += move;
	obj_->worldTransform_.translation_.y = 0.0f;

	if (move != zeroVector) {
		rotate_ = move;
		action_ = Action::Walking;
	}
	else {
		action_ = Action::Standing;
	}
	obj_->SetSkinCluster(&skinClusters_[action_]);

	//worldTransform_.rotation_.y = std::atan2(rotate_.x, rotate_.z);

	rotateMat_ = DirectionToDirection(from_, rotate_);

}

void Player::AttackInit() {

	action_ = Action::Attack;
	obj_->SetSkinCluster(&skinClusters_[action_]);
	animations_[action_].SetAnimationSpeed(1.0f / 30.0f);
	skeletons_[action_].Update();
	workAttack_.attackParam_ = 0;
	workAttack_.speed_ = 1.0f;
	//Search(enemies);

	/*if (target_) {

		Vector3 direction = target_->GetWorldTransform().translation_ - worldTransform_.translation_;

		rotateMat_ = DirectionToDirection(from_, direction);
	}*/
	
}

void Player::AttackUpdate() {

	if (workAttack_.comboIndex_ < comboNum_ - 1) {
		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_X)) {
			workAttack_.comboNext_ = true;
		}
	}

	uint32_t totalAttackTime = kComboAttacks_[workAttack_.comboIndex_].attackTime_;
	totalAttackTime += kComboAttacks_[workAttack_.comboIndex_].chargeTime_;
	totalAttackTime += kComboAttacks_[workAttack_.comboIndex_].recoveryTime_;

	if (workAttack_.attackParam_ >= kComboAttacks_[workAttack_.comboIndex_].chargeTime_ + kComboAttacks_[workAttack_.comboIndex_].attackTime_) {
		workAttack_.InComboPhase_ = 2;
	}else if (workAttack_.attackParam_ >= kComboAttacks_[workAttack_.comboIndex_].chargeTime_) {
		workAttack_.InComboPhase_ = 1;
	}else {
		workAttack_.InComboPhase_ = 0;
	}

	if (workAttack_.InComboPhase_ == 1) {
		isAttack_ = true;
	}else {
		isAttack_ = false;
	}

	if (workAttack_.attackParam_ >= totalAttackTime) {
		animations_[action_].TimeReset();
		if (workAttack_.comboNext_) {
			workAttack_.comboNext_ = false;
			workAttack_.comboIndex_++;

			AttackInit();
		}
		else {
			behaviorRequest_ = Behavior::kRoot;
			//workAttack_.isHit_ = false;
			workAttack_.comboIndex_ = 0;
			return;
		}
	}

	
	if (workAttack_.attackParam_ == kComboAttacks_[workAttack_.comboIndex_].chargeTime_) {

		Vector3 direction = { 0.0f,0.0f,1.0f };
		direction = TransformNormal(direction, rotateMat_);
		workAttack_.velocity_ = direction.Normalize() * workAttack_.speed_;

		///
		
		PlayerAttack* playerAttack = new PlayerAttack();
		std::shared_ptr<Model> attackModel = ModelManager::LoadOBJ("PlayerBullet");
		Vector3 offset = { 0.0f,0.0f,1.0f };
		offset = TransformNormal(offset, rotateMat_);
		Vector3 shotPos = Transform(skeletons_[action_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_) + offset;
		playerAttack->Init(attackModel, shotPos, direction);
		gameScene_->AddPlayerAttack(playerAttack);

		///

		
	}

	workAttack_.attackParam_++;
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

	obj_->worldTransform_.translation_ += workDash_.dashDirection_.Normalize() * dashSpeed;

	if (++workDash_.dashParam_ >= workDash_.dashTime_) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

//void Player::Search(const std::list<std::unique_ptr<Enemy>>& enemies) {
//
//	if (enemies.empty()) {
//		target_ = nullptr;
//		return;
//	}
//
//	//目標
//	///<playerとの距離、敵のポインタ>
//	std::list<std::pair<float, Enemy*>> targets;
//
//	for (const std::unique_ptr<Enemy>& enemy : enemies) {
//		
//		Vector3 distance = enemy->GetWorldTransform().translation_ - worldTransform_.translation_;
//
//		if (attackRange_ >= distance.Length()) {
//
//			targets.emplace_back(std::make_pair(distance.Length(), enemy.get()));
//
//		}
//
//		target_ = nullptr;
//		if (!targets.empty()) {
//			targets.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
//			target_ = targets.front().second;
//		}
//	}
//}

Vector3 Player::GetWorldPos() const{

	Vector3 worldPos;

	worldPos.x = obj_->worldTransform_.matWorld_.m[3][0];
	worldPos.y = obj_->worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = obj_->worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
