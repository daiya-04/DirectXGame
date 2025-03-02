#include "BaseCharactor.h"

#include "AnimationManager.h"
#include "ShapesDraw.h"
#include "ColliderManager.h"

BaseCharactor::~BaseCharactor() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void BaseCharactor::Init(const std::vector<std::shared_ptr<DaiEngine::Model>>& models) {

	animationModels_ = models;
	for (size_t index = 0; index < models.size(); index++) {
		animations_.emplace_back(DaiEngine::AnimationManager::Load(animationModels_[index]->name_));
	}

	collider_ = std::make_unique<DaiEngine::OBBCollider>();
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

}

void BaseCharactor::Update() {

	//UI更新
	UpdateUI();
	//コライダー更新
	collider_->Update(rotateMat_);
}

void BaseCharactor::Draw(const DaiEngine::Camera& camera) {

#ifdef _DEBUG
	//衝突範囲の可視化
	ShapesDraw::DrawOBB(std::get<Shapes::OBB>(collider_->GetShape()), camera);
#endif // _DEBUG

}

void BaseCharactor::DrawUI() {
	hpBar_->Draw();
}

void BaseCharactor::SetData(const LevelData::ObjectData& data) {

	collider_->SetSize(data.colliderSize);
	collider_->SetPosition(data.colliderCenter);

	//回転行列計算
	rotateMat_ = MakeRotateXMatrix(data.rotation.x) * MakeRotateYMatrix(data.rotation.y) * MakeRotateZMatrix(data.rotation.z);
	direction_ = Transform(direction_, rotateMat_);

	//コライダー更新
	collider_->Update(rotateMat_);
}

void BaseCharactor::SetDirection(const Vector3& direction) {
	direction_ = direction;
	rotateMat_ = DirectionToDirection(Vector3(0.0f, 0.0f, 1.0f), direction_);
}

void BaseCharactor::SetAnimation(size_t actionIndex, bool isLoop) {
	actionIndex_ = actionIndex;
	animations_[actionIndex_].Start(isLoop);
	animations_[actionIndex_].TimeReset();
}
