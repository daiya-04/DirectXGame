#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {}

void GameScene::Init(){

	viewProjection_.Initialize();

	
	
}

void GameScene::Update(){
	DebugGUI();

	rotation0 = MakwRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	rotation1 = MakwRotateAxisAngleQuaternion({ 0.71f,0.0f,0.71f }, 3.141592f);

	interpolate0 = Slerp(rotation0, rotation1, 0.0f);
	interpolate1 = Slerp(rotation0, rotation1, 0.3f);
	interpolate2 = Slerp(rotation0, rotation1, 0.5f);
	interpolate3 = Slerp(rotation0, rotation1, 0.7f);
	interpolate4 = Slerp(rotation0, rotation1, 1.0f);
	
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

	ImGui::Begin("result");

	Quaternion result = interpolate0;

	ImGui::Text("%.02f %.02f %.02f %.02f  : rotation", result.x, result.y, result.z, result.w);
	ImGui::NewLine();

	result = interpolate1;

	ImGui::Text("%.02f %.02f %.02f %.02f  : rotation", result.x, result.y, result.z, result.w);
	ImGui::NewLine();

	result = interpolate2;

	ImGui::Text("%.02f %.02f %.02f %.02f  : rotation", result.x, result.y, result.z, result.w);
	ImGui::NewLine();

	result = interpolate3;

	ImGui::Text("%.02f %.02f %.02f %.02f  : rotation", result.x, result.y, result.z, result.w);
	ImGui::NewLine();

	result = interpolate4;

	ImGui::Text("%.02f %.02f %.02f %.02f  : rotation", result.x, result.y, result.z, result.w);
	ImGui::NewLine();


	ImGui::End();

#endif // _DEBUG
}


