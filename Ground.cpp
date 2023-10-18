#include "Ground.h"
#include <cassert>

void Ground::Initialize(Object3d* model) {

	assert(model);
	model_ = model;

}

void Ground::Update() {

	position_ = {};
	scale_ = { 100.0f,100.0f,100.0f };

	model_->SetPosition(position_);
	model_->SetScale(scale_);
}

void Ground::Draw() {

	model_->Draw();

}