#include "Boss.h"
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "TextureManager.h"
#include "Easing.h"
#include "GameScene.h"
#include "ModelManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"

void Boss::Init(const std::vector<std::shared_ptr<Model>>& models) {

	animationModels_ = models;
	hpBerTex_ = TextureManager::Load("Boss_HP.png");

	obj_.reset(SkinningObject::Create(animationModels_[action_]));
	skinClusters_.resize(animationModels_.size());
	for (size_t index = 0; index < Action::ActionNum; index++) {
		animations_.emplace_back(AnimationManager::Load(animationModels_[index]->name_));
		skeletons_.emplace_back(Skeleton::Create(animationModels_[index]->rootNode_));
		skinClusters_[index].Create(skeletons_[index], animationModels_[index]);
	}
	obj_->SetSkinCluster(&skinClusters_[action_]);
	
	rotateMat_ = DirectionToDirection({0.0f,0.0f,1.0f}, direction_);

	behaviorRequest_ = Behavior::kAppear;

	life_ = maxHp_;

	hpBer_.reset(Sprite::Create(hpBerTex_, { 390.0f,40.0f }));
	hpBer_->SetAnchorpoint({ 0.0f,0.5f });
	hpBer_->SetSize({ 500.0f,10.0f });

}

void Boss::Update() {

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		behaviorInitTable_[behavior_]();

		behaviorRequest_ = std::nullopt;
	}

	behaviorUpdateTable_[behavior_]();

	obj_->SetSkinCluster(&skinClusters_[action_]);
	//行列更新
	
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);

	if (action_ == Action::Standing) {
		animations_[action_].Play(skeletons_[action_]);
	}else {
		animations_[action_].Play(skeletons_[action_], false);
	}
	
	skeletons_[action_].Update();
	skinClusters_[action_].Update(skeletons_[action_]);

	UIUpdate();
	ColliderUpdate();
}

void Boss::UIUpdate() {

	float percent = float(life_) / (float)maxHp_;

	hpBer_->SetSize({ 500.0f * percent,10.0f });
}

void Boss::Draw(const Camera& camera) {

#ifdef _DEBUG
	ShapesDraw::DrawAABB(collider_, camera);
#endif // _DEBUG


	obj_->Draw(camera);
	skeletons_[action_].Draw(obj_->worldTransform_, camera);

}

void Boss::DrawUI() {
	hpBer_->Draw();
}

void Boss::OnCollision() {
	life_--;
	if (life_ == 0) { isDead_ = true; }
}

void Boss::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	workAppear_.startPos = data.translation;
	workAppear_.endPos = data.translation;
	workAppear_.endPos.y = 0.0f;

	size_ = data.colliderSize;

	
	rotateMat_ = MakeRotateXMatrix(data.rotation.x) * MakeRotateYMatrix(data.rotation.y) * MakeRotateZMatrix(data.rotation.z);
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Boss::RootInit() {

	workAttack_.param = 0;
	action_ = Action::Standing;
	direction_ = { 0.0f,0.0f,-1.0f };

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

}

void Boss::RootUpdate() {

	if (++workAttack_.param > workAttack_.coolTime) {
		behaviorRequest_ = Behavior::kAttack;
	}
	
}

void Boss::AttackInit() {

	

	if (attackType_ == AttackType::kElementBall) {
		Vector3 offset[4] = {
		{4.0f,0.0f,2.0f},
		{-4.0f,0.0f,2.0f},
		{4.0f,0.0f,-2.0f},
		{-4.0f,0.0f,-2.0f},
		};

		for (size_t index = 0; index < 4; index++) {
			ElementBall* newElementBall = new ElementBall();
			std::shared_ptr<Model> model = ModelManager::LoadGLTF("ElementBall");
			newElementBall->Init(model, obj_->worldTransform_.translation_ + offset[index]);
			newElementBall->SetShotCount((uint32_t)index + 2);
			gameScene_->AddElementBall(newElementBall);
		}
		attackType_ = AttackType::kGroundFlare;
		
	}else if (attackType_ == AttackType::kGroundFlare) {
		GroundFlare::GetInstance()->AttackStart();
		attackType_ = AttackType::kIcicle;
	}
	else if (attackType_ == AttackType::kIcicle) {
		IcicleManager::GetInstanse()->AttackStart();
		IcicleManager::GetInstanse()->SetAttackData(GetWorldPos(), Vector3(0.0f, 0.0f, -1.0f));
		attackType_ = AttackType::kPlasmaShot;
	}
	else if (attackType_ == AttackType::kPlasmaShot) {
		PlasmaShotManager::GetInstance()->AttackStart();

		direction_ = (target_->translation_ - obj_->worldTransform_.translation_).Normalize();
		rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
		Vector3 offset = { 0.0f,0.0f,2.0f };
		offset = Transform(offset, rotateMat_);

		PlasmaShotManager::GetInstance()->SetAttackData(GetWorldPos() + offset);

		attackType_ = AttackType::kElementBall;
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
