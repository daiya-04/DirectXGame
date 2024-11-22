#include "Player.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Easing.h"
#include "TextureManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "GameScene.h"

const std::array<Player::ComboAttack, Player::comboNum_> Player::kComboAttacks_ = {
	{
		{30,20,5},
		{30,20,5},
		{30,20,10}
	}
};

void Player::Init(const std::vector<std::shared_ptr<Model>>& models){

	actionIndex_ = Action::Standing;
	
	BaseCharactor::Init(models);

	behaviorRequest_ = Behavior::kRoot;

	hp_ = maxHp_;

	hpBar_.reset(Sprite::Create(TextureManager::Load("Player_HP.png"), {440.0f,700.0f}));
	hpBar_->SetAnchorpoint({ 0.0f,0.5f });
	hpBar_->SetSize({ 400.0f,10.0f });

	//行列更新
	BaseCharactor::Update();
}

void Player::Update(){

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		behaviorInitTable_[behavior_]();
		
		behaviorRequest_ = std::nullopt;
	}

	behaviorUpdateTable_[behavior_]();

	obj_->worldTransform_.translation_.x = std::clamp(obj_->worldTransform_.translation_.x, -50.0f, 50.0f);
	obj_->worldTransform_.translation_.z = std::clamp(obj_->worldTransform_.translation_.z, -20.0f, 80.0f);

	BaseCharactor::Update();

	UpdateUI();
}

void Player::UpdateUI() {

	float percent = static_cast<float>(hp_) / static_cast<float>(maxHp_);

	hpBar_->SetSize({ 400.0f * percent,10.0f });
}

void Player::Draw(const Camera& camera){

#ifdef _DEBUG
	ShapesDraw::DrawOBB(collider_, camera);
#endif // _DEBUG


	obj_->Draw(camera);
	skeletons_[actionIndex_].Draw(obj_->worldTransform_, camera);

}

void Player::DrawUI() {
	hpBar_->Draw();
}

void Player::OnCollision() {
	hp_--;
	if (hp_ <= 0) {
		isDead_ = true;
		behaviorRequest_ = Behavior::kDead;
	}
}

void Player::RootInit() {

	isAttack_ = false;
	actionIndex_ = Action::Standing;
	animations_[actionIndex_].Start();
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);

}

void Player::RootUpdate() {

	Vector3 move{};
	Vector3 zeroVector{};
	const float speed = 0.3f;

	if (Input::GetInstance()->TriggerLStick(Input::Stick::All)) {
		actionIndex_ = Action::Walking;
		animations_[actionIndex_].Start();
	}
	if (Input::GetInstance()->ReleaseLStick(Input::Stick::All)) {
		actionIndex_ = Action::Standing;
		animations_[actionIndex_].Start();
	}

	move = Input::GetInstance()->GetMoveXZ();
	move = move / SHRT_MAX * speed;

	//攻撃
	if (Input::GetInstance()->TriggerButton(Input::Button::X)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	////ダッシュ
	//if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
	//	behaviorRequest_ = Behavior::kDash;
	//}

	move = TransformNormal(move, MakeRotateYMatrix(followCamera_->GetCamera().rotation_.y));

	obj_->worldTransform_.translation_ += move;
	obj_->worldTransform_.translation_.y = 0.0f;

	if (move != zeroVector) {
		direction_ = move;
	}

	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

}

void Player::AttackInit() {

	actionIndex_ = Action::Attack;
	animations_[actionIndex_].Start(false);
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);
	animations_[actionIndex_].SetAnimationSpeed(1.0f / 30.0f);
	skeletons_[actionIndex_].Update();
	workAttack_.attackParam_ = 0;
	workAttack_.speed_ = 1.0f;

	if (target_) {

		Vector3 direction = target_->translation_ - obj_->worldTransform_.translation_;

		if (direction.Length() <= attackRange_) {
			rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction);
		}
		
	}
	
}

void Player::AttackUpdate() {

	if (workAttack_.comboIndex_ < comboNum_ - 1) {
		if (Input::GetInstance()->TriggerButton(Input::Button::X)) {
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
		//animations_[actionIndex_].Start();
		if (workAttack_.comboNext_) {
			workAttack_.comboNext_ = false;
			workAttack_.comboIndex_++;
			workAttack_.comboIndex_ = std::clamp(workAttack_.comboIndex_, 0U, 2U);

			AttackInit();
		}
		else {
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.comboIndex_ = 0;
			return;
		}
	}

	
	if (workAttack_.attackParam_ == kComboAttacks_[workAttack_.comboIndex_].chargeTime_) {

		Vector3 direction = { 0.0f,0.0f,1.0f };
		direction = TransformNormal(direction, rotateMat_);
		workAttack_.velocity_ = direction.Normalize() * workAttack_.speed_;

		///
		
		PlayerAttack* playerAttack = new PlayerAttack();
		std::shared_ptr<Model> attackModel = ModelManager::LoadOBJ("PlayerBullet");
		Vector3 offset = { 0.0f,0.0f,1.0f };
		offset = TransformNormal(offset, rotateMat_);
		Vector3 shotPos = Transform(skeletons_[actionIndex_].GetSkeletonPos("mixamorig1:RightHand"), obj_->worldTransform_.matWorld_) + offset;
		playerAttack->Init(attackModel, shotPos, direction);
		gameScene_->AddPlayerAttack(playerAttack);

		///
		
	}

	workAttack_.attackParam_++;
}

void Player::DashInit() {

	workDash_.dashParam_ = 0;
	workDash_.dashDirection_ = direction_;
	followCamera_->Reset();

}

void Player::DashUpdate() {

	float dashSpeed = 1.5f;
	Vector3 zeroVector{};

	if (direction_ == zeroVector) {
		workDash_.dashDirection_ = { 0.0f,0.0f,1.0f };
	}

	obj_->worldTransform_.translation_ += workDash_.dashDirection_.Normalize() * dashSpeed;

	if (++workDash_.dashParam_ >= workDash_.dashTime_) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Player::DeadInit() {

	actionIndex_ = Action::Dead;
	animations_[actionIndex_].SetAnimationSpeed(1.0f / 60.0f);
	animations_[actionIndex_].Start();

	obj_->threshold_ = 0.0f;

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, Vector3(0.0f, 0.0f, 1.0f));

}

void Player::DeadUpdate() {

	if (!animations_[actionIndex_].IsPlaying()) {
		isFinishDeadMotion_ = true;
	}

	obj_->threshold_ = animations_[actionIndex_].GetAnimationTime() / animations_[actionIndex_].GetDuration();
	obj_->threshold_ = std::clamp(obj_->threshold_, 0.0f, animations_[actionIndex_].GetDuration());

}
