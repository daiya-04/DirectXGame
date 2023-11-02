#include "Player.h"

#include "../../../ImGuiManager.h"

void Player::Initialize()
{
	transformBase_.rotation_ = { 0.0f,0.0f,0.0f };
	transformBase_.translation_ = { 0.0f,0.0f,0.0f };

	WorldTransform wt;
	wt.rotation_ = { 0.0f,0.0f,0.0f };
	wt.translation_ = { 0.0f,0.0f,0.0f };
	wt.parent_ = &transformBase_;
	transforms_.push_back(wt);
	models_.emplace_back(Object3d::Create("teapot"));
}

void Player::Update()
{
#ifdef _DEBUG

	ImGui::Begin("Player");

	ImGui::DragFloat3("Translate", &transformBase_.translation_.x, 0.1f);


	ImGui::End();

#endif // _DEBUG




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
