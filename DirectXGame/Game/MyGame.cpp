#include "MyGame.h"

#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "SceneFactory.h"

void MyGame::Init(){

	DSFramework::Init();

	SceneManager::GetInstance()->ChangeScene("Title");

	TextureManager::Load("white.png");

	/*postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Init();*/

}

void MyGame::Update(){

	DSFramework::Update();

	///ゲーム固有処理

	
	
	///

	ImGuiManager::GetInstance()->End();
}

void MyGame::Draw(){

	//レンダーテクスチャへの描画
	//postEffect_->PreDrawScene(DirectXCommon::GetInstance()->GetCommandList());
	SceneManager::GetInstance()->Draw(DirectXCommon::GetInstance()->GetCommandList());
	//postEffect_->PostDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	//DirectXCommon::GetInstance()->preDraw();

	///ゲーム固有処理

	//SceneManager::GetInstance()->Draw(DirectXCommon::GetInstance()->GetCommandList());
	//postEffect_->Draw(DirectXCommon::GetInstance()->GetCommandList());

	///

	//ImGuiManager::GetInstance()->Draw();

	//DirectXCommon::GetInstance()->postDraw();

}

void MyGame::Finalize(){



	DSFramework::Finalize();
}
