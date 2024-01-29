#include "Skydome.h"

void Skydome::Init(uint32_t modelHandle) {

	worldTransform_.Init();

	obj_ = std::make_unique<Object3d>();
	obj_.reset(Object3d::Create(modelHandle));

	worldTransform_.UpdateMatrix();
}

void Skydome::Update() {


	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(const Camera& camera){
	obj_->Draw(worldTransform_, camera);
}
