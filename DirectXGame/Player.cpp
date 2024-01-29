#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"
#include "TextureManager.h"

#include "Enemy.h"

const std::array<Player::ComboAttack, Player::comboNum_> Player::kComboAttacks_ = {
	{
		{0,20,5},
		{0,20,5},
		{0,30,10}
	}
};

void Player::Init(std::vector<uint32_t> modelHandles){

	uint32_t particleTex = TextureManager::Load("circle.png");
	uint32_t particleTex2 = TextureManager::Load("star.png");

	worldTransform_.Init();
	partsWorldTransform_[Body].Init();
	partsWorldTransform_[Head].Init();

	obj_.resize(modelHandles.size());
	for (size_t index = 0; index < obj_.size(); ++index) {
		obj_[index] = std::make_unique<Object3d>();
		obj_[index].reset(Object3d::Create(modelHandles[index]));
	}

	magicParticle_ = std::make_unique<Particle>();
	magicParticle_.reset(Particle::Create(particleTex, 1000));
	magicParticle2_ = std::make_unique<Particle>();
	magicParticle2_.reset(Particle::Create(particleTex2, 1));

	behaviorRequest_ = Behavior::kRoot;

	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };
	partsWorldTransform_[Head].translation_ = { 0.0f,6.5f,0.0f };	

	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];


	//行列更新
	Matrix4x4 S = MakeScaleMatrix(worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.matWorld_ = S * rotateMat_ * T;
	//worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Player::Update(const std::list<std::unique_ptr<Enemy>>& enemies){

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		    case Behavior::kRoot:
				RootInit();
			    break;
			case Behavior::kAttack:
				AttackInit(enemies);
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
		AttackUpdate(enemies);
		break;
	case Behavior::kDash:
		DashUpdate();
		break;
	}

	

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
	magicParticle2_->Draw(particles2_, camera,false);

}

void Player::OnCollision() {
	life--;
}

void Player::RootInit() {

	isAttack_ = false;

}

void Player::RootUpdate() {

	Vector3 move{};
	Vector3 zeroVector{};
	const float speed = 0.5f;

	move = Input::GetInstance()->GetMoveXZ();
	move = move / SHRT_MAX * speed;

	//ダッシュ
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_X)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	//ダッシュ
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kDash;
	}

	move = TransformNormal(move, MakeRotateYMatrix(followCamera_->GetCamera().rotation_.y));

	worldTransform_.translation_ += move;
	worldTransform_.translation_.y = 0.0f;

	if (move != zeroVector) {
		rotate_ = move;
	}

	//worldTransform_.rotation_.y = std::atan2(rotate_.x, rotate_.z);

	rotateMat_ = DirectionToDirection(from_, rotate_);

}

void Player::AttackInit(const std::list<std::unique_ptr<Enemy>>& enemies) {

	workAttack_.attackParam_ = 0;
	Search(enemies);

	if (target_) {

		Vector3 direction = target_->GetWorldTransform().translation_ - worldTransform_.translation_;

		rotateMat_ = DirectionToDirection(from_, direction);
	}
	
}

void Player::AttackUpdate(const std::list<std::unique_ptr<Enemy>>& enemies) {

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
		if (workAttack_.comboNext_) {
			workAttack_.comboNext_ = false;
			workAttack_.comboIndex_++;

			AttackInit(enemies);
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

		if (target_) {

			emitter_.translate_ = target_->GetWorldPos();

		}else {
			Vector3 offset = { 0.0f,5.0f,10.0f };
			offset = TransformNormal(offset, rotateMat_);

			emitter_.translate_ = worldTransform_.translation_ + offset;
		}

		
		

		//速度とパーティクルの数の設定と生成
		switch (workAttack_.comboIndex_) {
		case 0:

			emitter_.count_ = 100;
			

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_, particleVelocity_);
				std::uniform_real_distribution<float> distVelocityY(-0.1f, 0.1f);
				std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				float size = 0.5f;
				particle.worldTransform_.scale_ = { size,size,size };
				particle.velocity_ = { distVelocity(randomEngine), distVelocityY(randomEngine) ,distVelocity(randomEngine) };
				particle.color_ = { 0.0f,0.0f,distColor(randomEngine),1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);

				if (count == 0) {
					particle.worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
					particle.velocity_ = {};
					particle.color_ = { 0.71f,0.94f,1.0f,1.0f };

					particles2_.push_back(particle);
				}
				
			}
			

			break;
		case 1:

			emitter_.count_ = 100;

			for (size_t count = 0; count < emitter_.count_; count++) {

				std::uniform_real_distribution<float> distVelocity(-particleVelocity_, particleVelocity_);
				std::uniform_real_distribution<float> distVelocityY(-0.1f, 0.1f);
				std::uniform_real_distribution<float> distColor(0.3f, 0.8f);

				Particle::ParticleData particle;
				particle.worldTransform_.translation_ = emitter_.translate_;
				float size = 0.5f;
				particle.worldTransform_.scale_ = { size,size,size };
				particle.velocity_ = { distVelocity(randomEngine), distVelocityY(randomEngine) ,distVelocity(randomEngine) };
				particle.color_ = { 0.0f,0.0f,distColor(randomEngine),1.0 };
				particle.lifeTime_ = (float)kComboAttacks_[workAttack_.comboIndex_].attackTime_;
				particle.currentTime_ = 0.0f;

				particles_.push_back(particle);

				if (count == 0) {
					
					particle.worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
					particle.velocity_ = {};
					particle.color_ = { 0.71f,0.94f,1.0f,1.0f };

					particles2_.push_back(particle);
				}
			}



			break;
		case 2:

			emitter_.count_ = 500;

			for (size_t count = 0; count < emitter_.count_; count++) {

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

				if (count == 0) {
					particle.velocity_ = {};
					particle.color_ = { 0.71f,0.94f,1.0f,1.0f };

					particles2_.push_back(particle);
				}
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

	worldTransform_.translation_ += workDash_.dashDirection_.Normalize() * dashSpeed;

	if (++workDash_.dashParam_ >= workDash_.dashTime_) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Player::Search(const std::list<std::unique_ptr<Enemy>>& enemies) {

	if (enemies.empty()) {
		target_ = nullptr;
		return;
	}

	//目標
	///<playerとの距離、敵のポインタ>
	std::list<std::pair<float, Enemy*>> targets;

	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		
		Vector3 distance = enemy->GetWorldTransform().translation_ - worldTransform_.translation_;

		if (attackRange_ >= distance.Length()) {

			targets.emplace_back(std::make_pair(distance.Length(), enemy.get()));

		}

		target_ = nullptr;
		if (!targets.empty()) {
			targets.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
			target_ = targets.front().second;
		}
	}
}

Vector3 Player::GetWorldPos() const{

	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
