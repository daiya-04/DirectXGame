#include "TitleScene.h"

#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

TitleScene::~TitleScene(){

}

void TitleScene::Init() {
	

	uint32_t circle = TextureManager::Load("circle.png");
	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 50));

	emitter_.count_ = 3;
	emitter_.frequency_ = 0.5f;

	accelerationField_.acceleration_ = { 5.0f,0.0f,0.0f };
	accelerationField_.area_.min = { -1.0f,-1.0f,-1.0f };
	accelerationField_.area_.max = { 1.0f,1.0f,1.0f };

	input_ = Input::GetInstance();
}

void TitleScene::Update() {
	DebugGUI();

	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	emitter_.frequencyTime_ += kDeltaTime;
	if (emitter_.frequency_ <= emitter_.frequencyTime_) {
		particles_.splice(particles_.end(), Particle::Emit(emitter_, randomEngine));
		emitter_.frequencyTime_ -= emitter_.frequency_;
	}
	for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
		if (isField_) {
			if (IsCollision(accelerationField_.area_, (*itParticle).worldTransform_.translation_)) {
				(*itParticle).velocity_ += accelerationField_.acceleration_ * kDeltaTime;
			}
		}
		(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_ * kDeltaTime;
		(*itParticle).currentTime_ += kDeltaTime;
	}

	if (input_->TriggerKey(DIK_RETURN)|| input_->TriggerButton(XINPUT_GAMEPAD_A)) {
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
}

void TitleScene::DrawBackGround() {

}

void TitleScene::DrawModel() {

}

void TitleScene::DrawParticleModel() {

}

void TitleScene::DrawParticle() {
	
}

void TitleScene::DrawUI() {

}

void TitleScene::DebugGUI() {
#ifdef _DEBUG

	ImGui::Begin("Title");

	ImGui::Checkbox("FieldEffect", &isField_);

	ImGui::End();

#endif // _DEBUG

}
