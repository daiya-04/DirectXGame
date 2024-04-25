#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();
	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	cubeModel_ = ModelManager::LoadGLTF("AnimatedCube");

	cube_.reset(Object3d::Create(cubeModel_, true));
	
}

void GameScene::Update(){
	DebugGUI();

	cube_->Update();
	camera_.UpdateViewMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){

	cube_->Draw(camera_);

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	

}

void GameScene::DrawUI(){

	

}

void GameScene::DebugGUI(){
#ifdef _DEBUG

	ImGui::Begin("camera");

	ImGui::DragFloat3("pos", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.01f);

	ImGui::End();

#endif // _DEBUG
}


