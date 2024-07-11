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
		{18,20,5},
		{18,20,5},
		{18,20,10}
	}
};

void Player::Init(std::vector<std::shared_ptr<Model>> models){

	uint32_t particleTex = TextureManager::Load("particle.png");
	uint32_t particleTex2 = TextureManager::Load("star.png");

	animationModels_ = models;
	
	obj_.reset(SkinningObject::Create(animationModels_[action_]));
	skinClusters_.resize(animationModels_.size());
	for (size_t index = 0; index < Action::kActionNum; index++) {
		animations_.emplace_back(AnimationManager::Load(animationModels_[index]->name_));
		skeletons_.emplace_back(Skeleton::Create(animationModels_[index]->rootNode_));
		skinClusters_[index].Create(skeletons_[index], animationModels_[index]);
	}
	obj_->SetSkinCluster(&skinClusters_[action_]);

	magicParticle_ = std::make_unique<Particle>();
	magicParticle_.reset(Particle::Create(particleTex, 10000));
	magicParticle2_ = std::make_unique<Particle>();
	magicParticle2_.reset(Particle::Create(particleTex2, 1));

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
	AttackColliderUpdate();
}

void Player::AttackColliderUpdate() {

	AttackCollider_.center = GetAttackPos();
	switch (workAttack_.comboIndex_) {
		case 0:
			AttackCollider_.radius = 2.0f;
			break;
		case 1:
			AttackCollider_.radius = 5.0f;
			break;
		case 2:
			AttackCollider_.radius = 12.0f;
			break;
	}

}

void Player::Draw(const Camera& camera){

#ifdef _DEBUG
	ShapesDraw::DrawAABB(collider_, camera);
#endif // _DEBUG


	obj_->Draw(camera);
	skeletons_[action_].Draw(obj_->worldTransform_, camera);

}

void Player::DrawParticle(const Camera& camera) {

	magicParticle_->Draw(particles_, camera);
	magicParticle2_->Draw(particles2_, camera,false);

}

void Player::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	baseStatus_.HP_ = data.status.HP;
	baseStatus_.power_ = data.status.power;
	baseStatus_.difense_ = data.status.defense;


	Matrix4x4 S = MakeScaleMatrix(obj_->worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(obj_->worldTransform_.translation_);
	obj_->worldTransform_.matWorld_ = S * rotateMat_ * T;
}

void Player::OnCollision() {
	life_--;
}

void Player::RootInit() {

	isAttack_ = false;
	particles_.clear();
	particles2_.clear();
	action_ = Action::Standing;
	obj_->SetSkinCluster(&skinClusters_[action_]);

}

