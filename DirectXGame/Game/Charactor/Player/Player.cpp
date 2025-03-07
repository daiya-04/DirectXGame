#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"
#include "TextureManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "PlayerIdel.h"
#include "PlayerAttack.h"
#include "PlayerDash.h"
#include "PlayerDead.h"
#include "PlayerJog.h"
#include "PlayerKnockBack.h"
#include "ColliderManager.h"

void Player::Init(const std::vector<std::shared_ptr<DaiEngine::Model>>& models){

	//アクション設定
	actionIndex_ = Action::Standing;

	BaseCharactor::Init(models);

	//モデル関連の初期化
	//モデルやアニメーションの設定
	obj_.reset(DaiEngine::SkinningObject::Create(animationModels_[actionIndex_]));
	skinClusters_.resize(animationModels_.size());
	for (size_t index = 0; index < animationModels_.size(); index++) {
		skeletons_.emplace_back(DaiEngine::Skeleton::Create(animationModels_[index]->rootNode_));
		skinClusters_[index].Create(skeletons_[index], animationModels_[index]);
	}
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);
	
	collider_->Init("Player", obj_->worldTransform_, {});
	collider_->SetCallbackFunc([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	collider_->ColliderOn();

	//状態の設定
	ChangeBehavior("Idel");
	//HPの設定
	hp_ = maxHp_;
	hpBarSize_ = { 400.0f,10.0f };

	//UIの設定
	hpBar_.reset(DaiEngine::Sprite::Create(DaiEngine::TextureManager::Load("Player_HP.png"), {440.0f,700.0f}));
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
	obj_->worldTransform_.translation_.z = std::clamp(obj_->worldTransform_.translation_.z, -30.0f, 30.0f);
	///

	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);

	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	UpdateCenterPos();
	//アニメーション再生
	animations_[actionIndex_].Play(skeletons_[actionIndex_]);

	skeletons_[actionIndex_].Update();
	skinClusters_[actionIndex_].Update(skeletons_[actionIndex_]);

	BaseCharactor::Update();
}

void Player::UpdateUI() {
	//現在のHPのパーセント計算
	float percent = static_cast<float>(hp_) / static_cast<float>(maxHp_);
	//HPのUIを割合に合わせる
	hpBar_->SetSize({ hpBarSize_.x * percent,hpBarSize_.y });
}

void Player::UpdateCenterPos() {
	centerPos_ = {
		obj_->worldTransform_.matWorld_.m[3][0],
		obj_->worldTransform_.matWorld_.m[3][1] + collider_->GetSize().y,
		obj_->worldTransform_.matWorld_.m[3][2],
	};
}

void Player::Draw(const DaiEngine::Camera& camera){

	BaseCharactor::Draw(camera);

	obj_->Draw(camera);
	skeletons_[actionIndex_].Draw(obj_->worldTransform_, camera);

}

void Player::OnCollision(DaiEngine::Collider* other) {

	if (actionIndex_ == Action::Accel) {
		return;
	}

	if (other->GetTag() == "BossAttack") {
		hp_--;

		Vector3 attackPos = other->GetWorldPos();
		attackPos.y = GetCenterPos().y;

		knockBackBaseDict_ = (attackPos - GetCenterPos());
		if (knockBackBaseDict_.Length() <= 0.0f) {
			knockBackBaseDict_ = direction_;
		}

		ChangeBehavior("KnockBack");
	}

	//ターゲットとの距離
	float distance = (target_->GetWorldPos().GetXZ() - GetCenterPos().GetXZ()).Length();
	//射程内に入ったらズーム
	if (distance <= attackRange_) {
		followCamera_->SetZoomFlag(true);
	}else {
		followCamera_->SetZoomFlag(false);
	}

	//HPが0になったら...
	if (hp_ <= 0) {
		isDead_ = true;
		ChangeBehavior("Dead");
	}
}

void Player::ChangeBehavior(const std::string& behaviorName) {
	//行動とそれに対応するStateクラスの生成処理のマップ
	const std::map<std::string, std::function<std::unique_ptr<IPlayerBehavior>()>> behaviorTable{
		{"Idel",[this]() {return std::make_unique<PlayerIdel>(this); }},
		{"Attack",[this]() {return std::make_unique<PlayerAttack>(this); }},
		{"Jog",[this]() {return std::make_unique<PlayerJog>(this); }},
		{"Dash",[this]() {return std::make_unique<PlayerDash>(this); }},
		{"Dead",[this]() {return std::make_unique<PlayerDead>(this); }},
		{"KnockBack",[this]() {return std::make_unique<PlayerKnockBack>(this); }},
	};
	//検索
	auto nextBehavior = behaviorTable.find(behaviorName);
	if (nextBehavior != behaviorTable.end()) {
		//対応するStateクラスの生成
		behaviorRequest_ = nextBehavior->second();
	}

}

void Player::SetData(const LevelData::ObjectData& data) {
	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	BaseCharactor::SetData(data);

	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	UpdateCenterPos();
}

void Player::ShotMagicBall() {

	//攻撃を発射する方向の計算
	Vector3 direction = { 0.0f,0.0f,1.0f };
	direction = TransformNormal(direction, rotateMat_);

	///攻撃の玉の生成
	Vector3 offset = { 0.0f,0.0f,1.0f };
	offset = TransformNormal(offset, rotateMat_);
	//手から発射
	Vector3 shotPos = Transform(skeletons_[actionIndex_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_) + offset;
	//攻撃配列から魔法弾を取り出す
	MagicBallManager* magicBall = GetAttackType<MagicBallManager>();
	magicBall->AttackStart(shotPos, direction);

	///
}

void Player::AttackGroundBurst() {
	
	//プレイヤーとターゲットの距離の計算
	float distance = (target_->GetWorldPos().GetXZ() - obj_->GetWorldPos().GetXZ()).Length();
	//攻撃配列から地面噴射を取り出す
	GroundBurstManager* groundBurst = GetAttackType<GroundBurstManager>();
	
	//射程内か？
	if (distance <= attackRange_) {
		//射程内ならターゲットの場所に
		groundBurst->AttackStart(target_->GetWorldPos());
	}
	else {
		//射程外なら一定の距離に
		//攻撃を発射する方向の計算
		Vector3 offset = { 0.0f,0.0f, 7.0f };
		offset = TransformNormal(offset, rotateMat_);

		groundBurst->AttackStart(GetCenterPos() + offset);
	}

}

void Player::SetAnimation(size_t actionIndex, bool isLoop) {
	BaseCharactor::SetAnimation(actionIndex, isLoop);
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);
}

void Player::DissolveUpdate() {
	obj_->threshold_ = animations_[actionIndex_].GetAnimationTime() / animations_[actionIndex_].GetDuration();
	obj_->threshold_ = std::clamp(obj_->threshold_, 0.0f, animations_[actionIndex_].GetDuration());
}
