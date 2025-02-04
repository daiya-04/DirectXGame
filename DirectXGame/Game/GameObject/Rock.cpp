#include "Rock.h"


void Rock::Init(const std::shared_ptr<DaiEngine::Model>& model) {
	//オブジェクト生成
	obj_.reset(DaiEngine::Object3d::Create(model));

}

void Rock::Update() {

	//行列更新
	obj_->worldTransform_.UpdateMatrix();
}

void Rock::Draw(const DaiEngine::Camera& camera) {
	obj_->Draw(camera);
}

void Rock::SetData(const LevelData::ObjectData& data) {
	//データのセット
	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;
	//行列更新
	obj_->worldTransform_.UpdateMatrix();
}
