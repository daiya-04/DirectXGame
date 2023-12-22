#include "Ground.h"

void Ground::Init(uint32_t modelHandle){

	worldTransform_.Init();

	obj_ = std::make_unique<Object3d>();
	obj_.reset(Object3d::Create(modelHandle));

	worldTransform_.scale_ = { 5.0f,1.0f,5.0f };

}

void Ground::Update(){


	worldTransform_.UpdateMatrix();
}

void Ground::Draw(const Camera& camera){

	obj_->Draw(worldTransform_, camera);

}
