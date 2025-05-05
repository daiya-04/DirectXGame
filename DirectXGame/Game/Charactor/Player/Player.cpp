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
#include "PlayerAvoid.h"
#include "PlayerChargeAttack.h"

#include "ColliderManager.h"
#include "ParticleManager.h"
#include "BurnState.h"
#include "ChilledState.h"
#include "NormalState.h"

#include "MagicBallManager.h"
#include "GroundBurstManager.h"
#include "MagicCannonManager.h"

void Player::Init(const std::vector<std::shared_ptr<DaiEngine::Model>>& models){

	//アクション設定
	actionIndex_ = Action::kStanding;

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
	collider_->SetStayCallback([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	collider_->SetEnterCallback([this](DaiEngine::Collider* other) {this->EnterCollision(other); });
	collider_->SetExitCallback([this](DaiEngine::Collider* other) {this->ExitCollision(other); });
	collider_->ColliderOn();

	handEff_ = ParticleManager::Load("PlayerHandEff");
	for (auto& [group, particle] : handEff_) {
		particle->particleData_.isLoop_ = false;
	}
	chilledEff_ = ParticleManager::Load("ChilledEffect");
	for (auto& [group, particle] : chilledEff_) {
		particle->particleData_.isLoop_ = false;
	}
	burnEff_ = ParticleManager::Load("BurnEffect");
	for (auto& [group, particle] : burnEff_) {
		particle->particleData_.isLoop_ = false;
	}
	chargeEff_ = ParticleManager::Load("MagicCannonCharge");
	for (auto& [group, particle] : chargeEff_) {
		particle->particleData_.isLoop_ = false;
	}

	//状態の設定
	ChangeBehavior("Idel");

	ChangeState("Normal");
	//HPの設定
	hp_.Init(DaiEngine::TextureManager::Load("Player_HP.png"), { 440.0f,690.0f }, { 400.0f,10.0f });
	hp_.SetMaxHP(maxHp_);

	stamina_.Init({ 440.0f,700.0f }, { 400.0f,10.0f });
	maxStamina_ = 150.0f;
	dashStamina_ = 15.0f;
	avoidStamina_ = 30.0f;
	healStamina_ = 20.0f;
	stamina_.SetMaxStamina(maxStamina_);

}

void Player::Update(){

	if (behaviorRequest_) {
		behavior_ = std::move(behaviorRequest_);
		behavior_->Init();
	}

	behavior_->Update();
	state_->Update();

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

	for (auto& [group, particle] : handEff_) {
		particle->Update();
	}
	for (auto& [group, particle] : burnEff_) {
		particle->particleData_.emitter_.translate = centerPos_;
		particle->Update();
	}
	for (auto& [group, particle] : chilledEff_) {
		particle->particleData_.emitter_.translate = centerPos_;
		particle->Update();
	}
	for (auto& [group, particle] : chargeEff_) {
		particle->Update();
	}

	stamina_.Update();
	BaseCharactor::Update();
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

void Player::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [group, particle] : handEff_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : burnEff_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : chilledEff_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : chargeEff_) {
		particle->Draw(camera);
	}

}

void Player::DrawUI() {
	BaseCharactor::DrawUI();
	stamina_.Draw();
}

void Player::OnCollision(DaiEngine::Collider* other) {

	if (other->GetTag() == "BossAttack") {
		hp_.TakeDamage(other->GetIntValue("Damage"));

		Vector3 attackPos = other->GetWorldPos();
		attackPos.y = GetCenterPos().y;

		knockBackBaseDict_ = (attackPos - GetCenterPos());
		if (knockBackBaseDict_.Length() <= 0.0f) {
			knockBackBaseDict_ = direction_;
		}
		EndHandEff();
		if (hp_.GetHP() > 0) {
			ChangeBehavior("KnockBack");
		}
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
	if (hp_.GetHP() <= 0) {
		Dead();
	}
}

void Player::EnterCollision(DaiEngine::Collider* other) {

	if (other->GetTag() == "BurnScar") {
		ChangeState("Burn");
	}

	if (other->GetTag() == "IceScar") {
		ChangeState("Chilled");
	}

}

void Player::ExitCollision(DaiEngine::Collider* other) {

	if (other->GetTag() == "BurnScar") {
		if (auto state = dynamic_cast<BurnState*>(state_.get())) {
			state_->Exit();
		}
	}

	if (other->GetTag() == "IceScar") {
		if (auto state = dynamic_cast<ChilledState*>(state_.get())) {
			state_->Exit();
		}
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
		{"Avoid", [this]() {return std::make_unique<PlayerAvoid>(this); }},
		{"ChargeAttack", [this]() {return std::make_unique<PlayerChargeAttack>(this); }},
	};
	//検索
	auto nextBehavior = behaviorTable.find(behaviorName);
	if (nextBehavior != behaviorTable.end()) {
		//対応するStateクラスの生成
		behaviorRequest_ = nextBehavior->second();
	}

	chargeCount_ = 0.0f;
}

void Player::ChangeState(const std::string& stateName) {

	const std::map<std::string, std::function<std::unique_ptr<IPlayerState>()>> stateTable{
		{"Normal", [this]() {return std::make_unique<NormalState>(this); }},
		{"Burn", [this]() {return std::make_unique<BurnState>(this); }},
		{"Chilled", [this]() {return std::make_unique<ChilledState>(this); }},
	};
	//検索
	auto newState = stateTable.find(stateName);
	if (newState != stateTable.end()) {
		state_ = newState->second();
		state_->Init();
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

void Player::ShotMagicCannon() {

	//攻撃を発射する方向の計算
	Vector3 direction = { 0.0f,0.0f,1.0f };
	direction = TransformNormal(direction, rotateMat_);

	Vector3 offset = { 0.0f,0.0f,1.0f };
	offset = TransformNormal(offset, rotateMat_);
	//両手の間から発射
	Vector3 shotPos = GetPlayerMidHandPos(offset);
	//攻撃配列から魔法弾を取り出す
	MagicCannonManager* magicBall = GetAttackType<MagicCannonManager>();
	magicBall->AttackStart(shotPos, direction);

}

void Player::SetAnimation(size_t actionIndex, bool isLoop) {
	BaseCharactor::SetAnimation(actionIndex, isLoop);
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);
}

void Player::DissolveUpdate() {
	obj_->threshold_ = animations_[actionIndex_].GetAnimationTime() / animations_[actionIndex_].GetDuration();
	obj_->threshold_ = std::clamp(obj_->threshold_, 0.0f, animations_[actionIndex_].GetDuration());
}

void Player::Move() {

	//移動ベクトル
	Vector3 move{};

	move = DaiEngine::Input::GetInstance()->GetMoveXZ();
	move = (move / SHRT_MAX) * (speed_ * speedRate_);

	GetNowAnimation().SetAnimationSpeed(baseAnimationSpeed_ * speedRate_);
	
	move = TransformNormal(move, GetRotateYMatrix(followCamera_->GetRotateMat()));

	obj_->worldTransform_.translation_ += move.GetXZ();

	SetDirection(move.Normalize());

}

void Player::DecideMoveBehavior() {

	if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
		if (DaiEngine::Input::GetInstance()->PushButton(DaiEngine::Input::Button::A) && IsDash()) {
			ChangeBehavior("Dash");
		}
		else {
			ChangeBehavior("Jog");
		}
	}
	else {
		ChangeBehavior("Idel");
	}

}

bool Player::AttackCommand() {

	if (DaiEngine::Input::GetInstance()->PushButton(DaiEngine::Input::Button::X)) {
		chargeCount_ += kDeltaTime_;
		//一定時間押したらチャージ開始
		if (chargeCount_ >= chargeAttackTime_) {
			ChangeBehavior("ChargeAttack");
			return true;
		}
	}

	if (DaiEngine::Input::GetInstance()->ReleaseButton(DaiEngine::Input::Button::X)) {
		if (chargeCount_ < chargeAttackTime_) {
			ChangeBehavior("Attack");
			return true;
		}
	}

	return false;
}

void Player::Dead() {
	isDead_ = true;
	EndHandEff();
	ChangeState("Normal");
	ChangeBehavior("Dead");
}

void Player::StartHandEff() {
	for (auto& [group, particle] : handEff_) {
		particle->particleData_.isLoop_ = true;
	}
}

void Player::EndHandEff() {
	for (auto& [group, particle] : handEff_) {
		particle->particleData_.isLoop_ = false;
	}
}

void Player::StartBurnEff() {
	for (auto& [group, particle] : burnEff_) {
		particle->particleData_.isLoop_ = true;
	}
}

void Player::EndBurnEff() {
	for (auto& [group, particle] : burnEff_) {
		particle->particleData_.isLoop_ = false;
	}
}

void Player::StartChilledEff() {
	for (auto& [group, particle] : chilledEff_) {
		particle->particleData_.isLoop_ = true;
	}
}

void Player::EndChilledEff() {
	for (auto& [group, particle] : chilledEff_) {
		particle->particleData_.isLoop_ = false;
	}
}

void Player::StartChargeEff() {
	for (auto& [group, particle] : chargeEff_) {
		particle->particleData_.isLoop_ = true;
	}
}

void Player::EndChargeEff() {
	for (auto& [group, particle] : chargeEff_) {
		particle->particleData_.isLoop_ = false;
	}
}

void Player::ChargeEffPosUpdate() {
	Vector3 offset = { 0.0f,0.0f,1.0f };
	offset = TransformNormal(offset, rotateMat_);

	for (auto& [group, particle] : chargeEff_) {
		particle->particleData_.emitter_.translate = GetPlayerMidHandPos(offset);
	}
}

void Player::HandEffPosUpdate(const std::string& side) {
	if (side == "right") {
		for (auto& [group, particle] : handEff_) {
			particle->particleData_.emitter_.translate = Transform(skeletons_[actionIndex_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_);
		}
	}else if(side=="left") {
		for (auto& [group, particle] : handEff_) {
			particle->particleData_.emitter_.translate = Transform(skeletons_[actionIndex_].GetSkeletonPos("mixamorig1:LeftHand"), obj_->worldTransform_.matWorld_);
		}
	}
}

Vector3 Player::GetPlayerMidHandPos(const Vector3& offset) {

	obj_->worldTransform_.UpdateMatrix();
	skeletons_[actionIndex_].Update();
	//両手の間から
	Vector3 RHandPos = Transform(skeletons_[actionIndex_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_);
	Vector3 LHandPos = Transform(skeletons_[actionIndex_].GetSkeletonPos("mixamorig1:LeftHand"), obj_->worldTransform_.matWorld_);
	Vector3 pos = (RHandPos + LHandPos) / 2.0f;
	pos += offset;

	return pos;
}
