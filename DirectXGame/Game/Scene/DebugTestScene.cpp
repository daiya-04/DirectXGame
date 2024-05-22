#include "DebugTestScene.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "Line.h"
#include "ShapesDraw.h"

void DebugTestScene::Init() {

	camera_.Init();
	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	humanModel_ = ModelManager::LoadGLTF("Standing");
	sneakModel_ = ModelManager::LoadGLTF("Walking");
	debugModel_ = ModelManager::LoadOBJ("cube", false);

	human_.reset(SkinningObject::Create(humanModel_));
	animation_ = Animation::LoadAnimationFile(humanModel_->name_);
	skeleton_ = Skeleton::Create(humanModel_->rootNode_);
	skinCluster_.Create(skeleton_, humanModel_);
	human_->SetSkinCluster(&skinCluster_);

	for (Skeleton::Joint& joint : skeleton_.joints_) {
		debugObj_.emplace_back(Object3d::Create(debugModel_));
	}

	human_->worldTransform_.rotation_.y = 3.14f;

}

void DebugTestScene::Update() {

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
		animation_ = Animation::LoadAnimationFile(sneakModel_->name_);
		skeleton_ = Skeleton::Create(sneakModel_->rootNode_);
		skinCluster_.Create(skeleton_, sneakModel_);
	}
	else if(Input::GetInstance()->ReleaseKey(DIK_SPACE)){
		//human_->SetModelHandle(humanModel_);
		animation_ = Animation::LoadAnimationFile(humanModel_->name_);
		skeleton_ = Skeleton::Create(humanModel_->rootNode_);
		skinCluster_.Create(skeleton_, humanModel_);
	}

	human_->worldTransform_.UpdateMatrix();
	animation_.Play(skeleton_);
	
	
	skeleton_.Update();
	skinCluster_.Update(skeleton_);

	for (Skeleton::Joint& joint : skeleton_.joints_) {
		debugObj_[joint.index_]->worldTransform_.matWorld_ = joint.skeletonSpaceMat_ * human_->worldTransform_.matWorld_;
	}

	camera_.UpdateViewMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void DebugTestScene::DrawBackGround() {



}

void DebugTestScene::DrawModel() {

	SkinningObject::preDraw();
	human_->Draw(camera_);

	
	Object3d::preDraw();
	for (const auto& obj : debugObj_) {
		obj->Draw(camera_);
	}

	//ShapesDraw::DrawSphere(Shapes::Sphere({}, 1.0f), camera_);
	//ShapesDraw::DrawPlane(Shapes::Plane({ 0.0f,0.0f,1.0f }, 10.0f), camera_);
	//ShapesDraw::DrawAABB(Shapes::AABB({ -1.0,-1.0,-1.0f }, { 1.0f,1.0f,1.0f }), camera_);

}

void DebugTestScene::DrawParticleModel() {



}

void DebugTestScene::DrawParticle() {



}

void DebugTestScene::DrawUI() {



}

void DebugTestScene::DrawPostEffect() {



}

void DebugTestScene::DebugGUI() {



}
