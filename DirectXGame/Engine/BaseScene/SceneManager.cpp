#include "SceneManager.h"
#include "SceneFactory.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <cassert>

SceneManager* SceneManager::GetInstance(){
	static SceneManager instance;

	return &instance;
}

void SceneManager::Init() {

	sceneFactory_ = std::make_unique<SceneFactory>();

	//uint32_t whiteTex = TextureManager::Load("white.png");

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Init();

}

void SceneManager::Update(){

	if (nextScene_) {
		scene_ = std::move(nextScene_);
		scene_->Init();
	}


	scene_->Update();

#ifdef _DEBUG

	ImGui::Begin("window");

	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::End();

#endif // _DEBUG

}

void SceneManager::Draw(ID3D12GraphicsCommandList* commandList){

	postEffect_->PreDrawScene(commandList);

	

	postEffect_->PostDrawScene(commandList);

	DirectXCommon::GetInstance()->preDraw();

	///背景スプライト
	Sprite::preDraw(commandList);

	scene_->DrawBackGround();

	Sprite::postDraw();

	DirectXCommon::GetInstance()->ClearDepthBaffer();

	///3dオブジェクト
	Object3d::preDraw();

	scene_->DrawModel();

	Object3d::postDraw();

	///パーティクル
	Particle::preDraw();

	scene_->DrawParticle();

	Particle::postDraw();

	postEffect_->Draw(commandList);

	///UI
	Sprite::preDraw(commandList);

	scene_->DrawUI();

	Sprite::postDraw();

	ImGuiManager::GetInstance()->Draw();

	DirectXCommon::GetInstance()->postDraw();
	

}

void SceneManager::ChangeScene(const std::string& sceneName){

	assert(nextScene_ == nullptr);

	nextScene_ = sceneFactory_->CreateScene(sceneName);
}


