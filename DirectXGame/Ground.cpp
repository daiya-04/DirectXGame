#include "Ground.h"

void Ground::Init(std::shared_ptr<Model> modelHandle){

	obj_ = std::make_unique<Object3d>();
	obj_.reset(Object3d::Create(modelHandle));

}

void Ground::Update(){


	obj_->worldTransform_.UpdateMatrix();
}

void Ground::Draw(const Camera& camera){

	obj_->Draw(camera);

}

void Ground::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	obj_->worldTransform_.UpdateMatrix();
}
