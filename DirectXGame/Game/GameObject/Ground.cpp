#include "Ground.h"
#include "ColliderManager.h"

Ground::~Ground() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Ground::Init(std::shared_ptr<DaiEngine::Model> model){

	//オブジェクト生成
	obj_.reset(DaiEngine::Object3d::Create(model));

	collider_ = std::make_unique<DaiEngine::OBBCollider>();
	collider_->Init("Ground", obj_->worldTransform_, {});
	collider_->SetCallbackFunc([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	collider_->ColliderOn();
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

}

void Ground::Update(){

	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	collider_->Update(rotateMat_);
}

void Ground::Draw(const DaiEngine::Camera& camera){

	obj_->Draw(camera);

}

void Ground::SetData(const LevelData::ObjectData& data) {
	//データのセット
	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	collider_->SetSize(data.colliderSize);
	collider_->SetPosition(data.colliderCenter);

	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Ground::OnCollision([[maybe_unused]] DaiEngine::Collider* other) {

}
