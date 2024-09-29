#include "Rock.h"


void Rock::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

}

void Rock::Update() {


	obj_->worldTransform_.UpdateMatrix();
}

void Rock::Draw(const Camera& camera) {
	obj_->Draw(camera);
}

void Rock::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	obj_->worldTransform_.UpdateMatrix();
}
