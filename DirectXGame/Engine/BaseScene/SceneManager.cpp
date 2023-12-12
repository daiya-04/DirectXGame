#include "SceneManager.h"
#include "SceneFactory.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include <cassert>

SceneManager* SceneManager::GetInstance(){
	static SceneManager instance;

	return &instance;
}

void SceneManager::Init() {

	sceneFactory_ = std::make_unique<SceneFactory>();

}

void SceneManager::Update(){

	if (nextScene_) {
		scene_ = std::move(nextScene_);
		scene_->Init();
	}


	scene_->Update();

}

void SceneManager::Draw(ID3D12GraphicsCommandList* commandList){

	///背景スプライト
	Sprite::preDraw(commandList);

	scene_->DrawBackGround();

	Sprite::postDraw();

	///3dオブジェクト
	Object3d::preDraw();

	scene_->DrawModel();

	Object3d::postDraw();

	///パーティクル
	Particle::preDraw();

	scene_->DrawParticle();

	Particle::postDraw();

	///UI
	Sprite::preDraw(commandList);

	scene_->DrawUI();

	Sprite::postDraw();

}

void SceneManager::ChangeScene(const std::string& sceneName){

	assert(nextScene_ == nullptr);

	nextScene_ = sceneFactory_->CreateScene(sceneName);
}


