#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"
#include "TextureManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "GameScene.h"

const std::array<Player::ComboAttack, Player::comboNum_> Player::kComboAttacks_ = {
	{
		{30,20,5},
		{30,20,5},
		{30,20,10}
	}
};

void Player::Init(std::vector<std::shared_ptr<Model>> models){

	animationModels_ = models;
	hpBerTex_ = TextureManager::Load("Player_HP.png");
	
	obj_.reset(SkinningObject::Create(animationModels_[action_]));
	skinClusters_.resize(animationModels_.size());
	for (size_t index = 0; index < Action::kActionNum; index++) {
		animations_.emplace_back(AnimationManager::Load(animationModels_[index]->name_));
		skeletons_.emplace_back(Skeleton::Create(animationModels_[index]->rootNode_));
		skinClusters_[index].Create(skeletons_[index], animationModels_[index]);
	}
	obj_->SetSkinCluster(&skinClusters_[action_]);

	behaviorRequest_ = Behavior::kRoot;

	life_ = maxHp_;

	hpBer_.reset(Sprite::Create(hpBerTex_, { 440.0f,700.0f }));
	hpBer_->SetAnchorpoint({ 0.0f,0.5f });
	hpBer_->SetSize({ 400.0f,10.0f });

	/*attackEff_.reset(GPUParticle::Create(TextureManager::Load("particle.png"), 20000));

	attackEff_->isLoop_ = false;
	attackEff_->emitter_.count = 10000;
	attackEff_->emitter_.color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	attackEff_->emitter_.angle = 60.0f;
	attackEff_->emitter_.emit = 0;
	attackEff_->emitter_.size = Vector3(0.0f, 0.0f, 0.0f);
	attackEff_->emitter_.scale = 0.05f;
	attackEff_->emitter_.lifeTime = 5.0f;
	attackEff_->emitter_.speed = 1.0f;*/

	ColliderUpdate();

	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Player::Update(){

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		behaviorInitTable_[behavior_]();
		
		behaviorRequest_ = std::nullopt;
	}

	behaviorUpdateTable_[behavior_]();

	obj_->worldTransform_.translation_.x = std::clamp(obj_->worldTransform_.translation_.x, -50.0f, 50.0f);
	obj_->worldTransform_.translation_.z = std::clamp(obj_->worldTransform_.translation_.z, -20.0f, 80.0f);


	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);

	animations_[action_].Play(skeletons_[action_]);
	skeletons_[action_].Update();
	skinClusters_[action_].Update(skeletons_[action_]);

	//attackEff_->Update();

	UIUpdate();
	ColliderUpdate();
}

void Player::UIUpdate() {

	float percent = (float)life_ / (float)maxHp_;

	hpBer_->SetSize({ 400.0f * percent,10.0f });
}

void Player::Draw(const Camera& camera){

#ifdef _DEBUG
	ShapesDraw::DrawAABB(collider_, camera);
#endif // _DEBUG


	obj_->Draw(camera);
	skeletons_[action_].Draw(obj_->worldTransform_, camera);

}

void Player::DrawParticle(const Camera& camera) {
	//attackEff_->Draw(camera);
}

void Player::DrawUI() {
	hpBer_->Draw();
}

void Player::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	size_ = data.colliderSize;


	
	rotateMat_ = MakeRotateXMatrix(data.rotation.x) * MakeRotateYMatrix(data.rotation.y) * MakeRotateZMatrix(data.rotation.z);
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
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

	if (target_) {

		Vector3 direction = target_->translation_ - obj_->worldTransform_.translation_;

		if (direction.Length() <= attackRange_) {
			rotateMat_ = DirectionToDirection(from_, direction);
		}
		
	}
	
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

		/*attackEff_->emitter_.emit = 1;
		attackEff_->emitter_.translate = shotPos;
		attackEff_->emitter_.direction = direction.Normalize();*/
		
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

Vector3 Player::GetWorldPos() const{

	Vector3 worldPos;

	worldPos.x = obj_->worldTransform_.matWorld_.m[3][0];
	worldPos.y = obj_->worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = obj_->worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
