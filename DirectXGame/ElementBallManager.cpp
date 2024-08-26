#include "ElementBallManager.h"

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
		burnScars.reset(BurnScars::Create(tex));
	}

	isAttack_ = false;
	preIsAttack_ = false;
	isShot_ = false;
	preIsShot_ = false;

}

void ElementBallManager::Update() {
	preIsAttack_ = isAttack_;
	preIsShot_ = isShot_;

	for (auto& elementBall : elementBalls_) {
		elementBall->Update();
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

}

void ElementBallManager::DrawBurnScars(const Camera& camera) {

	for (auto& burnScars : burnScareses_) {
		burnScars->Draw(camera);
	}

}

void ElementBallManager::OnCollision(uint32_t index) {

	elementBalls_[index]->OnCollision();
	burnScareses_[index]->EffectStart(elementBalls_[index]->GetWorldPos());
	burnScareses_[index]->HieghtAdjustment(0.0001f + (0.0001f * (float)index));

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

	for (auto& elementBall : elementBalls_) {
		elementBall->AttackStart();
	}
	isAttack_ = true;

}