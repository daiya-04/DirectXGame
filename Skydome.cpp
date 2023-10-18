#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(Object3d* model){

	assert(model);
	model_ = model;

}

void Skydome::Update() {

	position_ = {};

	model_->SetPosition(position_);
}

void Skydome::Draw() {

	model_->Draw();

}
