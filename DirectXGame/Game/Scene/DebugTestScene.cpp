#include "DebugTestScene.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "AnimationManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "Line.h"
#include "ShapesDraw.h"
#include "ParticleEditor.h"
#include "ParticleManager.h"

void DebugTestScene::Init() {

	camera_.Init();
	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	humanModel_ = ModelManager::LoadGLTF("Skin");
	standingModel_ = ModelManager::LoadGLTF("Standing");
	sneakModel_ = ModelManager::LoadGLTF("BossDead");
	model_ = ModelManager::LoadOBJ("Rock");

	skyBoxTex_ = TextureManager::Load("skyBox.dds");
	tex_ = TextureManager::Load("test.png");
	burnScarsTex_ = TextureManager::Load("ScarBase.png");

	skyBox_.reset(SkyBox::Create(skyBoxTex_));

	human_.reset(SkinningObject::Create(humanModel_));
	animation_ = AnimationManager::Load(standingModel_->name_);
	skeleton_ = Skeleton::Create(standingModel_->rootNode_);
	skinCluster_.Create(skeleton_, standingModel_);
	human_->SetSkinCluster(&skinCluster_);

	obj_.reset(Object3d::Create(model_));
	obj_->worldTransform_.rotation_.y = 3.14f;

	sprite_.reset(Sprite::Create(tex_, { 640.0f,360.0f }));
	
	dissolve_ = Dissolve::GetInstance();
	dissolve_->Init();

	postEffect_ = PostEffect::GetInstance();
	postEffect_->Init();

	burnScars_.reset(BurnScar::Create(burnScarsTex_));
	burnScars_->EffectStart(Vector3());

	iceScar_.reset(IceScar::Create(burnScarsTex_));
	iceScar_->EffectStart(Vector3());

	human_->worldTransform_.rotation_.y = 3.14f;
	human_->worldTransform_.translation_.z = 10.0f;

	particle2_.reset(GPUParticle::Create(TextureManager::Load("star.png"), 10000));
	particle2_->SetParticleData(ParticleManager::Load("testParticle"));

	/*particle2_->particleData_.isLoop_ = true;
	particle2_->particleData_.emitter_.translate = Vector3(0.0f, 0.0f, 0.0f);
	particle2_->particleData_.emitter_.size = Vector3(1.0f, 1.0f, 1.0f);
	particle2_->particleData_.emitter_.radius = 1.0f;
	particle2_->particleData_.emitter_.scale = 0.1f;
	particle2_->particleData_.emitter_.count = 1;
	particle2_->particleData_.emitter_.frequency = 1.0f / 60.0f;
	particle2_->particleData_.emitter_.color = Vector4(0.89f, 0.27f, 0.03f, 1.0f);
	particle2_->particleData_.emitter_.lifeTime = 1.0f;
	particle2_->particleData_.emitter_.speed = 0.0f;
	particle2_->particleData_.emitter_.emitterType = GPUParticle::EmitShape::Sphere;*/

	particle_.reset(GPUParticle::Create(TextureManager::Load("circle.png"),10000));
	particle_->particleData_.emitter_.translate = Vector3(0.0f, 0.0f, 0.0f);
	particle_->particleData_.emitter_.size = Vector3(0.0f,0.0f,0.0f);
	particle_->particleData_.emitter_.radius = 1.0f;
	particle_->particleData_.emitter_.scale = 0.1f;
	particle_->particleData_.emitter_.count = 1000;
	particle_->particleData_.emitter_.frequency = 1.0f / 60.0f;
	particle_->particleData_.emitter_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	particle_->particleData_.emitter_.lifeTime = 1.0f;
	particle_->particleData_.emitter_.speed = 1.0f;
	particle_->particleData_.emitter_.emitterType = GPUParticle::EmitShape::Circle;

	particle_->particleData_.isLoop_ = false;

	ParticleEditor::GetInstance()->Init();


	Update();
}

