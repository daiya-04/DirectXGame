#include "Player.h"

void Player::Init(std::vector<Object3d*> models)
{
	models_ = models;

	world_.Init();
	world_Arrow_.Init();

	playerQua_ = IdentityQuaternion();
	moveQua_ = IdentityQuaternion();
	ArrowQua_ = IdentityQuaternion();
	rotateQua = IdentityQuaternion();

	Character::SetColliderSize({1.0f,2.0f,1.0f});

}

void Player::Update()
{
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
	Character::ColliderUpdate();

	canGrap = false;
}

void Player::Draw(const Camera& camera)
{
	//プレイヤー		
	models_[0]->Draw(world_, camera);

	//矢印
	if (behavior_ == Behavior::kGrap) {
		models_[1]->Draw(world_Arrow_, camera);
	}

}

void Player::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::InputFloat3("translate",&world_.translation_.x);
	ImGui::End();
	if (ImGui::Button("Reset")) {
		world_.translation_ = { 0.0f,0.0f,0.0f };
		world_.UpdateMatrix();
		behaviorRequest_ = Behavior::kRoot;
	}
#endif
}

void Player::QuaternionUpdate()
{
	moveQua_ = moveQua_.Normalize();
	playerQua_ = playerQua_.Normalize();
	moveQua_ = Slerp(playerQua_, moveQua_, 0.3f);
	moveQua_ = moveQua_ * rotateQua.Normalize();
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
	world_.translation_.y -= 0.98f;
}

#pragma region
void Player::BehaviorRootInit()
{
	ArrowQua_ = IdentityQuaternion();
	rotateQua = IdentityQuaternion();
}

