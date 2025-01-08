#include "BaseCharactor.h"

#include "AnimationManager.h"
#include "ShapesDraw.h"

void BaseCharactor::Init(const std::vector<std::shared_ptr<Model>>& models) {

	animationModels_ = models;

	//モデルやアニメーションの設定
	obj_.reset(SkinningObject::Create(animationModels_[actionIndex_]));
	skinClusters_.resize(animationModels_.size());
	for (size_t index = 0; index < animationModels_.size(); index++) {
		animations_.emplace_back(AnimationManager::Load(animationModels_[index]->name_));
		skeletons_.emplace_back(Skeleton::Create(animationModels_[index]->rootNode_));
		skinClusters_[index].Create(skeletons_[index], animationModels_[index]);
	}
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);

}

void BaseCharactor::Update() {

	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);

	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	UpdateCenterPos();
	//アニメーション再生
	animations_[actionIndex_].Play(skeletons_[actionIndex_]);

	skeletons_[actionIndex_].Update();
	skinClusters_[actionIndex_].Update(skeletons_[actionIndex_]);
	//UI更新
	UpdateUI();
	//コライダー更新
	collider_->Update(rotateMat_);
}

void BaseCharactor::UpdateCenterPos() {
	centerPos_ = {
		obj_->worldTransform_.matWorld_.m[3][0],
		obj_->worldTransform_.matWorld_.m[3][1] + collider_->GetSize().y,
		obj_->worldTransform_.matWorld_.m[3][2],
	};
}

void BaseCharactor::Draw(const Camera& camera) {

#ifdef _DEBUG
	//衝突範囲の可視化
	ShapesDraw::DrawOBB(std::get<Shapes::OBB>(collider_->GetShape()), camera);
#endif // _DEBUG

	obj_->Draw(camera);
	skeletons_[actionIndex_].Draw(obj_->worldTransform_, camera);

}

void BaseCharactor::DrawUI() {
	hpBar_->Draw();
}

void BaseCharactor::SetData(const LevelData::ObjectData& data) {

	obj_->worldTransform_.translation_ = data.translation;
	obj_->worldTransform_.scale_ = data.scaling;

	collider_->SetSize(data.colliderSize);
	collider_->SetPosition(data.colliderCenter);

	//回転行列計算
	rotateMat_ = MakeRotateXMatrix(data.rotation.x) * MakeRotateYMatrix(data.rotation.y) * MakeRotateZMatrix(data.rotation.z);
	direction_ = Transform(direction_, rotateMat_);

	BaseCharactor::Update();
}

void BaseCharactor::SetAnimation(size_t actionIndex, bool isLoop) {

	actionIndex_ = actionIndex;
	animations_[actionIndex_].Start(isLoop);
	animations_[actionIndex_].TimeReset();
	obj_->SetSkinCluster(&skinClusters_[actionIndex_]);

}

void BaseCharactor::SetDirection(const Vector3& direction) {
	direction_ = direction;
	rotateMat_ = DirectionToDirection(Vector3(0.0f, 0.0f, 1.0f), direction_);
}

void BaseCharactor::DissolveUpdate() {

	obj_->threshold_ = animations_[actionIndex_].GetAnimationTime() / animations_[actionIndex_].GetDuration();
	obj_->threshold_ = std::clamp(obj_->threshold_, 0.0f, animations_[actionIndex_].GetDuration());

}
