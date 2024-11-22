#include "MyGame.h"

#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "SceneFactory.h"
#include "BurnScar.h"
#include "IceScar.h"

void MyGame::Init(){

	DSFramework::Init();

	BurnScar::StaticInit(DirectXCommon::GetInstance()->GetDevice(), DirectXCommon::GetInstance()->GetCommandList());
	IceScar::StaticInit(DirectXCommon::GetInstance()->GetDevice(), DirectXCommon::GetInstance()->GetCommandList());

#ifdef NDEBUG
	SceneManager::GetInstance()->ChangeScene("Title");
#endif // NDEBUG
	

#ifdef _DEBUG

	SceneManager::GetInstance()->ChangeScene("Debug");

#endif // _DEBUG

}

void MyGame::Update(){

	DSFramework::Update();

	///ゲーム固有処理

	
	
	///

	ImGuiManager::GetInstance()->End();
}

void MyGame::Draw(){

	SceneManager::GetInstance()->Draw(DirectXCommon::GetInstance()->GetCommandList());

}

void MyGame::Finalize(){

	DSFramework::Finalize();
}
