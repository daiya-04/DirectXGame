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
	//エンジン層の初期化
	DSFramework::Init();
	//ゲーム固有のパイプラインの生成
	BurnScar::StaticInit(DirectXCommon::GetInstance()->GetDevice(), DirectXCommon::GetInstance()->GetCommandList());
	IceScar::StaticInit(DirectXCommon::GetInstance()->GetDevice(), DirectXCommon::GetInstance()->GetCommandList());

}

void MyGame::Update(){
	//エンジン層の更新
	DSFramework::Update();

	///ゲーム固有処理

	
	
	///
	//ImGuiの終了
	ImGuiManager::GetInstance()->End();
}

void MyGame::Draw(){
	//描画
	SceneManager::GetInstance()->Draw(DirectXCommon::GetInstance()->GetCommandList());

}

void MyGame::Finalize(){
	//エンジン層の解放
	DSFramework::Finalize();
}
