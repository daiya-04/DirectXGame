#include "ElementBall.h"

#include "Easing.h"

const WorldTransform* ElementBall::target_ = nullptr;

void ElementBall::Init(std::shared_ptr<Model> model, const Vector3& startPos) {

	worldTransform_.Init();

	obj_.reset(Object3d::Create(model));

	phaseRequest_ = Phase::kSet;

	workSet_.start = startPos;

}

void ElementBall::Update() {

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		switch (phase_) {
			case Phase::kSet:
				SetInit();
				break;
			case Phase::kCharge:
				ChargeInit();
				break;
			case Phase::kShot:
				ShotInit();
				break;
		}

		phaseRequest_ = std::nullopt;
	}

	switch (phase_) {
		case Phase::kSet:
			SetUpdate();
			break;
		case Phase::kCharge:
			ChargeUpdate();
			break;
		case Phase::kShot:
			ShotUpdate();
			break;
	}

	//行列更新
	worldTransform_.UpdateMatrix();
}

void ElementBall::Draw(const Camera& camera) {

	obj_->Draw(worldTransform_, camera);

}

void ElementBall::SetInit() {

	Vector3 offset = {0.0f,17.0f,0.0f};
	workSet_.end = workSet_.start + offset;
	worldTransform_.translation_ = workSet_.start;
	workSet_.param = 0.0f;

}

void ElementBall::SetUpdate() {

	float T = Easing::easeInOutQuint(workSet_.param);

	worldTransform_.translation_ = Lerp(T, workSet_.start, workSet_.end);

	workSet_.param += 0.005f;
	
	if (workSet_.param >= 1.0f) {
		phaseRequest_ = Phase::kCharge;
		//workSet_.param = 1.0f;
	}

}

void ElementBall::ChargeInit() {

	workCharge_.param = 0;

}

void ElementBall::ChargeUpdate() {

	if (++workCharge_.param >= workCharge_.coolTime) {
		phaseRequest_ = Phase::kShot;
	}

}

void ElementBall::ShotInit() {

	workShot_.move = {};
	workShot_.isTrack = true;

}

void ElementBall::ShotUpdate() {

	Vector3 diff = target_->translation_ - worldTransform_.translation_;
	float distance = diff.Length();
	const float kSpeed = 0.7f;

	if (distance < workShot_.trackingDist) {
		workShot_.isTrack = false;
	}

	if (workShot_.isTrack || worldTransform_.translation_.y >= 2.0f) {
		workShot_.move = diff.Normalize() * kSpeed;
	}

	worldTransform_.translation_ += workShot_.move;

	if (worldTransform_.translation_.y < 0.0f) {
		isLife_ = true;
	}

}
