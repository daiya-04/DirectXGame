#include "Boss.h"
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "Easing.h"
#include "GameScene.h"
#include "ModelManager.h"

const WorldTransform* Boss::target_ = nullptr;

void Boss::Init(const std::vector<std::shared_ptr<Model>>& modelHandle) {

	worldTransform_.Init();

	obj_.resize(modelHandle.size());
	for (size_t index = 0; index < obj_.size(); index++) {
		obj_[index].reset(Object3d::Create(modelHandle[index]));
	}

	worldTransform_.translation_ = workAppear_.startPos;
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };
	obj_[Head]->worldTransform_.translation_ = { 0.0f,4.8f,0.0f };

	obj_[Body]->worldTransform_.parent_ = &worldTransform_;
	obj_[Head]->worldTransform_.parent_ = &obj_[Body]->worldTransform_;

	behaviorRequest_ = Behavior::kAppear;


	FloatingGimmickInit();
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

	//行列更新
	//worldTransform_.UpdateMatrix();
	Matrix4x4 S = MakeScaleMatrix(worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.matWorld_ = S * rotateMat_ * T;
	for (const auto& obj : obj_) {
		obj->worldTransform_.UpdateMatrix();
	}

	ColliderUpdate();
}

void Boss::Draw(const Camera& camera) {

	obj_[Body]->Draw(camera);
	obj_[Head]->Draw(camera);

}

void Boss::OnCollision() {

}

void Boss::FloatingGimmickInit() {

	floatingParameter_ = 0.0f;

}

void Boss::FloatingGimmickUpdate() {

	//1フレームでのパラメータ加算値
	const float step = 2.0f * (float)std::numbers::pi / (float)cycle;

	floatingParameter_ += step;

	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * (float)std::numbers::pi);

	obj_[Body]->worldTransform_.translation_.y = std::sinf(floatingParameter_) * amplitude;

}

void Boss::RootInit() {

	workAttack_.param = 0;


}

void Boss::RootUpdate() {

	if (++workAttack_.param > workAttack_.coolTime) {
		behaviorRequest_ = Behavior::kAttack;
	}
	

	FloatingGimmickUpdate();
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
		newElementBall->Init(model, worldTransform_.translation_ + offset[index]);
		newElementBall->SetShotCount((uint32_t)index + 2);
		gameScene_->AddElementBall(newElementBall);
	}
	

}

void Boss::AttackUpdate() {


	FloatingGimmickUpdate();
}

void Boss::AppearInit() {

	worldTransform_.translation_ = workAppear_.startPos;
	workAppear_.param = 0.0f;

}

void Boss::AppearUpdate() {

	if (workAppear_.param >= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
		return;
	}

	float T = Easing::easeInOutQuart(workAppear_.param);
	worldTransform_.translation_ = Lerp(T, workAppear_.startPos, workAppear_.endPos);

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

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
