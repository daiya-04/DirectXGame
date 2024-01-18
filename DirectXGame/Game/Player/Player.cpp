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
	directionQua_ = IdentityQuaternion();
	moveParam = 0.3f;
	Character::SetColliderSize({1.0f,1.0f,1.0f});
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

	IsOnGraund = false;
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

	if (ImGui::Button("Reset")) {
		world_.translation_ = { 0.0f,0.0f,0.0f };
		world_.UpdateMatrix();
		behaviorRequest_ = Behavior::kRoot;
	}
	ImGui::InputInt("sangoId", &sangoId_);
	ImGui::InputInt("sangoId", &PreSangoId_);
	ImGui::End();
	if (world_.translation_.y < -20.0f) {
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
	if (behavior_ == Behavior::kRoot) {
		moveQua_ = Slerp(playerQua_, moveQua_, moveParam);
	}

	moveQua_ = moveQua_.Normalize() * rotateQua.Normalize();
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
	moveParam = 0.3f;
	ArrowQua_ = IdentityQuaternion();
	rotateQua = IdentityQuaternion();
	directionQua_ = IdentityQuaternion();
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
	moveParam = 1.0f;
	PreSangoId_ = sangoId_;
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
	angle = 1.0f;
	GrapBehaviorRequest_ = GrapBehavior::kLeft;
	canGrap = false;
	DeletePreIdTime_ = 0.0f;
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

	if (Input::GetInstance()->PushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		//前のフレームでで押していてもOK
			if (canGrap == true && grapJump == true && PreSangoId_ != sangoId_) {
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

	if (grapJump) {
		if (DeletePreIdTime_ < kDeletePreIdTime_) {
		DeletePreIdTime_+= 1.0f;
		}
		if (DeletePreIdTime_ >= kDeletePreIdTime_) {
		PreSangoId_ = -1;
		}
	}
	

	if (IsOnGraund) {
		behaviorRequest_ = Behavior::kRoot;
	}
	canGrap = false;
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
	angle = 1.0f;
	Vector3 cross = Cross(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();
	beginVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(-1.0f));
	ArrowQua_ = beginVecQua.Normalize();
	beginVecQua = beginVecQua.Normalize();
	endVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	endVecQua = endVecQua.Normalize();
	sangoDirection_.x = -1.0f;
	cross = Cross(Vector3{ -1.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,1.0f });
	cross = cross.Normalize();
	directionQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	moveQua_ = directionQua_.Normalize();

}
void Player::GrapJumpLeftUpdate()
{
	float Norm = moveQua_.Norm();
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::InputFloat("Norm", &Norm);
	ImGui::End();
#endif
	grapJumpVec = { 1.0f,0.0f,0.0f };
	//////回転行列を作る
	lerpQua = lerpQua.Normalize();
	Matrix4x4 rotateMatrix = lerpQua.MakeRotateMatrix();
	//移動ベクトルをカメラの角度だけ回転
	grapJumpVec = TransformNormal(grapJumpVec, rotateMatrix);
	grapJumpVec = grapJumpVec.Normalize();
	Vector3 cross = Cross(Vector3{ sangoDirection_ }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();
	
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
			moveVector = grapJumpVec * jumpParam;
	}
	if (grapJump == true) {
		if (moveVector.y > -0.98f) {
			moveVector.y -= 0.03f;
		}
		else if (moveVector.y < -0.98f) {
			moveVector.y = -0.98f;
		}

		world_.translation_.x += moveVector.x;
		world_.translation_.y += moveVector.y;
		world_.translation_.z += moveVector.z;
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
	angle = 1.0f;
	Vector3 cross = Cross(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();
	beginVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(1.0f));
	ArrowQua_ = beginVecQua.Normalize();
	beginVecQua = beginVecQua.Normalize();
	endVecQua = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	endVecQua = endVecQua.Normalize();
	cross = Cross(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,1.0f });
	cross = cross.Normalize();
	directionQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(0.0f));
	moveQua_ = directionQua_.Normalize();
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
	Vector3 cross = Cross(Vector3{ sangoDirection_ }, Vector3{ 0.0f,1.0f,0.0f });
	cross = cross.Normalize();

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
		moveVector = grapJumpVec * jumpParam;
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