void DebugTestScene::Update() {
	DebugGUI();
	dissolve_->DebugGUI();

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	
#endif // _DEBUG
	
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		human_->SetModelHandle(sneakModel_);
		animation_ = AnimationManager::Load(sneakModel_->name_);
		skeleton_ = Skeleton::Create(sneakModel_->rootNode_);
		skinCluster_.Create(skeleton_, sneakModel_);
		animation_.SetAnimationSpeed(1.0f / 30.0f);
	}
	else if(Input::GetInstance()->ReleaseKey(DIK_SPACE)){
		//human_->SetModelHandle(humanModel_);
		animation_ = AnimationManager::Load(standingModel_->name_);
		skeleton_ = Skeleton::Create(standingModel_->rootNode_);
		skinCluster_.Create(skeleton_, standingModel_);
		animation_.SetAnimationSpeed(1.0f / 60.0f);
	}

	if (Input::GetInstance()->TriggerKey(DIK_H)) {
		particle_->particleData_.emitter_.emit = 1;
	}

	particle_->Update();
	particle2_->Update();

	ParticleEditor::GetInstance()->Update();

	human_->worldTransform_.UpdateMatrix();
	animation_.Play(skeleton_);
	
	skeleton_.Update();
	skinCluster_.Update(skeleton_);

	obj_->worldTransform_.UpdateMatrix();

	camera_.UpdateViewMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void DebugTestScene::DrawBackGround() {



}

void DebugTestScene::DrawModel() {

	SkinningObject::preDraw();
	//human_->Draw(camera_);
	//skeleton_.Draw(human_->worldTransform_, camera_);


	Object3d::preDraw();
	//obj_->Draw(camera_);

	//skyBox_->Draw(camera_);
	//ShapesDraw::DrawSphere(Shapes::Sphere({}, 1.0f), camera_);
	//ShapesDraw::DrawPlane(Shapes::Plane({ 0.0f,0.0f,1.0f }, 10.0f), camera_);
	//ShapesDraw::DrawAABB(Shapes::AABB({ -1.0,-1.0,-1.0f }, { 1.0f,1.0f,1.0f }), camera_);

	BurnScar::preDraw();
	//burnScars_->Draw(camera_);

	IceScar::preDraw();
	//iceScar_->Draw(camera_);

}

void DebugTestScene::DrawParticleModel() {



}

void DebugTestScene::DrawParticle() {

	GPUParticle::preDraw();
	//particle_->Draw(camera_);
	//particle2_->Draw(camera_);
	ParticleEditor::GetInstance()->Draw(camera_);

}

void DebugTestScene::DrawUI() {



}

void DebugTestScene::DrawPostEffect() {
	/*dissolve_->PreDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	Sprite::preDraw(DirectXCommon::GetInstance()->GetCommandList());
	sprite_->Draw();

	dissolve_->PostDrawScene(DirectXCommon::GetInstance()->GetCommandList());*/

}

void DebugTestScene::DrawRenderTexture() {

	
	//postEffect_->Draw(DirectXCommon::GetInstance()->GetCommandList());
	//dissolve_->Draw(DirectXCommon::GetInstance()->GetCommandList());

}