void Player::RootUpdate() {

	Vector3 move{};
	Vector3 zeroVector{};
	const float speed = 0.5f;

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
		//パーティクルの要素の削除
		particles_.clear();

		/*if (target_) {
			emitter_.translate_ = target_->GetWorldPos();
		}*/

		Vector3 direction = { 0.0f,0.0f,1.0f };
		direction = TransformNormal(direction, rotateMat_);
		workAttack_.velocity_ = direction.Normalize() * workAttack_.speed_;
		
		/*Vector3 offset = { 0.0f,0.0f,10.0f };
		offset = TransformNormal(offset, rotateMat_);*/

		if (workAttack_.comboIndex_ == 0) {
			Vector3 offset = { 0.0f,2.0f,10.0f };
			offset = TransformNormal(offset, rotateMat_);
			emitter_.translate_ = obj_->worldTransform_.translation_ + offset;
		}
		else {
			Vector3 offset = { 0.0f,0.0f,1.0f };
			offset = TransformNormal(offset, rotateMat_);
			emitter_.translate_ = Transform(skeletons_[action_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_) + offset;
		}

		///
		
		PlayerAttack* playerAttack = new PlayerAttack();
		std::shared_ptr<Model> attackModel = ModelManager::LoadOBJ("PlayerBullet");
		Vector3 offset = { 0.0f,0.0f,1.0f };
		offset = TransformNormal(offset, rotateMat_);
		Vector3 shotPos = Transform(skeletons_[action_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_) + offset;
		playerAttack->Init(attackModel, shotPos, direction);
		gameScene_->AddPlayerAttack(playerAttack);

		///

		//速度とパーティクルの数の設定と生成
		switch (workAttack_.comboIndex_) {
		case 0:

			emitter_.count_ = 150;
			

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_ * 0.6f, particleVelocity_ * 0.6f);
				std::uniform_real_distribution<float> distVelocityY(-0.1f, 0.1f);
				//std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				float size = 0.8f;
				particle.worldTransform_.scale_ = { size,size,size };
				particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocity(randomEngine) };
				//particle.color_ = { 0.38f,0.85f,0.97f,1.0 };
				particle.color_ = { 0.0f,0.0f,1.0f,1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);
				
			}
			

			break;
		case 1:

			emitter_.count_ = 300;

			//for (size_t count = 0; count < emitter_.count_; count++) {

			//	std::uniform_real_distribution<float> distVelocity(-particleVelocity_, particleVelocity_);
			//	//std::uniform_real_distribution<float> distVelocityY(-0.1f, 0.1f);
			//	std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

			//	Particle::ParticleData particle;
			//	particle.worldTransform_.translation_ = emitter_.translate_;
			//	float size = 0.5f;
			//	particle.worldTransform_.scale_ = { size,size,size };
			//	particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocity(randomEngine) };
			//	particle.color_ = { 0.0f,0.0f,distColor(randomEngine),1.0 };
			//	particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
			//	particle.currentTime_ = 0.0f;

			//	particles_.push_back(particle);
			//}



			break;
		case 2:

			emitter_.count_ = 500;

			/*for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_ * 1.5f, particleVelocity_ * 1.5f);
				std::uniform_real_distribution<float> distVelocityY(0.0f, particleVelocity_);
				std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocity(randomEngine) };
				particle.color_ = { 0.0f,0.0f,distColor(randomEngine),1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);

				
			}*/
			break;
		}

		Particle::ParticleData particle;
		particle.worldTransform_.translation_ = emitter_.translate_;
		particle.worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
		particle.velocity_ = {};
		particle.color_ = { 0.71f,0.94f,1.0f,1.0f };
		particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
		particle.currentTime_ = 0.0f;

		particles2_.push_back(particle);
	}

	switch (workAttack_.comboIndex_) {
	case 0:
		
		break;
	case 1:
		if (workAttack_.InComboPhase_ == 1) {
			emitter_.translate_ += workAttack_.velocity_;

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_ * 0.6f, particleVelocity_ * 0.6f);
				//std::uniform_real_distribution<float> distVelocityY(-0.1f, 0.1f);
				std::uniform_real_distribution<float> distVelocityZ(-particleVelocity_ * 0.6f, 0.0f);
				std::uniform_real_distribution<float> distColor(0.1f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				float size = 0.6f;
				particle.worldTransform_.scale_ = { size,size,size };
				particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocityZ(randomEngine) };
				particle.velocity_ = TransformNormal(particle.velocity_, DirectionToDirection({ 0.0f,0.0f,1.0f }, workAttack_.velocity_));
				//particle.color_ = { 0.38f,0.85f,0.97f,1.0 };
				particle.color_ = { 0.0f,0.0f,1.0f,1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);
			}

			for (std::list<Particle::ParticleData>::iterator itParticle = particles2_.begin(); itParticle != particles2_.end(); itParticle++) {
				(*itParticle).worldTransform_.translation_ += workAttack_.velocity_;
			}

		}
		break;
	case 2:

		if (workAttack_.InComboPhase_ == 1) {
			emitter_.translate_ += workAttack_.velocity_ * 1.2f;

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_ * 1.3f, particleVelocity_ * 1.3f);
				//std::uniform_real_distribution<float> distVelocityY(0.0f, particleVelocity_);
				std::uniform_real_distribution<float> distVelocityZ(-particleVelocity_ * 1.3f, 0.0f);
				std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				float size = 0.4f;
				particle.worldTransform_.scale_ = { size,size,size };
				particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocityZ(randomEngine) };
				particle.velocity_ = TransformNormal(particle.velocity_, DirectionToDirection({ 0.0f,0.0f,1.0f }, workAttack_.velocity_));
				//particle.color_ = { 0.38f,0.85f,0.97f,1.0 };
				particle.color_ = { 0.0f,0.0f,1.0f,1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);
			}

			for (std::list<Particle::ParticleData>::iterator itParticle = particles2_.begin(); itParticle != particles2_.end(); itParticle++) {
				(*itParticle).worldTransform_.translation_ += workAttack_.velocity_;
			}
		}

		break;
	}

	for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
		(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_;
		(*itParticle).currentTime_++;
	}
	for (std::list<Particle::ParticleData>::iterator itParticle = particles2_.begin(); itParticle != particles2_.end(); itParticle++) {
		(*itParticle).currentTime_++;
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
