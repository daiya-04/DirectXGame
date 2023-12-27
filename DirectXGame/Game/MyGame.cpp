#include "MyGame.h"

#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "SceneFactory.h"

void MyGame::Init(){

	DSFramework::Init();

	SceneManager::GetInstance()->ChangeScene("Title");

	
}

void MyGame::Update(){

	DSFramework::Update();

	///ゲーム固有処理
	
	
	
	///

	ImGuiManager::GetInstance()->End();
}

void MyGame::Draw(){

	DirectXCommon::GetInstance()->preDraw();

	///ゲーム固有処理

	SceneManager::GetInstance()->Draw(DirectXCommon::GetInstance()->GetCommandList());

	///

	ImGuiManager::GetInstance()->Draw();

	DirectXCommon::GetInstance()->postDraw();

}

void MyGame::Finalize(){



	DSFramework::Finalize();
}
