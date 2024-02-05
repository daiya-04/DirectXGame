#include "Player.h"

void Player::Init(std::vector<Object3d*> models)
{
	models_ = models;

	world_.Init();
	world_Arrow_.Init();

	behavior_ = Behavior::kRoot;

	playerQua_ = IdentityQuaternion();
	moveQua_ = IdentityQuaternion();
	ArrowQua_ = IdentityQuaternion();
	rotateQua = IdentityQuaternion();
	directionQua_ = IdentityQuaternion();
	moveParam = 0.3f;
	Character::SetColliderSize({ 1.0f,1.0f,1.0f });
	Character::SetOffset({0.0f,1.0f,0.0f});
}

void Player::Update()
{
	tlanslatePre = world_.translation_;

	if (behaviorRequest_) {
		//ふるまいの変更
		behavior_ = behaviorRequest_.value();
		//各ふるまいごとに初期化
		switch (behavior_)
		{
		case Behavior::kRoot:
		default:
			BehaviorRootInit();
			break;
		case Behavior::kGrap:
			GrapInit();
			break;
		case Behavior::kJump:
			BehaviorJumpInit();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kGrap:
		GrapUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}

	WorldUpdate();
	
	if (world_.translation_.y < -20.0f) {
		world_.translation_ = { 0.0f,2.0f,0.0f };
		world_.UpdateMatrix();
		behaviorRequest_ = Behavior::kRoot;
	}
	
	Character::ColliderUpdate();

	if (world_.translation_.y < -20.0f) {
		world_.translation_ = { 0.0f,0.0f,0.0f };
		world_.UpdateMatrix();
		behaviorRequest_ = Behavior::kRoot;
	}

	IsOnGraund = false;
}
void Player::Draw(const Camera& camera)
{
	//プレイヤー		
	models_[0]->Draw(world_, camera);

	//矢印
	if ((behavior_ == Behavior::kGrap) && grapJump == false) {
		models_[1]->Draw(world_Arrow_, camera);
	}

}
void Player::hitBox(Vector3 colliderPos, Vector3 HitcolliderSize)
{
#pragma region
	if (tlanslatePre.y - colliderSize.y + offset_.y < colliderPos.y + HitcolliderSize.y && tlanslatePre.y + colliderSize.y + offset_.y > colliderPos.y - HitcolliderSize.y) {
		if (tlanslatePre.x > colliderPos.x + colliderSize.x) {
			//左から右
			if (world_.translation_.x - colliderSize.x < colliderPos.x + HitcolliderSize.x) {
				world_.translation_.x = colliderPos.x + HitcolliderSize.x + colliderSize.x;
			}
		}
		if (tlanslatePre.x < colliderPos.x - HitcolliderSize.x) {
			//右から左
			if (world_.translation_.x + colliderSize.x > colliderPos.x - HitcolliderSize.x) {
				world_.translation_.x = colliderPos.x - HitcolliderSize.x - colliderSize.x;
			}
		}
	}

	if (tlanslatePre.y >= colliderPos.y + HitcolliderSize.y) {
		//上から下
		if (world_.translation_.y - colliderSize.y < colliderPos.y + HitcolliderSize.y) {
			world_.translation_.y = colliderPos.y + HitcolliderSize.y;
		}
	}
	if (tlanslatePre.y < colliderPos.y - HitcolliderSize.y) {
		//下から上
		if (world_.translation_.y + colliderSize.y > colliderPos.y - HitcolliderSize.y) {
			world_.translation_.y = colliderPos.y - HitcolliderSize.y;
		}
	}
	if (tlanslatePre.y - colliderSize.y < colliderPos.y + HitcolliderSize.y && tlanslatePre.y + colliderSize.y > colliderPos.y - HitcolliderSize.y) {
		if (tlanslatePre.z < colliderPos.z - HitcolliderSize.z) {
			//手前から奥
			if (world_.translation_.z + colliderSize.z > colliderPos.z - HitcolliderSize.z) {
				world_.translation_.z = colliderPos.z - HitcolliderSize.z - colliderSize.z;
			}
		}
		if (tlanslatePre.z > colliderPos.z + HitcolliderSize.z) {
			//奥から手前
			if (world_.translation_.z - colliderSize.z < colliderPos.z + HitcolliderSize.z) {
				world_.translation_.z = colliderPos.z + HitcolliderSize.z + colliderSize.z;
			}
		}
	}
#pragma endregion 移動制御	
	world_.UpdateMatrixQua(moveQua_.MakeRotateMatrix());
	IsOnGraund = true;
}
void Player::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::InputFloat3("translate", &world_.translation_.x);
	ImGui::InputFloat("MoveSpeed", &speed);
	ImGui::InputFloat("JumpMaxValue", &kjumpParam);
	ImGui::InputFloat("JumpaddValue", &addJumpParam);
	ImGui::InputFloat("JumpsubValue", &subJumpParam);
	ImGui::InputFloat("MoveSpeed", &speed);

	if (ImGui::Button("Reset")) {
		world_.translation_ = { 0.0f,0.0f,0.0f };
		world_.UpdateMatrix();
		behaviorRequest_ = Behavior::kRoot;
	}
	
	ImGui::End();
#endif
}

