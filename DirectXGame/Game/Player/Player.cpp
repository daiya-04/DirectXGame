#include "Player.h"

void Player::Init(std::vector<Object3d*> models)
{
	models_ = models;

	world_.Init();

	input = Input::GetInstance();

	playerQua_ = IdentityQuaternion();
	moveQua_ = IdentityQuaternion();
}

void Player::Update()
{
	//パッドの状態をゲット

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


}

void Player::Draw(const Camera& camera)
{

	for (Object3d* model : models_) {
		model->Draw(world_,camera);
	}
}

void Player::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::InputFloat3("translate",&world_.translation_.x);
	ImGui::End();
#endif
}

void Player::QuaternionUpdate()
{
	moveQua_ = moveQua_.Normalize();

	moveQua_ = Slerp(playerQua_, moveQua_, 0.3f);
}

void Player::WorldUpdate()
{
	QuaternionUpdate();
	playerQua_ = moveQua_;
	world_.UpdateMatrixQua(moveQua_.MakeRotateMatrix());
}

#pragma region
void Player::BehaviorRootInit()
{
}

void Player::BehaviorRootUpdate()
{
	Move();
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
}
void Player::GrapUpdate()
{
}
void Player::GrapJumpLeftInitalize()
{
}
void Player::GrapJumpLeftUpdate()
{
}
void Player::GrapJumpRightInitalize()
{
}
void Player::GrapJumpRightUpdate()
{
}
#pragma endregion

void Player::Move()
{
	//移動量
	if (input->GetMoveXZ().x == 0 && input->GetMoveXZ().z == 0) {
		return;
	}

		move = input->GetMoveXZ();
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
		move.y = 0.0f;
		Vector3 cross = Cross({ 0.0f,0.0f,1.0f }, move);
		cross = cross.Normalize();
		float dot = Dot({ 0.0f,0.0f,1.0f }, move);
		moveQua_ = MakwRotateAxisAngleQuaternion(cross, std::acos(dot));

}