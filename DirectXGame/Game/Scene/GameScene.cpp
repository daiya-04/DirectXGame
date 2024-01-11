#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();

	
	
}

void GameScene::Update(){
	DebugGUI();



	camera_.UpdateViewMatrix();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){


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


