#include "MyGame.h"

#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"

void MyGame::Init(){

	DSFramework::Init();

	

}

void MyGame::Update(){

	DSFramework::Update();

	///ゲーム固有処理


	
	///

	ImGuiManager::GetInstance()->End();
}

void MyGame::Draw(){

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	dxCommon->preDraw();

	///ここはシーンマネージャへ

	///背景スプライト
	Sprite::preDraw(dxCommon->GetCommandList());



	Sprite::postDraw();

	///3dオブジェクト
	Object3d::preDraw();



	Object3d::postDraw();

	///パーティクル
	Particle::preDraw();



	Particle::postDraw();

	///UI
	Sprite::preDraw(dxCommon->GetCommandList());



	Sprite::postDraw();

	///

	ImGuiManager::GetInstance()->Draw();

	dxCommon->postDraw();

}

void MyGame::Finalize(){



	DSFramework::Finalize();
}
