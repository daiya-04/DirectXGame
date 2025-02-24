#include "Boss.h"
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "TextureManager.h"
#include "Easing.h"
#include "ModelManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "ParticleManager.h"
#include "BossIdel.h"
#include "BossAttack.h"
#include "BossAppear.h"
#include "BossDead.h"
#include "ColliderManager.h"

#include "GroundFlare.h"

void Boss::Init(const std::vector<std::shared_ptr<DaiEngine::Model>>& models) {

	//アクション設定
	actionIndex_ = Action::Standing;
	
	//モデル関連の初期化
	BaseCharactor::Init(models);

	collider_->Init("Boss", obj_->worldTransform_, {});
	collider_->SetCallbackFunc([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	collider_->ColliderOn();

	///エフェクト初期化
	effect_ = ParticleManager::Load("BossEnter");
	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = false;
	}
	///

	//状態設定
	ChangeBehavior("Appear");
	//攻撃設定
	attackType_ = AttackType::kElementBall;

	//HPの設定
	hp_ = maxHp_;
	hpBarSize_ = { 500.0f,10.0f };

	//UIの設定
	hpBar_.reset(DaiEngine::Sprite::Create(DaiEngine::TextureManager::Load("Boss_HP.png"), { 390.0f,40.0f }));
	hpBar_->SetAnchorpoint({ 0.0f,0.5f });
	hpBar_->SetSize(hpBarSize_);

}

void Boss::Update() {

	if (behaviorRequest_) {

		behavior_ = std::move(behaviorRequest_);
		behavior_->Init();

	}

	behavior_->Update();

	BaseCharactor::Update();

	//エフェクト発生位置更新
	for (auto& [group, particle] : effect_) {
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
		particle->particleData_.emitter_.translate.y = 0.01f;
		particle->Update();
	}

}

void Boss::UpdateUI() {
	//現在のHPのパーセント計算
	float percent = static_cast<float>(hp_) / static_cast<float>(maxHp_);
	//HPのUIを割合に合わせる
	hpBar_->SetSize({ hpBarSize_.x * percent,hpBarSize_.y });
}

void Boss::Draw(const DaiEngine::Camera& camera) {

	BaseCharactor::Draw(camera);

}

void Boss::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [group, particle] : effect_) {
		particle->Draw(camera);
	}
}

void Boss::DrawUI() {
	hpBar_->Draw();
}

void Boss::OnCollision(DaiEngine::Collider* other) {

	if (other->GetTag() == "PlayerAttack") {
		hp_--;
	}
	
	//HPが0になったら...
	if (hp_ <= 0) {
		isDead_ = true;
		ChangeBehavior("Dead");
	}
}

void Boss::ChangeBehavior(const std::string& behaviorName) {
	//行動とそれに対応するStateクラスの生成処理のマップ
	const std::unordered_map<std::string, std::function<std::unique_ptr<IBossBehavior>()>> behaviorTable{
		{"Idel", [this]() { return std::make_unique<BossIdel>(this); }},
		{"Attack", [this]() {return std::make_unique<BossAttack>(this); }},
		{"Appear", [this]() {return std::make_unique<BossAppear>(this); }},
		{"Dead", [this]() {return std::make_unique<BossDead>(this); }},
	};
	//検索
	auto nextBehavior = behaviorTable.find(behaviorName);
	if (nextBehavior != behaviorTable.end()) {
		//対応するStateクラスの生成
		behaviorRequest_ = nextBehavior->second();
	}

}	

void Boss::AppearEffectStart() {
	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = true;
	}
}

void Boss::AppearEffectEnd() {
	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = false;
	}
}

