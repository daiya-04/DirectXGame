#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(uint32_t modelHandle){

	object_ = std::make_unique<Object3d>();
	object_->Initialize(modelHandle);

}

void Skydome::Update() {

	worldTransform_.translation_ = {};

	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(const ViewProjection& viewProjection) {

	object_->Draw(worldTransform_,viewProjection);

}