void Player::QuaternionUpdate()
{
	moveQua_ = moveQua_.Normalize();
	playerQua_ = playerQua_.Normalize();
	if (behavior_ == Behavior::kRoot) {
		moveQua_ = Slerp(playerQua_, moveQua_, moveParam);
	}
	/*if (behavior_ == Behavior::kGrap) {
		moveQua_ = Slerp(playerQua_, moveQua_, moveParam);
	}*/
}
void Player::WorldUpdate()
{
	QuaternionUpdate();
	playerQua_ = moveQua_;
	world_.UpdateMatrixQua(moveQua_.MakeRotateMatrix());

	world_Arrow_.UpdateMatrixQua(ArrowQua_.MakeRotateMatrix());
}
void Player::Gravity()
{
	world_.translation_.y += kGravity;
}
#pragma region
void Player::BehaviorRootInit()
{
	farstFlag = true;
	moveParam = 0.3f;
	ArrowQua_ = IdentityQuaternion();
	rotateQua = IdentityQuaternion();
	directionQua_ = IdentityQuaternion();
	moveQua_ = IdentityQuaternion();
	playerQua_ = IdentityQuaternion();
	rotateQua = IdentityQuaternion();
	angle = 1.0f;
	jumpParam = 0.0f;
}
void Player::BehaviorRootUpdate()
{
	//Bでジャンプ
	/*if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A) && IsOnGraund == true) {
		behaviorRequest_ = Behavior::kJump;
	}*/
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		//前のフレームでは押していない
		if (canGrap) {
			behaviorRequest_ = Behavior::kGrap;
		}
	}
	Move();
	Gravity();
	canGrap = false;
}
void Player::BehaviorJumpInit()
{
}
void Player::BehaviorJumpUpdate()
{
}
#pragma endregion Behavior
#pragma region
void Player::GrapInit()
{
	farstFlag = false;
	moveParam = 1.0f;
	PreSangoId_ = sangoId_;
	world_.translation_ = grapPoint;
	world_.UpdateMatrix();
	world_Arrow_.translation_ = grapPoint;
	world_Arrow_.UpdateMatrix();
	playerQua_ = IdentityQuaternion();
	IsOnGraund = false;
	moveVector = { 0.0f,0.0f,0.0f };
	grapJump = false;
	GrapBehaviorRequest_ = GrapBehavior::kLeft;
	canGrap = false;
	DeletePreIdTime_ = 0.0f;
	secondJump = false;
	JumpFlame = 0;
}
void Player::GrapUpdate()
{
	if (Input::GetInstance()->GetMoveXZ().x != 0 && grapJump == false) {
		if (Input::GetInstance()->GetMoveXZ().x > 0 && GrapBehavior_ != GrapBehavior::kRight) {
			GrapBehaviorRequest_ = GrapBehavior::kRight;
		}
		else if (Input::GetInstance()->GetMoveXZ().x < 0 && GrapBehavior_ != GrapBehavior::kLeft) {
			GrapBehaviorRequest_ = GrapBehavior::kLeft;
		}
	}

		//自動でつかむ
		if (canGrap == true && grapJump == true && PreSangoId_ != sangoId_) {
			behaviorRequest_ = Behavior::kGrap;
		}
	

	if (GrapBehaviorRequest_) {
		//ふるまいの変更
		GrapBehavior_ = GrapBehaviorRequest_.value();
		//各ふるまいごとに初期化
		switch (GrapBehavior_)
		{
		case GrapBehavior::kLeft:
			GrapJumpLeftInitalize();
			break;
		case GrapBehavior::kRight:
		default:
			GrapJumpRightInitalize();
			break;
		}
		GrapBehaviorRequest_ = std::nullopt;
	}
	switch (GrapBehavior_)
	{
	case GrapBehavior::kLeft:
		GrapJumpLeftUpdate();
		break;
	case GrapBehavior::kRight:
	default:
		GrapJumpRightUpdate();
		break;
	}

	GrapSelectDirection();

	if (grapJump) {
		//当たり判定履歴を削除するまでの時間
		if (DeletePreIdTime_ < kDeletePreIdTime_) {
			DeletePreIdTime_ += 1.0f;
		}
		if (DeletePreIdTime_ >= kDeletePreIdTime_) {
			PreSangoId_ = -1;
		}
		//2段ジャンプ
		/*if (secondJump == false && Input::GetInstance()->PushButton(XINPUT_GAMEPAD_X)) {
			moveVector = secondJumpVec/2;
			secondJump = true;
		}*/
		JumpFlame++;
		if (JumpFlame == 45) {
			angle = 0.9f;
		}
		Vector3 cross = Cross(Vector3{ 0.0f,0.0f,1.0f }, Vector3{ 0.0f,1.0f,0.0f });
		cross = cross.Normalize();
		rotateQua = MakwRotateAxisAngleQuaternion(cross, std::acos(angle));
	}
	if (secondJump == false) {
		moveQua_ = moveQua_.Normalize() * rotateQua.Normalize();
	}
	/*if (secondJump) {
		Move();
	}*/
	if (Input::GetInstance()->PushButton(XINPUT_GAMEPAD_X)) {
		if (jumpParam < kjumpParam) {
		jumpParam += addJumpParam;
		}
		else if (jumpParam >= kjumpParam) {
		jumpParam = kjumpParam;
		}
	}
	else if (jumpParam > 0.0f) {
		jumpParam -= subJumpParam;
	}
	else if (jumpParam < 0.0f) {
		jumpParam = 0.0f;
	}
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::InputFloat("JumpParameter",&jumpParam);
	ImGui::End();
#endif

	if (IsOnGraund) {
		behaviorRequest_ = Behavior::kRoot;
	}
	canGrap = false;
}
void Player::GrapSelectDirection()
{
	if (Input::GetInstance()->GetMoveXZ().x == 0 && Input::GetInstance()->GetMoveXZ().z == 0) {
		return;
	}

	move = Input::GetInstance()->GetMoveXZ();
	//正規化をして斜めの移動量を正しくする
	move = move.Normalize();

	//プレイヤーの向きを移動方向に合わせる
	move = move.Normalize();
	Vector3 Direction;
	Vector3 dotVec;
	if (move.z > 0) {
		Direction = {0.0f,-1.0f,0.0f};
	}else if(move.z < 0) {
		Direction = { 0.0f,1.0f,0.0f };
	}
	Vector3 cross = Cross(Direction, Vector3{ 1.0f,0.0f,0.0f });
	cross = cross.Normalize();
	float dot = Dot({ 1.0f,0.0f,0.0f }, move);
	//後ろを向いたら後ろ向きにする
	if (move.x == -1.0f) {
		cross.y = 1.0f;
	}
	ArrowQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(dot));

}
void Player::GrapJumpLeftInitalize()
{
	playerQua_ = IdentityQuaternion();
	Vector3 cross = Cross(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,1.0f });
	cross = cross.Normalize();
	directionQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	moveQua_ = directionQua_.Normalize();
	secondJumpVec = {-1.0f,1.0f,0.0f};
}
void Player::GrapJumpLeftUpdate()
{

	grapJumpVec = { 1.0f,0.0f,0.0f };
	//////回転行列を作る
	ArrowQua_ = ArrowQua_.Normalize();
	Matrix4x4 rotateMatrix = ArrowQua_.MakeRotateMatrix();
	//移動ベクトルをカメラの角度だけ回転
	grapJumpVec = TransformNormal(grapJumpVec, rotateMatrix);
	grapJumpVec = grapJumpVec.Normalize();
	Vector3 cross = Cross(Vector3{ 0.0f,0.0f,1.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();

	if (Input::GetInstance()->PushButton(XINPUT_GAMEPAD_X)) {
		if (angle > kAngleMax) {
			angle -= 0.001f;
		}
		else {
			angle = kAngleMax;
		}
		rotateQua = MakwRotateAxisAngleQuaternion(cross, std::acos(angle));
		rotateQua = rotateQua.Normalize();
	}
	else if (Input::GetInstance()->ReleaseButton(XINPUT_GAMEPAD_X) && grapJump == false) {
		grapJump = true;
		moveVector = grapJumpVec * jumpParam;
	}
	else if(grapJump == false) {
		if (angle < 1.0f) {
			angle += 0.001f;
		}
		else {
			angle = 1.0f;
		}
		rotateQua = MakwRotateAxisAngleQuaternion(cross, std::acos(angle));
	}
	if (grapJump == true) {
		if (moveVector.y > kGravity) {
			moveVector.y -= 0.03f;
		}
		else if (moveVector.y < kGravity) {
			moveVector.y = kGravity;
		}

		world_.translation_.x += moveVector.x;
		world_.translation_.y += moveVector.y;
		world_.translation_.z += moveVector.z;
	}
}
void Player::GrapJumpRightInitalize()
{
	moveQua_ = IdentityQuaternion();
	playerQua_ = IdentityQuaternion();
	Vector3 cross = Cross(Vector3{ -1.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,1.0f });
	cross = cross.Normalize();
	directionQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	moveQua_ = directionQua_.Normalize();
	secondJumpVec = { 1.0f,1.0f,0.0f };
}
void Player::GrapJumpRightUpdate()
{
	grapJumpVec = { 1.0f,0.0f,0.0f };
	//////回転行列を作る
	ArrowQua_ = ArrowQua_.Normalize();
	Matrix4x4 rotateMatrix = ArrowQua_.MakeRotateMatrix();
	//移動ベクトルをカメラの角度だけ回転
	grapJumpVec = TransformNormal(grapJumpVec, rotateMatrix);
	grapJumpVec = grapJumpVec.Normalize();
	Vector3 cross = Cross(Vector3{ 0.0f,0.0f,1.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();

	if (Input::GetInstance()->PushButton(XINPUT_GAMEPAD_X)) {
		if (angle > kAngleMax) {
			angle -= 0.001f;
		}
		else {
			angle = kAngleMax;
		}
		rotateQua = MakwRotateAxisAngleQuaternion(cross, std::acos(angle));
		rotateQua = rotateQua.Normalize();

	}
	else if (Input::GetInstance()->ReleaseButton(XINPUT_GAMEPAD_X) && grapJump == false) {
		grapJump = true;

		moveVector = grapJumpVec * jumpParam;
	}
	else if (grapJump == false) {
		if (angle < 1.0f) {
			angle += 0.001f;
		}
		else {
			angle = 1.0f;
		}
		rotateQua = MakwRotateAxisAngleQuaternion(cross, std::acos(angle));
	}


	if (grapJump == true) {
		moveVector.y -= 0.03f;
		world_.translation_.x += moveVector.x;
		world_.translation_.y += moveVector.y;
		world_.translation_.z += moveVector.z;
	}
}
#pragma endregion Grap
void Player::Move()
{
	//移動量
	if (Input::GetInstance()->GetMoveXZ().x == 0 && Input::GetInstance()->GetMoveXZ().z == 0) {
		return;
	}

	move = Input::GetInstance()->GetMoveXZ();
	//正規化をして斜めの移動量を正しくする
	move = move.Normalize();
	move.y = 0.0f;
	move.z = 0.0f;
	move.x = move.x * speed;
	move.y = move.y * speed;
	move.z = move.z * speed;
	//カメラの正面方向に移動するようにする
	//回転行列を作る
	//Matrix4x4 rotateMatrix = MakeRotateXMatrix(camera_->rotation_.x) * MakeRotateYMatrix(camera_->rotation_.y) * MakeRotateZMatrix(camera_->rotation_.z);
	////移動ベクトルをカメラの角度だけ回転
	//move = TransformNormal(move, rotateMatrix);
	//移動

	world_.translation_ = world_.translation_ + move;
	//プレイヤーの向きを移動方向に合わせる
	move = move.Normalize();
	if (move.x > 0) {
		direction.x = 1.0f;
	}
	else if (move.x < 0) {
		direction.x = -1.0f;
	}
	Vector3 cross = Cross(Vector3{ 0.0f,0.0f,1.0f }, direction);
	cross = cross.Normalize();
	float dot = Dot({ 0.0f,0.0f,1.0f }, move);

	moveQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(dot));
}