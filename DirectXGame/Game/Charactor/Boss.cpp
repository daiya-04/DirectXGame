#include "Boss.h"
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "Easing.h"
#include "GameScene.h"
#include "ModelManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"

const WorldTransform* Boss::target_ = nullptr;

void Boss::Init(const std::vector<std::shared_ptr<Model>>& models) {

	animationModels_ = models;

	obj_.reset(SkinningObject::Create(animationModels_[action_]));
	skinClusters_.resize(animationModels_.size());
	for (size_t index = 0; index < Action::ActionNum; index++) {
		animations_.emplace_back(AnimationManager::Load(animationModels_[index]->name_));
		skeletons_.emplace_back(Skeleton::Create(animationModels_[index]->rootNode_));
		skinClusters_[index].Create(skeletons_[index], animationModels_[index]);
	}
	obj_->SetSkinCluster(&skinClusters_[action_]);
	
	rotateMat_ = DirectionToDirection({0.0f,0.0f,1.0f}, { 0.0f,0.0f,-1.0f });

	behaviorRequest_ = Behavior::kAppear;

}

void Boss::Update() {

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
			case Behavior::kRoot:
				RootInit();
				break;
			case Behavior::kAttack:
				AttackInit();
				break;
			case Behavior::kAppear:
				AppearInit();
				break;
		}

		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
		case Behavior::kRoot:
			RootUpdate();
			break;
		case Behavior::kAttack:
			AttackUpdate();
			break;
		case Behavior::kAppear:
			AppearUpdate();
			break;
	}

	obj_->SetSkinCluster(&skinClusters_[action_]);
	//行列更新
	//worldTransform_.UpdateMatrix();
	Matrix4x4 S = MakeScaleMatrix(obj_->worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(obj_->worldTransform_.translation_);
	obj_->worldTransform_.matWorld_ = S * rotateMat_ * T;

	if (action_ == Action::Standing) {
		animations_[action_].Play(skeletons_[action_]);
	}else {
		animations_[action_].Play(skeletons_[action_], false);
	}
	
	skeletons_[action_].Update();
	skinClusters_[action_].Update(skeletons_[action_]);

	ColliderUpdate();
}

void Boss::Draw(const Camera& camera) {

#ifdef _DEBUG
	ShapesDraw::DrawAABB(collider_, camera);
#endif // _DEBUG


	obj_->Draw(camera);
	skeletons_[action_].Draw(obj_->worldTransform_, camera);

}

void Boss::OnCollision() {

}

void Boss::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	baseStatus_.HP_ = data.status.HP;
	baseStatus_.power_ = data.status.power;
	baseStatus_.difense_ = data.status.defense;


	Matrix4x4 S = MakeScaleMatrix(obj_->worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(obj_->worldTransform_.translation_);
	obj_->worldTransform_.matWorld_ = S * rotateMat_ * T;
}

void Boss::RootInit() {

	workAttack_.param = 0;
	action_ = Action::Standing;

}

void Boss::RootUpdate() {

	if (++workAttack_.param > workAttack_.coolTime) {
		behaviorRequest_ = Behavior::kAttack;
	}
	
}

void Boss::AttackInit() {

	Vector3 offset[4] = {
		{5.0f,0.0f,3.0f},
		{-5.0f,0.0f,3.0f},
		{5.0f,0.0f,-3.0f},
		{-5.0f,0.0f,-3.0f},
	};

	for (size_t index = 0; index < 4; index++) {
		ElementBall* newElementBall = new ElementBall();
		std::shared_ptr<Model> model = ModelManager::LoadGLTF("ElementBall");
		newElementBall->Init(model, obj_->worldTransform_.translation_ + offset[index]);
		newElementBall->SetShotCount((uint32_t)index + 2);
		gameScene_->AddElementBall(newElementBall);
	}


	action_ = Action::AttackSet;
	animations_[action_].Play(skeletons_[action_]);

}

void Boss::AttackUpdate() {

	if ((action_ == Action::Attack || action_ == Action::AttackSet) && !animations_[action_].IsPlaying()) {
		action_ = Action::Standing;
	}
	
}

void Boss::AppearInit() {

	obj_->worldTransform_.translation_ = workAppear_.startPos;
	workAppear_.param = 0.0f;

}

void Boss::AppearUpdate() {

	if (workAppear_.param >= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
		return;
	}

	float T = Easing::easeInOutQuart(workAppear_.param);
	obj_->worldTransform_.translation_ = Lerp(T, workAppear_.startPos, workAppear_.endPos);

	workAppear_.param += 0.005f;

}

void Boss::ChangeBehavior(Behavior behavior) {

	switch (behavior) {
		case Behavior::kAppear:
			behaviorRequest_ = Behavior::kAppear;
			break;
		case Behavior::kAttack:
			behaviorRequest_ = Behavior::kAttack;
			break;
		case Behavior::kRoot:
			behaviorRequest_ = Behavior::kRoot;
			break;
	}

}

Vector3 Boss::GetWorldPos() const {
	Vector3 worldPos;

	worldPos.x = obj_->worldTransform_.matWorld_.m[3][0];
	worldPos.y = obj_->worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = obj_->worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
