#include "Skydome.h"

void Skydome::Init(std::shared_ptr<Model> modelHandle) {

	obj_ = std::make_unique<Object3d>();
	obj_.reset(Object3d::Create(modelHandle));

}

void Skydome::Update() {


	obj_->Update();
}

void Skydome::Draw(const Camera& camera){
	obj_->Draw(camera);
}
