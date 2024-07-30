#include "ElementBall.h"

#include "Easing.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "TextureManager.h"

const WorldTransform* ElementBall::target_ = nullptr;

void ElementBall::Init(std::shared_ptr<Model> model, const Vector3& startPos) {

	obj_.reset(Object3d::Create(model));
	animation_ = AnimationManager::Load(obj_->GetModel()->name_);
	obj_->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };

	phaseRequest_ = Phase::kSet;

	workSet_.start = startPos;

	particle_.reset(GPUParticle::Create(TextureManager::Load("FireParticle.png"), 2000));

	particle_->emitter_.size = 1.3f;
	particle_->emitter_.scale = 0.5f;
	particle_->emitter_.direction = Vector3(1.0f, 0.0f, 0.0f).Normalize();
	particle_->emitter_.angle = 360.0f;
	particle_->emitter_.frequency = 0.01f;
	particle_->emitter_.count = 50;

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
	obj_->worldTransform_.UpdateMatrix();

	if (animation_.IsPlaying()) {
		animation_.Play(obj_->GetModel()->rootNode_, false);
		obj_->worldTransform_.matWorld_ = animation_.GetLocalMatrix() * obj_->worldTransform_.matWorld_;
	}

	particle_->emitter_.translate = GetWorldPos();
	particle_->Update();

	ColliderUpdate();
}

void ElementBall::Draw(const Camera& camera) {

#ifdef _DEBUG
	ShapesDraw::DrawSphere(collider_, camera);
#endif // _DEBUG

	obj_->Draw(camera);

}

void ElementBall::DrawParticle(const Camera& camera) {
	particle_->Draw(camera);
}

void ElementBall::OnCollision() {
	isLife_ = true;
}

void ElementBall::SetInit() {

	obj_->worldTransform_.translation_ = workSet_.start;
	animation_.Start();

}

void ElementBall::SetUpdate() {

	if (!animation_.IsPlaying()) {
		phaseRequest_ = Phase::kCharge;
		animation_.End();
		obj_->worldTransform_.translation_.x = obj_->worldTransform_.matWorld_.m[3][0];
		obj_->worldTransform_.translation_.y = obj_->worldTransform_.matWorld_.m[3][1];
		obj_->worldTransform_.translation_.z = obj_->worldTransform_.matWorld_.m[3][2];
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

	Vector3 diff = target_->translation_ - obj_->worldTransform_.translation_;
	float distance = diff.Length();
	const float kSpeed = 0.5f;

	if (distance < workShot_.trackingDist) {
		workShot_.isTrack = false;
	}

	if (workShot_.isTrack || obj_->worldTransform_.translation_.y >= 2.0f) {
		workShot_.move = diff.Normalize() * kSpeed;
	}

	obj_->worldTransform_.translation_ += workShot_.move;

	if (obj_->worldTransform_.translation_.y < 0.0f) {
		isLife_ = true;
	}

}
