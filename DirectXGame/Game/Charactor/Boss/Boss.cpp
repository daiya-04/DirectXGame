#include "Boss.h"
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "TextureManager.h"
#include "Easing.h"
#include "ModelManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"

#include "GroundFlare.h"
#include "IcicleManager.h"
#include "PlasmaShotManager.h"
#include "ElementBallManager.h"

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
	obj_->threshold_ = 0.0f;

	appearEff_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 10000));
	appearEff_->isLoop_ = false;

	appearEff_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	appearEff_->emitter_.angle = 0.0f;
	appearEff_->emitter_.emitterType = 4;
	appearEff_->emitter_.count = 100;
	appearEff_->emitter_.frequency = 1.0f / 60.0f;
	appearEff_->emitter_.color = Vector4(0.2f, 0.05f, 0.32f, 1.0f);
	appearEff_->emitter_.lifeTime = 2.0f;
	appearEff_->emitter_.scale = 0.5f;
	appearEff_->emitter_.size = Vector3(2.0f, 1.0f, 1.0f);
	appearEff_->emitter_.speed = 5.0f;
	
	
	rotateMat_ = DirectionToDirection({0.0f,0.0f,1.0f}, direction_);

	behaviorRequest_ = Behavior::kAppear;
	attackType_ = AttackType::kElementBall;

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

	appearEff_->emitter_.translate = obj_->GetWorldPos();
	appearEff_->emitter_.translate.y = 0.01f;

	appearEff_->Update();

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

void Boss::DrawParticle(const Camera& camera) {

	appearEff_->Draw(camera);

}

void Boss::DrawUI() {
	hpBer_->Draw();
}

void Boss::OnCollision() {
	life_--;
	if (life_ <= 0) {
		isDead_ = true;
		behaviorRequest_ = Behavior::kDead;
	}
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
		ElementBallManager::GetInstance()->SetAttackData(obj_->worldTransform_.translation_);
		ElementBallManager::GetInstance()->AttackStart();
		
		
		attackType_ = AttackType::kGroundFlare;
		
	}else if (attackType_ == AttackType::kGroundFlare) {
		GroundFlare::GetInstance()->AttackStart();
		attackType_ = AttackType::kIcicle;
	}
	else if (attackType_ == AttackType::kIcicle) {
		IcicleManager::GetInstanse()->SetAttackData(GetWorldPos(), Vector3(0.0f, 0.0f, -1.0f));
		IcicleManager::GetInstanse()->AttackStart();
		attackType_ = AttackType::kPlasmaShot;
	}
	else if (attackType_ == AttackType::kPlasmaShot) {

		direction_ = (target_->translation_ - obj_->worldTransform_.translation_).Normalize();
		rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
		Vector3 offset = { 0.0f,0.0f,2.0f };
		offset = Transform(offset, rotateMat_);

		PlasmaShotManager::GetInstance()->SetAttackData(GetWorldPos() + offset);

		PlasmaShotManager::GetInstance()->AttackStart();

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
	appearEff_->isLoop_ = true;

}

void Boss::AppearUpdate() {

	if (workAppear_.param >= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
		appearEff_->isLoop_ = false;
		return;
	}

	float T = Easing::easeInOutQuart(workAppear_.param);
	obj_->worldTransform_.translation_ = Lerp(T, workAppear_.startPos, workAppear_.endPos);

	workAppear_.param += 0.005f;

}

void Boss::DeadInit() {

	action_ = Action::Dead;
	animations_[action_].SetAnimationSpeed(1.0f / 60.0f);
	animations_[action_].Start();

	obj_->threshold_ = 0.0f;
	
}

void Boss::DeadUpdate() {

	if (!animations_[action_].IsPlaying()) {
		isFinishDeadMotion_ = true;
	}

	obj_->threshold_ = animations_[Action::Dead].GetAnimationTime() / animations_[Action::Dead].GetDuration();
	obj_->threshold_ = std::clamp(obj_->threshold_, 0.0f, animations_[Action::Dead].GetDuration());

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
		case Behavior::kDead:
			behaviorRequest_ = Behavior::kDead;
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
