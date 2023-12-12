#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {}

void GameScene::Init(){

	viewProjection_.Initialize();

	uint32_t circle = TextureManager::Load("circle.png");
	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 50));

	emitter_.count_ = 3;
	emitter_.frequency_ = 0.5f;

	accelerationField_.acceleration_ = { 5.0f,0.0f,0.0f };
	accelerationField_.area_.min = { -1.0f,-1.0f,-1.0f };
	accelerationField_.area_.max = { 1.0f,1.0f,1.0f };
	
}

void GameScene::Update(){
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
	
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){

	

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	particle_->Draw(particles_, viewProjection_);

}

void GameScene::DrawUI(){



}

void GameScene::DebugGUI(){
#ifdef _DEBUG

	ImGui::Begin("teapot");

	ImGui::Checkbox("FieldEffect", &isField_);

	ImGui::End();

#endif // _DEBUG
}


