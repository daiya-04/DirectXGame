#include "SceneManager.h"
#include "SceneFactory.h"
#include "DirectXCommon.h"
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

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Init();

	fadeTex_ = TextureManager::Load("Black.png");

	fade_.reset(new Sprite(fadeTex_, { 640.0f,360.0f }, { 1280.0f, 720.0f }));
	fade_->Initialize();
	fade_->SetColor({ 1.0f,1.0f,1.0f,alpha_ });

}

void SceneManager::Update(){

	if (nextScene_) {
		if (scene_) {
			alpha_ += 0.01f;
		}else {
			scene_ = std::move(nextScene_);
			scene_->Init();
		}

		if (alpha_ >= 1.0f) {
			scene_ = std::move(nextScene_);
			scene_->Init();
		}
	}else {
		alpha_ -= 0.01f;
	}

	if (alpha_ <= 0.0f) { alpha_ = 0.0f; }

	if (!nextScene_ && alpha_ <= 0.0f) {
		scene_->Update();
	}
  
  fade_->SetColor({ 1.0f,1.0f,1.0f,alpha_ });

#ifdef _DEBUG

	ImGui::Begin("window");

	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::End();

#endif // _DEBUG

}

void SceneManager::Draw(ID3D12GraphicsCommandList* commandList){

	postEffect_->PreDrawScene(commandList);

	

	///パーティクル
	Particle::preDraw();

	scene_->DrawParticle();

	Particle::postDraw();

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

	postEffect_->Draw(commandList);

	///UI
	Sprite::preDraw(commandList);

	scene_->DrawUI();
	fade_->Draw();

	Sprite::postDraw();

	ImGuiManager::GetInstance()->Draw();

	DirectXCommon::GetInstance()->postDraw();
	

}

void SceneManager::ChangeScene(const std::string& sceneName){

	assert(nextScene_ == nullptr);

	nextScene_ = sceneFactory_->CreateScene(sceneName);
}


