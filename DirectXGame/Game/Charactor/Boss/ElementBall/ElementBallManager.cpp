#include "ElementBallManager.h"
#include "TextureManager.h"
#include "ParticleManager.h"


void ElementBallManager::Init(const std::shared_ptr<Model>& model, uint32_t tex) {

	for (auto& elementBall : elementBalls_) {
		elementBall = std::make_unique<ElementBall>();
		elementBall->Init(model);
	}

	for (auto& burnScars : burnScareses_) {
		burnScars.reset(BurnScar::Create(tex));
	}


	///エフェクト設定
	for (auto& fireField : fireFields_) {
		fireField.reset(GPUParticle::Create(TextureManager::Load("Steam.png"), 15000));
		fireField->particleData_.emitter_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		fireField->particleData_.emitter_.emitterType = GPUParticle::EmitShape::Circle;
		fireField->particleData_.emitter_.frequency = 1.0f / 60.0f;
		fireField->particleData_.emitter_.count = 2000;
		fireField->particleData_.emitter_.lifeTime = 10.0f / 60.0f;
		fireField->particleData_.emitter_.speed = 0.1f;
		fireField->particleData_.emitter_.scale = 0.2f;
		fireField->particleData_.emitter_.radius = 2.5f;
		fireField->particleData_.isLoop_ = false;

		fireField->particleData_.overLifeTime_.isAlpha = 1;
		fireField->particleData_.overLifeTime_.midAlpha = 1.0f;
	}

	for (auto& splash : splashes_) {
		splash.reset(GPUParticle::Create(TextureManager::Load("Steam.png"), 10000));
		splash->SetParticleData(ParticleManager::Load("FireBallSplash"));
	}

	for (auto& fireSpark : fireSparks_) {
		fireSpark.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 2048));
		fireSpark->SetParticleData(ParticleManager::Load("FireBallFireSpark"));
	}
	///

	isAttack_ = false;
	preIsAttack_ = false;
	isShot_ = false;
	preIsShot_ = false;

}

void ElementBallManager::Update() {
	preIsAttack_ = isAttack_;
	preIsShot_ = isShot_;
	//攻撃が当たるか地面に着いたら
	for (uint32_t index = 0; index < elementBalls_.size();index++) {
		if (elementBalls_[index]->DeadFlag()) {
			OnCollision(index);
		}

		elementBalls_[index]->Update();
	}

	for (auto& burnScars : burnScareses_) {
		burnScars->Update();
	}
	//発射中か否か
	if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kShot) {
		isShot_ = true;
	}else {
		isShot_ = false;
	}

	
	//弾全部消えたら攻撃終了
	if (!elementBalls_[0]->IsLife() && !elementBalls_[1]->IsLife() && !elementBalls_[2]->IsLife() && !elementBalls_[3]->IsLife()) {
		isAttack_ = false;
	}

	for (auto& splash : splashes_) {
		if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kSet) {
			splash->particleData_.isLoop_ = true;
		}
		else {
			splash->particleData_.isLoop_ = false;
		}
		splash->Update();
	}

	for (auto& fireSpark : fireSparks_) {
		if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kSet) {
			fireSpark->particleData_.isLoop_ = true;
		}else {
			fireSpark->particleData_.isLoop_ = false;
		}
		fireSpark->Update();
	}
	

	for (auto& fireField : fireFields_) {
		if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kCharge) {
			fireField->particleData_.isLoop_ = false;
		}
		fireField->Update();
	}

}

void ElementBallManager::Draw(const Camera& camera) {

	for (auto& elementBall : elementBalls_) {
		elementBall->Draw(camera);
	}

}

void ElementBallManager::DrawParticle(const Camera& camera) {

	for (auto& elementBall : elementBalls_) {
		elementBall->DrawParticle(camera);
	}
	for (auto& burnScars : burnScareses_) {
		burnScars->DrawParticle(camera);
	}
	for (auto& fireField : fireFields_) {
		fireField->Draw(camera);
	}
	for (auto& splash : splashes_) {
		splash->Draw(camera);
	}
	for (auto& fireSpark : fireSparks_) {
		fireSpark->Draw(camera);
	}

}

void ElementBallManager::DrawBurnScars(const Camera& camera) {

	for (auto& burnScars : burnScareses_) {
		burnScars->Draw(camera);
	}

}

void ElementBallManager::OnCollision(uint32_t index) {

	elementBalls_[index]->OnCollision();
	burnScareses_[index]->EffectStart(elementBalls_[index]->GetWorldPos());
	burnScareses_[index]->HeightAdjustment(0.0001f + (0.0001f * (float)index));

}

void ElementBallManager::SetTartget(const WorldTransform* target) {

	for (auto& elementBall : elementBalls_) {
		elementBall->SetTarget(target);
	}

}

void ElementBallManager::SetAttackData(const Vector3& pos) {

	Vector3 offset[4] = {
		{4.0f,0.0f,2.0f},
		{-4.0f,0.0f,2.0f},
		{4.0f,0.0f,-2.0f},
		{-4.0f,0.0f,-2.0f},
	};
	//生成位置の設定
	for (uint32_t index = 0; index < 4; index++) {
		elementBalls_[index]->SetAttackData(pos + offset[index], 2 + index);
	}

}

void ElementBallManager::AttackStart() {

	for (auto& fireField : fireFields_) {
		fireField->particleData_.isLoop_ = true;
	}

	for (auto& elementBall : elementBalls_) {
		elementBall->AttackStart();
	}
	isAttack_ = true;

	for (size_t index = 0; index < 4; index++) {
		fireFields_[index]->particleData_.emitter_.translate = elementBalls_[index]->GetWorldPos();
		fireFields_[index]->particleData_.emitter_.translate.y = 0.1f;

		splashes_[index]->particleData_.emitter_.translate = elementBalls_[index]->GetWorldPos();
		splashes_[index]->particleData_.emitter_.translate.y = 0.1f;

		fireSparks_[index]->particleData_.emitter_.translate = elementBalls_[index]->GetWorldPos();
		fireSparks_[index]->particleData_.emitter_.translate.y = 0.1f;
	}

}