void Player::BehaviorRootUpdate()
{
	//Bでジャンプ
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A) && IsOnGraund == true) {
		behaviorRequest_ = Behavior::kJump;
	}
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		//前のフレームでは押していない
			if (canGrap) {
				behaviorRequest_ = Behavior::kGrap;
			}
	}
	Move();

	Gravity();
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
	world_.translation_ = grapPoint;
	world_.UpdateMatrix();
	world_Arrow_.translation_ = grapPoint;
	world_Arrow_.UpdateMatrix();
	rotateQua = IdentityQuaternion();
	moveQua_ = IdentityQuaternion();
	playerQua_ = IdentityQuaternion();

	beginVecQua = IdentityQuaternion();
	endVecQua = IdentityQuaternion();
	lerpQua = IdentityQuaternion();
	IsOnGraund = false;
	angleParam = 0.0f;
	moveVector = { 0.0f,0.0f,0.0f };
	grapJump = false;
	grapJumpAnime = 0;
	angle = 1.0f;
	GrapBehaviorRequest_ = GrapBehavior::kLeft;
	canGrap = false;
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

	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		//前のフレームでは押していない
			if (canGrap == true && grapJump == true) {
				behaviorRequest_ = Behavior::kGrap;
			}
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

	if (IsOnGraund) {
		behaviorRequest_ = Behavior::kRoot;
	}

}
void Player::GrapJumpLeftInitalize()
{
	rotateQua = IdentityQuaternion();
	moveQua_ = IdentityQuaternion();
	playerQua_ = IdentityQuaternion();
	beginVecQua = IdentityQuaternion();
	endVecQua = IdentityQuaternion();
	lerpQua = IdentityQuaternion();
	angleParam = 0.0f;
	grapJumpAnime = 0;
	angle = 1.0f;
	Vector3 cross = Cross(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();
	beginVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(-1.0f));
	ArrowQua_ = beginVecQua.Normalize();
	beginVecQua = beginVecQua.Normalize();
	endVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	endVecQua = endVecQua.Normalize();
}
void Player::GrapJumpLeftUpdate()
{
	grapJumpVec = { 1.0f,0.0f,0.0f };
	//////回転行列を作る
	lerpQua = lerpQua.Normalize();
	Matrix4x4 rotateMatrix = lerpQua.MakeRotateMatrix();
	//移動ベクトルをカメラの角度だけ回転
	grapJumpVec = TransformNormal(grapJumpVec, rotateMatrix);
	grapJumpVec = grapJumpVec.Normalize();
	Vector3 cross = Cross(Vector3{ -1.0f,0.0f,0.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		//前のフレームでは押していない
			if (grapJump == false) {
				behaviorRequest_ = Behavior::kRoot;
			}
	}
	if (Input::GetInstance()->PushButton(XINPUT_GAMEPAD_X)) {
		if (angle > 0.9f) {
			angle -= 0.001f;
		}
		else {
			angle = 0.9f;
		}
		rotateQua = MakwRotateAxisAngleQuaternion(cross, std::acos(angle));
		rotateQua = rotateQua.Normalize();

		angleParam += kParam;
		if (0.0f > angleParam || angleParam > 1.0f) {
			kParam *= -1;
		}
		lerpQua = Slerp(beginVecQua, endVecQua, angleParam);

		ArrowQua_ = lerpQua.Normalize();

	}
	else if (Input::GetInstance()->ReleaseButton(XINPUT_GAMEPAD_X) && grapJump == false) {
			grapJump = true;
			moveVector = grapJumpVec;
	}
	if (grapJump == true) {
		moveVector.y -= 0.03f;
		world_.translation_.x += moveVector.x;
		world_.translation_.y += moveVector.y;
		world_.translation_.z += moveVector.z;
		grapJumpAnime++;
	}
}
void Player::GrapJumpRightInitalize()
{
	rotateQua = IdentityQuaternion();
	moveQua_ = IdentityQuaternion();
	playerQua_ = IdentityQuaternion();
	beginVecQua = IdentityQuaternion();
	endVecQua = IdentityQuaternion();
	lerpQua = IdentityQuaternion();
	angleParam = 0.0f;
	grapJumpAnime = 0;
	angle = 1.0f;
	Vector3 cross = Cross(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();
	beginVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(1.0f));
	ArrowQua_ = beginVecQua.Normalize();
	beginVecQua = beginVecQua.Normalize();
	endVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	endVecQua = endVecQua.Normalize();
}
void Player::GrapJumpRightUpdate()
{
	grapJumpVec = { 1.0f,0.0f,0.0f };
	//////回転行列を作る
	lerpQua = lerpQua.Normalize();
	Matrix4x4 rotateMatrix = lerpQua.MakeRotateMatrix();
	//移動ベクトルをカメラの角度だけ回転
	grapJumpVec = TransformNormal(grapJumpVec, rotateMatrix);
	grapJumpVec = grapJumpVec.Normalize();
	Vector3 cross = Cross(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();
	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		//前のフレームでは押していない
		if (grapJump == false) {
			behaviorRequest_ = Behavior::kRoot;
		}
	}
	if (Input::GetInstance()->PushButton(XINPUT_GAMEPAD_X)) {
		if (angle > 0.9f) {
			angle -= 0.001f;
		}
		else {
			angle = 0.9f;
		}
		rotateQua = MakwRotateAxisAngleQuaternion(cross, std::acos(angle));
		rotateQua = rotateQua.Normalize();

		angleParam += kParam;
		if (0.0f > angleParam || angleParam > 1.0f) {
			kParam *= -1;
		}
		lerpQua = Slerp(beginVecQua, endVecQua, angleParam);

		ArrowQua_ = lerpQua.Normalize();

	}
	else if (Input::GetInstance()->ReleaseButton(XINPUT_GAMEPAD_X) && grapJump == false) {
		grapJump = true;
		moveVector = grapJumpVec;
	}
	if (grapJump == true) {
		moveVector.y -= 0.03f;
		world_.translation_.x += moveVector.x;
		world_.translation_.y += moveVector.y;
		world_.translation_.z += moveVector.z;
		grapJumpAnime++;
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

		move.x = move.x * speed;
		move.y = move.y * speed;
		move.z = move.z * speed;
		//カメラの正面方向に移動するようにする
		//回転行列を作る
		Matrix4x4 rotateMatrix = MakeRotateXMatrix(camera_->rotation_.x) * MakeRotateYMatrix(camera_->rotation_.y) * MakeRotateZMatrix(camera_->rotation_.z);
		//移動ベクトルをカメラの角度だけ回転
		move = TransformNormal(move, rotateMatrix);
		//移動
		move.y = 0.0f;
		world_.translation_ = world_.translation_ + move;
		//プレイヤーの向きを移動方向に合わせる
		move = move.Normalize();
		Vector3 cross = Cross({ 0.0f,0.0f,1.0f }, move);
		cross = cross.Normalize();
		float dot = Dot({ 0.0f,0.0f,1.0f }, move);
		//後ろを向いたら後ろ向きにする
		if (move.z == -1.0f) {
			cross.y = -1.0f;
		}
		moveQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(dot));
}