void DebugTestScene::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("camera");

	ImGui::DragFloat3("pos", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.01f);

	ImGui::End();

	ImGui::Begin("object");

	ImGui::DragFloat3("rotate", &human_->worldTransform_.rotation_.x, 0.01f);

	ImGui::End();

	//ImGui::Begin("Particle");

	//if(ImGui::TreeNode("Particle1")){
	//	ImGui::InputFloat("speed", &particle2_->emitter_.speed);
	//	ImGui::InputFloat("Scale", &particle2_->emitter_.scale);
	//	ImGui::InputFloat("rotate", &particle2_->emitter_.rotate);
	//	ImGui::InputInt("count", reinterpret_cast<int*>(&particle2_->emitter_.count));
	//	ImGui::InputFloat("radius", &particle2_->emitter_.radius);
	//	ImGui::InputFloat("LifeTime", &particle2_->emitter_.lifeTime);
	//	ImGui::InputFloat4("Color", &particle2_->emitter_.color.x);
	//	bool isCheck = (particle2_->emitter_.isBillboard != 0);
	//	if (ImGui::Checkbox("isBillboard", &isCheck)) {
	//		particle2_->emitter_.isBillboard = static_cast<uint32_t>(isCheck);
	//	}
	//	
	//	int currentEmitShape = static_cast<int>(particle2_->emitter_.emitterType);

	//	if (ImGui::RadioButton("Sphere", currentEmitShape == GPUParticle::EmitShape::Sphere)) {
	//		currentEmitShape = GPUParticle::EmitShape::Sphere;
	//	}
	//	if (ImGui::RadioButton("Hemisphere", currentEmitShape == GPUParticle::EmitShape::Hemisphere)) {
	//		currentEmitShape = GPUParticle::EmitShape::Hemisphere;
	//	}
	//	if (ImGui::RadioButton("Box", currentEmitShape == GPUParticle::EmitShape::Box)) {
	//		currentEmitShape = GPUParticle::EmitShape::Box;
	//	}
	//	if (ImGui::RadioButton("Square", currentEmitShape == GPUParticle::EmitShape::Squere)) {
	//		currentEmitShape = GPUParticle::EmitShape::Squere;
	//	}
	//	if (ImGui::RadioButton("Circle", currentEmitShape == GPUParticle::EmitShape::Circle)) {
	//		currentEmitShape = GPUParticle::EmitShape::Circle;
	//	}

	//	// 選択された形状を反映
	//	particle2_->emitter_.emitterType = static_cast<GPUParticle::EmitShape>(currentEmitShape);



	//	isCheck = (particle2_->overLifeTime_.isConstantVelocity != 0);
	//	if (ImGui::Checkbox("isConstantVelocity", &isCheck)) {
	//		particle2_->overLifeTime_.isConstantVelocity = static_cast<uint32_t>(isCheck);
	//	}
	//	if (particle2_->overLifeTime_.isConstantVelocity) {
	//		if (ImGui::TreeNode("ConstantVelocity")) {
	//			ImGui::InputFloat3("velocity", &particle2_->overLifeTime_.velocity.x);
	//			ImGui::TreePop();
	//		}
	//	}

	//	isCheck = (particle2_->overLifeTime_.isTransVelocity != 0);
	//	if (ImGui::Checkbox("isTransVelocity", &isCheck)) {
	//		particle2_->overLifeTime_.isTransVelocity = static_cast<uint32_t>(isCheck);
	//	}
	//	if (particle2_->overLifeTime_.isTransVelocity) {
	//		if (ImGui::TreeNode("TransVelocity")) {
	//			ImGui::InputFloat3("startVelocity", &particle2_->overLifeTime_.startVelocity.x);
	//			ImGui::InputFloat3("endVelocity", &particle2_->overLifeTime_.endVelocity.x);
	//			ImGui::TreePop();
	//		}
	//	}

	//	isCheck = (particle2_->overLifeTime_.isScale != 0);
	//	if (ImGui::Checkbox("isScale", &isCheck)) {
	//		particle2_->overLifeTime_.isScale = static_cast<uint32_t>(isCheck);
	//	}
	//	if (particle2_->overLifeTime_.isScale) {
	//		if (ImGui::TreeNode("ScaleOverLifeTime")) {
	//			ImGui::InputFloat("startScale", &particle2_->overLifeTime_.startScale);
	//			ImGui::InputFloat("endScale", &particle2_->overLifeTime_.endScale);
	//			ImGui::TreePop();
	//		}
	//	}

	//	isCheck = (particle2_->overLifeTime_.isColor != 0);
	//	if (ImGui::Checkbox("isColor", &isCheck)) {
	//		particle2_->overLifeTime_.isColor = static_cast<uint32_t>(isCheck);
	//	}
	//	if (particle2_->overLifeTime_.isColor) {
	//		if (ImGui::TreeNode("ColorOverLifeTime")) {
	//			ImGui::InputFloat3("startColor", &particle2_->overLifeTime_.startColor.x);
	//			ImGui::InputFloat3("endColor", &particle2_->overLifeTime_.endColor.x);
	//			ImGui::TreePop();
	//		}
	//	}

	//	isCheck = (particle2_->overLifeTime_.isAlpha != 0);
	//	if (ImGui::Checkbox("isAlpha", &isCheck)) {
	//		particle2_->overLifeTime_.isAlpha = static_cast<uint32_t>(isCheck);
	//	}
	//	if (particle2_->overLifeTime_.isAlpha) {
	//		if (ImGui::TreeNode("AlphaOverLifeTime")) {
	//			ImGui::SliderFloat("startAlpha", &particle2_->overLifeTime_.startAlpha, 0.0f, 1.0f);
	//			ImGui::SliderFloat("midAlpha", &particle2_->overLifeTime_.midAlpha, 0.0f, 1.0f);
	//			ImGui::SliderFloat("endAlpha", &particle2_->overLifeTime_.endAlpha, 0.0f, 1.0f);
	//			ImGui::TreePop();
	//		}
	//	}

	//	isCheck = (particle2_->overLifeTime_.isTransSpeed != 0);
	//	if (ImGui::Checkbox("isTransSpeed", &isCheck)) {
	//		particle2_->overLifeTime_.isTransSpeed = static_cast<uint32_t>(isCheck);
	//	}
	//	if (particle2_->overLifeTime_.isTransSpeed) {
	//		if (ImGui::TreeNode("SpeedOverLifeTime")) {
	//			ImGui::InputFloat("statrSpeed", &particle2_->overLifeTime_.startSpeed);
	//			ImGui::InputFloat("endSpeed", &particle2_->overLifeTime_.endSpeed);
	//			ImGui::TreePop();
	//		}
	//	}

	//	ImGui::InputFloat("gravity", &particle2_->overLifeTime_.gravity);

	//	ImGui::TreePop();
	//}

	/*if (ImGui::TreeNode("Particle2")) {

		ImGui::InputFloat("speed", &particle_->emitter_.speed);
		ImGui::InputInt("count", reinterpret_cast<int*>(&particle_->emitter_.count));

		bool isCheck = (particle_->overLifeTime_.isConstantVelocity != 0);
		if (ImGui::Checkbox("isConstantVelocity", &isCheck)) {
			particle_->overLifeTime_.isConstantVelocity = static_cast<uint32_t>(isCheck);
		}
		if (particle_->overLifeTime_.isConstantVelocity) {
			if (ImGui::TreeNode("ConstantVelocity")) {
				ImGui::InputFloat3("velocity", &particle_->overLifeTime_.velocity.x);
				ImGui::TreePop();
			}
		}

		isCheck = (particle_->overLifeTime_.isTransVelocity != 0);
		if (ImGui::Checkbox("isTransVelocity", &isCheck)) {
			particle_->overLifeTime_.isTransVelocity = static_cast<uint32_t>(isCheck);
		}
		if (particle_->overLifeTime_.isTransVelocity) {
			if (ImGui::TreeNode("TransVelocity")) {
				ImGui::InputFloat3("startVelocity", &particle_->overLifeTime_.startVelocity.x);
				ImGui::InputFloat3("endVelocity", &particle_->overLifeTime_.endVelocity.x);
				ImGui::TreePop();
			}
		}

		isCheck = (particle_->overLifeTime_.isScale != 0);
		if (ImGui::Checkbox("isScale", &isCheck)) {
			particle_->overLifeTime_.isScale = static_cast<uint32_t>(isCheck);
		}
		if (particle_->overLifeTime_.isScale) {
			if (ImGui::TreeNode("ScaleOverLifeTime")) {
				ImGui::InputFloat("startScale", &particle_->overLifeTime_.startScale);
				ImGui::InputFloat("endScale", &particle_->overLifeTime_.endScale);
				ImGui::TreePop();
			}
		}

		isCheck = (particle_->overLifeTime_.isColor != 0);
		if (ImGui::Checkbox("isColor", &isCheck)) {
			particle_->overLifeTime_.isColor = static_cast<uint32_t>(isCheck);
		}
		if (particle_->overLifeTime_.isColor) {
			if (ImGui::TreeNode("ColorOverLifeTime")) {
				ImGui::InputFloat3("startColor", &particle_->overLifeTime_.startColor.x);
				ImGui::InputFloat3("endColor", &particle_->overLifeTime_.endColor.x);
				ImGui::TreePop();
			}
		}

		isCheck = (particle_->overLifeTime_.isAlpha != 0);
		if (ImGui::Checkbox("isAlpha", &isCheck)) {
			particle_->overLifeTime_.isAlpha = static_cast<uint32_t>(isCheck);
		}
		if (particle_->overLifeTime_.isAlpha) {
			if (ImGui::TreeNode("AlphaOverLifeTime")) {
				ImGui::SliderFloat("startAlpha", &particle_->overLifeTime_.startAlpha, 0.0f, 1.0f);
				ImGui::SliderFloat("midAlpha", &particle_->overLifeTime_.midAlpha, 0.0f, 1.0f);
				ImGui::SliderFloat("endAlpha", &particle_->overLifeTime_.endAlpha, 0.0f, 1.0f);
				ImGui::TreePop();
			}
		}

		isCheck = (particle_->overLifeTime_.isTransSpeed != 0);
		if (ImGui::Checkbox("isTransSpeed", &isCheck)) {
			particle_->overLifeTime_.isTransSpeed = static_cast<uint32_t>(isCheck);
		}
		if (particle_->overLifeTime_.isTransSpeed) {
			if (ImGui::TreeNode("SpeedOverLifeTime")) {
				ImGui::InputFloat("statrSpeed", &particle_->overLifeTime_.startSpeed);
				ImGui::InputFloat("endSpeed", &particle_->overLifeTime_.endSpeed);
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}*/

	

	//ImGui::End();



#endif // _DEBUG
}
