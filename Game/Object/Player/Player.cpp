#include "Player.h"

void Player::Initialize()
{
	transformBase_.rotation_ = { 0.0f,0.0f,0.0f };
	transformBase_.translation_ = { 0.0f,0.0f,0.0f };

	WorldTransform wt;
	wt.rotation_ = { 0.0f,0.0f,0.0f };
	wt.translation_ = { 0.0f,0.0f,0.0f };
	transforms_.push_back(wt);
	models_.emplace_back(Object3d::Create("teapot"));
}

void Player::Update()
{
	if (input_->PushKey(DIK_A)) {
		transformBase_.translation_.x -= 0.1f;
	}
	if (input_->PushKey(DIK_D)) {
		transformBase_.translation_.x += 0.1f;
	}
	if (input_->PushKey(DIK_W)) {
		transformBase_.translation_.z += 0.1f;
	}
	if (input_->PushKey(DIK_S)) {
		transformBase_.translation_.z -= 0.1f;
	}
	
	UpdateMatrixs();
}

void Player::Draw()
{
	DrawAllModel();
}
