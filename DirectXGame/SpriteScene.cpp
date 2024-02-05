#include "SpriteScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include <random>
#include <numbers>

SpriteScene::~SpriteScene(){}

void SpriteScene::Init(){

	uint32_t uvTex = TextureManager::Load("uvChecker.png");
	uint32_t ballTex = TextureManager::Load("monsterBall.png");
	uint32_t white = TextureManager::Load("white.png");

	check_.reset(new Sprite(uvTex, {200.0f,200.0f}, { 100.0f,100.0f }));
	check_->Initialize();
	checkPos_ = check_->GetPosition();
	checkSize_ = check_->GetSize();

	ball_.reset(new Sprite(ballTex, {640.0f,360.0f}, { 100.0f,100.0f }));
	ball_->Initialize();
	ballPos_ = ball_->GetPosition();
	ballSize_ = ball_->GetSize();

	solid_.reset(new Sprite(white, {800.0f,400.0f}, { 100.0f,100.0f }));
	solid_->Initialize();
	solidPos_ = solid_->GetPosition();
	solidSize_ = solid_->GetSize();

}

void SpriteScene::Update(){
	DebugGUI();

	if (Input::GetInstance()->TriggerKey(DIK_K)) {
		SceneManager::GetInstance()->ChangeScene("Model");
	}
	
	rotate_ += 0.01f;
	rotate_ = std::fmodf(rotate_, 2.0f * std::numbers::pi_v<float>);

	check_->SetRotate(rotate_);
}

void SpriteScene::DrawBackGround(){



}

void SpriteScene::DrawModel(){



}

void SpriteScene::DrawParticleModel(){



}

void SpriteScene::DrawParticle(){



}

void SpriteScene::DrawUI(){

	check_->Draw();
	ball_->Draw();
	solid_->Draw();

}

void SpriteScene::DebugGUI(){
#ifdef _DEBUG

	ImGui::Begin("Sprite");

	if (ImGui::TreeNode("Check")) {

		ImGui::DragFloat2("position", &checkPos_.x, 0.1f);
		ImGui::DragFloat2("size", &checkSize_.x, 0.1f);

		check_->SetPosition(checkPos_);
		check_->SetSize(checkSize_);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("MonsterBall")) {

		ImGui::DragFloat2("position", &ballPos_.x, 0.1f);
		ImGui::DragFloat2("size", &ballSize_.x, 0.1f);

		ball_->SetPosition(ballPos_);
		ball_->SetSize(ballSize_);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Solid")) {

		ImGui::DragFloat2("position", &solidPos_.x, 0.1f);
		ImGui::DragFloat2("size", &solidSize_.x, 0.1f);

		solid_->SetPosition(solidPos_);
		solid_->SetSize(solidSize_);

		ImGui::TreePop();
	}


	ImGui::End();

#endif // _DEBUG
}
