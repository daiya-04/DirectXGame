#include "ElementBallManager.h"
#include "TextureManager.h"

ElementBallManager* ElementBallManager::GetInstance() {
	static ElementBallManager instance;

	return &instance;
}

void ElementBallManager::Init(const std::shared_ptr<Model>& model, uint32_t tex) {

	for (auto& elementBall : elementBalls_) {
		elementBall = std::make_unique<ElementBall>();
		elementBall->Init(model);
	}

	for (auto& burnScars : burnScareses_) {
		burnScars.reset(BurnScar::Create(tex));
	}

	for (auto& fireField : fireFields_) {
		fireField.reset(GPUParticle::Create(TextureManager::Load("FireParticle.png"), 15000));
		fireField->emitter_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		fireField->emitter_.emitterType = 4;
		fireField->emitter_.frequency = 1.0f / 60.0f;
		fireField->emitter_.count = 2000;
		fireField->emitter_.lifeTime = 10.0f / 60.0f;
		fireField->emitter_.speed = 0.1f;
		fireField->emitter_.scale = 0.2f;
		fireField->emitter_.size = Vector3(2.5f, 1.5f, 1.5f);
		fireField->isLoop_ = false;
	}

	for (auto& splash : splashes_) {
		splash.reset(GPUParticle::Create(TextureManager::Load("FireParticle.png"), 10000));
		splash->emitter_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		splash->emitter_.emitterType = 4;
		splash->emitter_.scale = 0.1f;
		splash->emitter_.size = Vector3(1.0f, 1.0f, 1.0f);
		splash->emitter_.frequency = 20.0f / 60.0f;
		splash->isLoop_ = false;
	}

	isAttack_ = false;
	preIsAttack_ = false;
	isShot_ = false;
	preIsShot_ = false;

}

void ElementBallManager::Update() {
	preIsAttack_ = isAttack_;
	preIsShot_ = isShot_;

	for (uint32_t index = 0; index < elementBalls_.size();index++) {
		if (elementBalls_[index]->DeadFlag()) {
			OnCollision(index);
		}

		elementBalls_[index]->Update();
	}

	for (auto& burnScars : burnScareses_) {
		burnScars->Update();
	}

	if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kShot) {
		isShot_ = true;
	}else {
		isShot_ = false;
	}

	

	if (!elementBalls_[0]->IsLife() && !elementBalls_[1]->IsLife() && !elementBalls_[2]->IsLife() && !elementBalls_[3]->IsLife()) {
		isAttack_ = false;
	}

	/*if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kSet) {
		for (size_t index = 0; index < 4; index++) {
			fireFields_[index]->emitter_.translate = elementBalls_[index]->GetWorldPos();
			fireFields_[index]->emitter_.translate.y = 0.1f;
		}
	}*/

	for (auto& splash : splashes_) {
		if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kSet) {
			splash->isLoop_ = true;
			splash->emitter_.lifeTime = 20.0f / 60.0f;
			splash->emitter_.speed = 5.0f;
			splash->emitter_.size = Vector3(1.5f, 1.0f, 1.0f);
			splash->emitter_.count = 1000;
			splash->emitter_.scale = 0.2f;
		}
		if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kCharge) {
			splash->isLoop_ = false;
			splash->emitter_.frequencyTime = 0.0f;
			splash->emitter_.emit = 0;
		}
		splash->Update();
	}
	

	for (auto& fireField : fireFields_) {
		if (elementBalls_[0]->GetPhase() == ElementBall::Phase::kCharge) {
			fireField->isLoop_ = false;
			fireField->emitter_.emit = 0;
			fireField->emitter_.frequencyTime = 0.0f;
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

	for (uint32_t index = 0; index < 4; index++) {
		elementBalls_[index]->SetAttackData(pos + offset[index], 2 + index);
	}

}

void ElementBallManager::AttackStart() {

	for (auto& fireField : fireFields_) {
		fireField->isLoop_ = true;
	}

	for (auto& elementBall : elementBalls_) {
		elementBall->AttackStart();
	}
	isAttack_ = true;

	for (size_t index = 0; index < 4; index++) {
		fireFields_[index]->emitter_.translate = elementBalls_[index]->GetWorldPos();
		fireFields_[index]->emitter_.translate.y = 0.1f;

		splashes_[index]->emitter_.translate = elementBalls_[index]->GetWorldPos();
		splashes_[index]->emitter_.translate.y = 0.1f;
	}

	/*for (auto& splash : splashes_) {
		splash->emitter_.count = 2000;
		splash->emitter_.size = Vector3(0.7f, 1.0f, 1.0f);
		splash->emitter_.speed = 50.0f;
		splash->emitter_.lifeTime = 60.0f / 60.0f;
		splash->emitter_.emit = 1;
		splash->emitter_.frequencyTime = 0.0f;
		splash->emitter_.scale = 0.5f;

		splash->isLoop_ = false;

		splash->Update();
	}*/

}