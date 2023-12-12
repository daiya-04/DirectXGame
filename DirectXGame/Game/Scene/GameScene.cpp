#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {}

void GameScene::Init(){

	viewProjection_.Initialize();

	backGroundHandle_ = TextureManager::Load("white.png");
	backGround_.reset(new Sprite(backGroundHandle_, { 0.0f,0.0f }, { 1280.0f,720.0f }));
	backGround_->Initialize();
	backGround_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	uint32_t teapotModel = ModelManager::Load("teapot");

	teapot_ = std::make_unique<Object3d>();
	teapot_.reset(Object3d::Create(teapotModel));

}

void GameScene::Update(){
	DebugGUI();

	worldTransform_.UpdateMatrix();
}

void GameScene::DrawBackGround(){

	backGround_->Draw();

}

void GameScene::DrawModel(){

	teapot_->Draw(worldTransform_, viewProjection_);

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){



}

void GameScene::DrawUI(){



}

void GameScene::DebugGUI(){
#ifdef _DEBUG

	ImGui::Begin("teapot");

	ImGui::DragFloat3("transform", &worldTransform_.translation_.x, 0.01f);

	ImGui::End();

#endif // _DEBUG
}


