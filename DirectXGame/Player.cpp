#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"
#include "TextureManager.h"

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

const std::array<Player::ComboAttack, Player::comboNum_> Player::kComboAttacks_ = {
	{
		{0,20,10},
		{0,20,10},
		{0,30,10}
	}
};

void Player::Init(std::vector<uint32_t> modelHandles){

	uint32_t particleTex = TextureManager::Load("circle.png");

	worldTransform_.Init();
	partsWorldTransform_[Body].Init();
	partsWorldTransform_[Head].Init();

	obj_.resize(modelHandles.size());
	for (size_t index = 0; index < obj_.size(); ++index) {
		obj_[index] = std::make_unique<Object3d>();
		obj_[index].reset(Object3d::Create(modelHandles[index]));
	}

	magicParticle_ = std::make_unique<Particle>();
	magicParticle_.reset(Particle::Create(particleTex, 200));

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

void Player::DrawParticle(const Camera& camera) {

	magicParticle_->Draw(particles_, camera);

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
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_X)) {
		behaviorRequest_ = Behavior::kAttack;
	}

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

	workAttack_.attackParam_ = 0;

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

	if (workAttack_.attackParam_ >= totalAttackTime) {
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

		Vector3 offset = { 0.0f,5.0f,10.0f };
		offset = TransformNormal(offset, rotateMat_);

		emitter_.translate_ = worldTransform_.translation_ + offset;
		

		//速度とパーティクルの数の設定と生成
		switch (workAttack_.comboIndex_) {
		case 0:

			emitter_.count_ = 100;

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_, particleVelocity_);
				std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocity(randomEngine) };
				particle.color_ = { 0.0f,0.0f,distColor(randomEngine),1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);
			}

			break;
		case 1:

			emitter_.count_ = 100;

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_, particleVelocity_);
				std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocity(randomEngine) };
				particle.color_ = { distColor(randomEngine),distColor(randomEngine),0.0f,1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);
			}

			break;
		case 2:

			emitter_.count_ = 200;

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_ * 1.5f, particleVelocity_ * 1.5f);
				std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocity(randomEngine) };
				particle.color_ = { distColor(randomEngine),0.0f,0.0f,1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);
			}

			break;
		}


	}

	switch (workAttack_.comboIndex_) {
	case 0:
		
		break;
	case 1:
		
		break;
	case 2:
		
		break;
	}

	for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
		(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_;
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
