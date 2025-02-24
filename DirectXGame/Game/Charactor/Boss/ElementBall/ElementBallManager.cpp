#include "ElementBallManager.h"
#include "TextureManager.h"


void ElementBallManager::Init(const std::shared_ptr<DaiEngine::Model>& model) {

	for (auto& burnScars : burnScareses_) {
		burnScars.reset(BurnScar::Create(DaiEngine::TextureManager::Load("BurnScars.png")));
	}

	for (size_t index = 0; index < elementBalls_.size(); index++) {
		elementBalls_[index] = std::make_unique<ElementBall>();
		elementBalls_[index]->Init(model);
		elementBalls_[index]->SetBurnScar(burnScareses_[index].get());
	}

	isAttack_ = false;
	preIsAttack_ = false;
	isShot_ = false;
	preIsShot_ = false;

}

void ElementBallManager::Update() {
	preIsAttack_ = isAttack_;
	preIsShot_ = isShot_;
	//攻撃が当たるか地面に着いたら
	for (auto& elementBall : elementBalls_) {
		elementBall->Update();
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

}

void ElementBallManager::DrawParticle(const DaiEngine::Camera& camera) {

	for (auto& elementBall : elementBalls_) {
		elementBall->DrawParticle(camera);
	}
	for (auto& burnScars : burnScareses_) {
		burnScars->DrawParticle(camera);
	}
}

void ElementBallManager::DrawBurnScars(const DaiEngine::Camera& camera) {

	//高さが低い順にソート
	std::sort(burnScareses_.begin(), burnScareses_.end(), [](const std::unique_ptr<BurnScar>& scarA, const std::unique_ptr<BurnScar>& scarB) {
		return scarA->GetPosition().y < scarB->GetPosition().y;
		}
	);

	for (auto& burnScars : burnScareses_) {
		burnScars->Draw(camera);
	}

}

void ElementBallManager::SetTartget(const Vector3* target) {

	for (auto& elementBall : elementBalls_) {
		elementBall->SetTarget(target);
	}

}

void ElementBallManager::SetAttackData(const Vector3& pos) {

	//攻撃位置のオフセット
	const Vector3 kOffset[kElementBallNum_] = {
		{4.0f,0.0f,2.0f},
		{-4.0f,0.0f,2.0f},
		{4.0f,0.0f,-2.0f},
		{-4.0f,0.0f,-2.0f},
	};
	//インターバルの基準
	const uint32_t kBaseInterval = 2;

	//生成位置の設定
	for (uint32_t index = 0; index < kElementBallNum_; index++) {
		elementBalls_[index]->SetAttackData(pos + kOffset[index], kBaseInterval + index);
	}

}

void ElementBallManager::AttackStart() {

	for (auto& elementBall : elementBalls_) {
		elementBall->AttackStart();
	}
	isAttack_ = true;

}