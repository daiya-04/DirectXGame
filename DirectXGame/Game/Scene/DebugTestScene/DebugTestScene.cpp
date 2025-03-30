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
	///必須処理
	//カメラ初期化
	camera_.Init();
	camera_.SetFOV(45.0f);
	//ライト初期化
	pointLight_.Init();
	spotLight_.Init();
	//Object3dにライトセット
	DaiEngine::Object3d::SetPointLight(&pointLight_);
	DaiEngine::Object3d::SetSpotLight(&spotLight_);

	///

	humanModel_ = DaiEngine::ModelManager::LoadGLTF("Skin");
	standingModel_ = DaiEngine::ModelManager::LoadGLTF("Standing");
	sneakModel_ = DaiEngine::ModelManager::LoadGLTF("Running");
	model_ = DaiEngine::ModelManager::LoadOBJ("Rock");

	skyBoxTex_ = DaiEngine::TextureManager::Load("skyBox.dds");
	tex_ = DaiEngine::TextureManager::Load("test.png");
	burnScarsTex_ = DaiEngine::TextureManager::Load("BurnScars.png");

	skyBox_.reset(DaiEngine::SkyBox::Create(skyBoxTex_));

	human_.reset(DaiEngine::SkinningObject::Create(humanModel_));
	animation_ = DaiEngine::AnimationManager::Load(standingModel_->name_);
	skeleton_ = DaiEngine::Skeleton::Create(standingModel_->rootNode_);
	skinCluster_.Create(skeleton_, standingModel_);
	human_->SetSkinCluster(&skinCluster_);

	obj_.reset(DaiEngine::Object3d::Create(model_));
	obj_->worldTransform_.rotation_.y = 3.14f;

	sprite_.reset(DaiEngine::Sprite::Create(tex_, { 640.0f,360.0f }));
	
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

	ParticleEditor::GetInstance()->Init();

	effect_ = ParticleManager::Load("FireBallImpact");


	
}

void DebugTestScene::Update() {
	DebugGUI();
	dissolve_->DebugGUI();

#ifdef _DEBUG
	//デバッグ用シーンの切り替えコマンド
	if (DaiEngine::Input::GetInstance()->PushKey(DIK_LCONTROL) && DaiEngine::Input::GetInstance()->TriggerKey(DIK_1)) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (DaiEngine::Input::GetInstance()->PushKey(DIK_LCONTROL) && DaiEngine::Input::GetInstance()->TriggerKey(DIK_2)) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Game");
	}
	
#endif // _DEBUG
	
	if (DaiEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		human_->SetModelHandle(sneakModel_);
		animation_ = DaiEngine::AnimationManager::Load(sneakModel_->name_);
		skeleton_ = DaiEngine::Skeleton::Create(sneakModel_->rootNode_);
		skinCluster_.Create(skeleton_, sneakModel_);
		animation_.Start();
		animation_.SetAnimationSpeed(1.0f / 30.0f);
	}
	else if(DaiEngine::Input::GetInstance()->ReleaseKey(DIK_SPACE)){
		human_->SetModelHandle(humanModel_);
		animation_ = DaiEngine::AnimationManager::Load(standingModel_->name_);
		skeleton_ = DaiEngine::Skeleton::Create(standingModel_->rootNode_);
		skinCluster_.Create(skeleton_, standingModel_);
		animation_.Start();
		animation_.SetAnimationSpeed(1.0f / 60.0f);
	}

	if (DaiEngine::Input::GetInstance()->PushKey(DIK_LCONTROL) && DaiEngine::Input::GetInstance()->TriggerKey(DIK_H)) {
		for (auto& [group, particle] : effect_) {
			particle->Emit();
		}
	}

	ParticleEditor::GetInstance()->Update();

	human_->worldTransform_.UpdateMatrix();
	animation_.Play(skeleton_);
	
	skeleton_.Update();
	skinCluster_.Update(skeleton_);

	obj_->worldTransform_.UpdateMatrix();

	for (auto& [group, particle] : effect_) {
		particle->Update();
	}
	///必須処理
	//行列更新
	camera_.UpdateViewMatrix();
	//ライト更新
	pointLight_.Update();
	spotLight_.Update();
	///
}

void DebugTestScene::DrawBackGround() {



}

void DebugTestScene::DrawModel() {

	DaiEngine::SkinningObject::preDraw();
	//human_->Draw(camera_);
	//skeleton_.Draw(human_->worldTransform_, camera_);


	DaiEngine::Object3d::preDraw();
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

	DaiEngine::GPUParticle::preDraw();

	for (auto& [group, particle] : effect_) {
		//particle->Draw(camera_);
	}
	
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

	ImGui::Begin("texture");

	uint32_t image = DaiEngine::TextureManager::Load("cross_01.png");
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = DaiEngine::TextureManager::GetInstance()->GetSRVGPUHandle(image);

	ImGui::ImageButton("cross_01", (ImTextureID)gpuHandle.ptr, ImVec2(128, 128));

	image = DaiEngine::TextureManager::Load("ring_03.png");
	gpuHandle = DaiEngine::TextureManager::GetInstance()->GetSRVGPUHandle(image);

	ImGui::SameLine();

	ImGui::ImageButton("ring_03", (ImTextureID)gpuHandle.ptr, ImVec2(128, 128));

	ImGui::End();

#endif // _DEBUG
}
