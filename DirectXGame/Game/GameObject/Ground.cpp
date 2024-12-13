#include "Ground.h"

void Ground::Init(std::shared_ptr<Model> model){

	//オブジェクト生成
	obj_.reset(Object3d::Create(model));

}

void Ground::Update(){

	//行列更新
	obj_->worldTransform_.UpdateMatrix();
}

void Ground::Draw(const Camera& camera){

	obj_->Draw(camera);

}

void Ground::SetData(const LevelData::ObjectData& data) {
	//データのセット
	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;
	//行列更新
	obj_->worldTransform_.UpdateMatrix();
}
