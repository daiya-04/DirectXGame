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

	//アクション設定
	actionIndex_ = Action::Standing;
	
	//モデル関連の初期化
	BaseCharactor::Init(models);

	//状態の設定
	behaviorRequest_ = Behavior::kRoot;
	//HPの設定
	hp_ = maxHp_;
	hpBarSize_ = { 400.0f,10.0f };

	//UIの設定
	hpBar_.reset(Sprite::Create(TextureManager::Load("Player_HP.png"), {440.0f,700.0f}));
	hpBar_->SetAnchorpoint({ 0.0f,0.5f });
	hpBar_->SetSize(hpBarSize_);

}

void Player::Update(){

	//状態が切り替わった時の初期化処理
	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		behaviorInitTable_[behavior_]();
		
		behaviorRequest_ = std::nullopt;
	}

	//状態の更新
	behaviorUpdateTable_[behavior_]();

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

void Player::OnCollision() {
	hp_--;
	//HPが0になったら...
	if (hp_ <= 0) {
		isDead_ = true;
		behaviorRequest_ = Behavior::kDead;
	}
}

void Player::RootInit() {

	//スティック入力されたままだったら移動アニメーションに
	if (Input::GetInstance()->TiltLStick(Input::Stick::All)) {
		actionIndex_ = Action::Walking;
	}else {
		actionIndex_ = Action::Standing;
	}
	animations_[actionIndex_].Start();
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);
	
}

void Player::RootUpdate() {

	///移動計算

	//移動ベクトル
	Vector3 move{};

	Vector3 zeroVector{};
	const float speed = 0.3f;
	//スティック入力がされたら歩きのアニメーション再生
	if (Input::GetInstance()->TriggerLStick(Input::Stick::All)) {
		actionIndex_ = Action::Walking;
		animations_[actionIndex_].Start();
	}
	//スティックを話したら立ちアニメーション再生
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

	//カメラの向きを基準に上下左右に移動するようにする
	move = TransformNormal(move, MakeRotateYMatrix(followCamera_->GetCamera().rotation_.y));

	obj_->worldTransform_.translation_ += move;
	obj_->worldTransform_.translation_.y = 0.0f;

	///

	//移動するなら進む方向を設定する
	if (move != zeroVector) {
		direction_ = move;
	}

	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);

	//回転行列作成
	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

}

void Player::AttackInit() {

	actionIndex_ = Action::Attack;
	animations_[actionIndex_].Start(false);
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);
	animations_[actionIndex_].TimeReset();
	animations_[actionIndex_].SetAnimationSpeed(attackAnimeSpeed_);
	skeletons_[actionIndex_].Update();

	//攻撃用パラメータの初期化
	workAttack_.attackParam_ = 0;
	workAttack_.speed_ = 1.0f;

	//ターゲットがいたら
	if (target_) {
		//攻撃の発射方向計算
		Vector3 direction = target_->translation_ - obj_->worldTransform_.translation_;
		//射程内ならターゲット方向に向く
		if (direction.Length() <= attackRange_) {
			rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction);
		}
	}
	
}

void Player::AttackUpdate() {
	//1コンボ目、2コンボ目だったら入力を受け付ける
	if (workAttack_.comboIndex_ < comboNum_ - 1) {
		if (Input::GetInstance()->TriggerButton(Input::Button::X)) {
			workAttack_.comboNext_ = true;
		}
	}
	//攻撃の総時間計算
	uint32_t totalAttackTime = kComboAttacks_[workAttack_.comboIndex_].attackTime_;
	totalAttackTime += kComboAttacks_[workAttack_.comboIndex_].chargeTime_;
	totalAttackTime += kComboAttacks_[workAttack_.comboIndex_].recoveryTime_;

	//今コンボのどのフェーズか
	if (workAttack_.attackParam_ >= kComboAttacks_[workAttack_.comboIndex_].chargeTime_ + kComboAttacks_[workAttack_.comboIndex_].attackTime_) {
		workAttack_.InComboPhase_ = static_cast<uint32_t>(ComboPhase::Recovery);
	}else if (workAttack_.attackParam_ >= kComboAttacks_[workAttack_.comboIndex_].chargeTime_) {
		workAttack_.InComboPhase_ = static_cast<uint32_t>(ComboPhase::Attack);
	}else {
		workAttack_.InComboPhase_ = static_cast<uint32_t>(ComboPhase::Charge);
	}

	//攻撃が終了したら
	if (workAttack_.attackParam_ >= totalAttackTime) {
		//攻撃が続くか
		if (workAttack_.comboNext_) {
			workAttack_.comboNext_ = false;
			workAttack_.comboIndex_++;
			workAttack_.comboIndex_ = std::clamp(workAttack_.comboIndex_, static_cast<uint32_t>(ComboIndex::First), static_cast<uint32_t>(ComboIndex::Third));

			AttackInit();
		}
		else {
			//続かないなら通常行動に戻る
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.comboIndex_ = static_cast<uint32_t>(ComboIndex::First);
			return;
		}
	}

	
	if (workAttack_.attackParam_ == kComboAttacks_[workAttack_.comboIndex_].chargeTime_) {
		//攻撃を発射する方向の計算
		Vector3 direction = { 0.0f,0.0f,1.0f };
		direction = TransformNormal(direction, rotateMat_);
		workAttack_.velocity_ = direction.Normalize() * workAttack_.speed_;

		///攻撃の玉の生成
		
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
	animations_[actionIndex_].Start(false);

	obj_->threshold_ = 0.0f;

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, Vector3(0.0f, 0.0f, 1.0f));

}

void Player::DeadUpdate() {
	//アニメーション終了
	if (!animations_[actionIndex_].IsPlaying()) {
		isFinishDeadMotion_ = true;
	}

	obj_->threshold_ = animations_[actionIndex_].GetAnimationTime() / animations_[actionIndex_].GetDuration();
	obj_->threshold_ = std::clamp(obj_->threshold_, 0.0f, animations_[actionIndex_].GetDuration());

}
