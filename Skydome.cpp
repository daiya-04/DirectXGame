#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(Object3d* model){

	assert(model);
	model_ = model;

}

void Skydome::Update() {

	worldTransform_.translation_ = {};

	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_,viewProjection);

}
