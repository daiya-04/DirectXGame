#include "Boss.h"
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "Easing.h"

const WorldTransform* Boss::target_ = nullptr;

void Boss::Init(const std::vector<std::shared_ptr<Model>>& modelHandle) {

	worldTransform_.Init();
	partsWorldTransform_[Body].Init();
	partsWorldTransform_[Head].Init();

	obj_.resize(modelHandle.size());
	for (size_t index = 0; index < obj_.size(); index++) {
		obj_[index].reset(Object3d::Create(modelHandle[index]));
	}

	worldTransform_.translation_ = workAppear_.startPos;
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };
	partsWorldTransform_[Head].translation_ = { 0.0f,4.8f,0.0f };

	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];

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
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Boss::Draw(const Camera& camera) {

	obj_[Body]->Draw(partsWorldTransform_[Body], camera);
	obj_[Head]->Draw(partsWorldTransform_[Head], camera);

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

	partsWorldTransform_[Body].translation_.y = std::sinf(floatingParameter_) * amplitude;

}

void Boss::RootInit() {



}

void Boss::RootUpdate() {


	FloatingGimmickUpdate();

}

void Boss::AttackInit() {


}

void Boss::AttackUpdate() {



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