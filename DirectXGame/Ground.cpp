#include "Ground.h"

void Ground::Init(std::shared_ptr<Model> modelHandle){

	obj_ = std::make_unique<Object3d>();
	obj_.reset(Object3d::Create(modelHandle));

	obj_->worldTransform_.scale_ = { 100.0f,1.0f,100.0f };

}

void Ground::Update(){


	obj_->Update();
}

void Ground::Draw(const Camera& camera){

	obj_->Draw(camera);

}
