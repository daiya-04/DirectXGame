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
	//ライト初期化
	pointLight_.Init();
	spotLight_.Init();
	//Object3dにライトセット
	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	///

	humanModel_ = ModelManager::LoadGLTF("Skin");
	standingModel_ = ModelManager::LoadGLTF("Standing");
	sneakModel_ = ModelManager::LoadGLTF("BossDead");
	model_ = ModelManager::LoadOBJ("Rock");

	skyBoxTex_ = TextureManager::Load("skyBox.dds");
	tex_ = TextureManager::Load("test.png");
	burnScarsTex_ = TextureManager::Load("BurnScars.png");

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

	ParticleEditor::GetInstance()->Init();

	effect_ = ParticleManager::Load("FireBallImpact");


	
}

void DebugTestScene::Update() {
	DebugGUI();
	dissolve_->DebugGUI();

#ifdef _DEBUG
	//デバッグ用シーンの切り替えコマンド
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

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_H)) {
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

	for (auto& [group, particle] : effect_) {
		particle->Draw(camera_);
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

#endif // _DEBUG
}
