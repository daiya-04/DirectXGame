#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"
#include "TextureManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "GameScene.h"
#include "PlayerIdel.h"
#include "PlayerAttack.h"
#include "PlayerDash.h"
#include "PlayerDead.h"
#include "PlayerJog.h"
#include "PlayerKnockBack.h"

void Player::Init(const std::vector<std::shared_ptr<Model>>& models){

	//アクション設定
	actionIndex_ = Action::Standing;
	
	//モデル関連の初期化
	BaseCharactor::Init(models);

	//状態の設定
	ChangeBehavior("Idel");
	//HPの設定
	hp_ = maxHp_;
	hpBarSize_ = { 400.0f,10.0f };

	//UIの設定
	hpBar_.reset(Sprite::Create(TextureManager::Load("Player_HP.png"), {440.0f,700.0f}));
	hpBar_->SetAnchorpoint({ 0.0f,0.5f });
	hpBar_->SetSize(hpBarSize_);

}

void Player::Update(){

	if (behaviorRequest_) {

		behavior_ = std::move(behaviorRequest_);
		behavior_->Init();

	}

	behavior_->Update();

	///プレイヤーの移動制限
	obj_->worldTransform_.translation_.x = std::clamp(obj_->worldTransform_.translation_.x, -30.0f, 30.0f);
	obj_->worldTransform_.translation_.z = std::clamp(obj_->worldTransform_.translation_.z, -20.0f, 40.0f);
	///

	BaseCharactor::Update();
}

void Player::UpdateUI() {
	//現在のHPのパーセント計算
	float percent = static_cast<float>(hp_) / static_cast<float>(maxHp_);
	//HPのUIを割合に合わせる
	hpBar_->SetSize({ hpBarSize_.x * percent,hpBarSize_.y });
}

void Player::Draw(const Camera& camera){

	BaseCharactor::Draw(camera);

}

void Player::DrawUI() {
	hpBar_->Draw();
}

void Player::OnCollision(const Vector3& hitPos) {

	if (actionIndex_ == Action::Accel) {
		return;
	}

	hp_--;

	Vector3 attackPos = hitPos;
	attackPos.y = GetCenterPos().y;

	knockBackBaseDict_ = (attackPos - GetCenterPos());
	if (knockBackBaseDict_.Length() <= 0.0f) {
		knockBackBaseDict_ = direction_;
	}

	//HPが0になったら...
	if (hp_ <= 0) {
		isDead_ = true;
		ChangeBehavior("Dead");
	}
	else {
		ChangeBehavior("KnockBack");
	}
}

void Player::ChangeBehavior(const std::string& behaviorName) {

	static const std::map<std::string, std::function<std::unique_ptr<IPlayerBehavior>()>> behaviorTable{
		{"Idel",[this]() {return std::make_unique<PlayerIdel>(this); }},
		{"Attack",[this]() {return std::make_unique<PlayerAttack>(this); }},
		{"Jog",[this]() {return std::make_unique<PlayerJog>(this); }},
		{"Dash",[this]() {return std::make_unique<PlayerDash>(this); }},
		{"Dead",[this]() {return std::make_unique<PlayerDead>(this); }},
		{"KnockBack",[this]() {return std::make_unique<PlayerKnockBack>(this); }},
	};

	auto nextBehavior = behaviorTable.find(behaviorName);
	if (nextBehavior != behaviorTable.end()) {
		behaviorRequest_ = nextBehavior->second();
	}

}

void Player::ShotMagicBall() {
	//攻撃を発射する方向の計算
	Vector3 direction = { 0.0f,0.0f,1.0f };
	direction = TransformNormal(direction, rotateMat_);

	///攻撃の玉の生成

	PlayerMagicBall* playerAttack = new PlayerMagicBall();
	std::shared_ptr<Model> attackModel = ModelManager::LoadOBJ("PlayerBullet");
	Vector3 offset = { 0.0f,0.0f,1.0f };
	offset = TransformNormal(offset, rotateMat_);
	Vector3 shotPos = Transform(skeletons_[actionIndex_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_) + offset;
	playerAttack->Init(attackModel, shotPos, direction);
	gameScene_->AddPlayerAttack(playerAttack);

	///
}
