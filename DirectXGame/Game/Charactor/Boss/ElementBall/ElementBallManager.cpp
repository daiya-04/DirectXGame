#include "ElementBallManager.h"
#include "TextureManager.h"
#include "ParticleManager.h"


void ElementBallManager::Init(const std::shared_ptr<Model>& model) {


	for (size_t index = 0; index < elementBalls_.size(); index++) {
		elementBalls_[index] = std::make_unique<ElementBall>();
		elementBalls_[index]->Init(model);
		elementBalls_[index]->GetCollider()->SetCallbackFunc([this, number = index](Collider* other) {this->OnCollision(number, other); });
	}

	for (auto& burnScars : burnScareses_) {
		burnScars.reset(BurnScar::Create(TextureManager::Load("BurnScars.png")));
	}


	///エフェクト設定
	for (auto& effect : fireSetEffs_) {
		effect = ParticleManager::Load("FireBallSet");
		for (auto& [group, particle] : effect) {
			particle->particleData_.isLoop_ = false;
		}
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
		/*if (elementBalls_[index]->DeadFlag()) {
			OnCollision(index);
		}*/

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

	for (auto& effect : fireSetEffs_) {
		for (auto& [group, particle] : effect) {
			//攻撃準備中のみエフェクトを出す
			if (elementBalls_[0]->GetPhase() != ElementBall::Phase::kSet) {
				particle->particleData_.isLoop_ = false;
			}
			particle->Update();
		}
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
	for (auto& effect : fireSetEffs_) {
		for (auto& [group, particle] : effect) {
			particle->Draw(camera);
		}
	}

}

void ElementBallManager::DrawBurnScars(const Camera& camera) {

	for (auto& burnScars : burnScareses_) {
		burnScars->Draw(camera);
	}

}

void ElementBallManager::OnCollision(size_t index, Collider* other) {

	if (other->GetTag() == "Player" || other->GetTag() == "Ground") {
		elementBalls_[index]->OnCollision(other);
		burnScareses_[index]->EffectStart(elementBalls_[index]->GetWorldPos());
		burnScareses_[index]->HeightAdjustment(0.0001f + (0.0001f * (float)index));
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

	for (size_t index = 0; index < kElementBallNum_; index++) {
		for (auto& [group, particle] : fireSetEffs_[index]) {
			particle->particleData_.isLoop_ = true;
			particle->particleData_.emitter_.translate = elementBalls_[index]->GetWorldPos();
			particle->particleData_.emitter_.translate.y = 0.01f;
		}
	}

}