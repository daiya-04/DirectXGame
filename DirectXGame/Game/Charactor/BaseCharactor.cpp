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
	//アニメーション再生
	animations_[actionIndex_].Play(skeletons_[actionIndex_]);

	skeletons_[actionIndex_].Update();
	skinClusters_[actionIndex_].Update(skeletons_[actionIndex_]);
	//UI更新
	UpdateUI();
	//コライダー更新
	UpdateCollider();
}

void BaseCharactor::UpdateCollider() {

	collider_.orientation[0] = { rotateMat_.m[0][0],rotateMat_.m[0][1],rotateMat_.m[0][2] };
	collider_.orientation[1] = { rotateMat_.m[1][0],rotateMat_.m[1][1],rotateMat_.m[1][2] };
	collider_.orientation[2] = { rotateMat_.m[2][0],rotateMat_.m[2][1],rotateMat_.m[2][2] };
	
	collider_.center = GetCenterPos();
}

void BaseCharactor::Draw(const Camera& camera) {

#ifdef _DEBUG
	//衝突範囲の可視化
	ShapesDraw::DrawOBB(collider_, camera);
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

	collider_.size = data.colliderSize;

	//回転行列計算
	rotateMat_ = MakeRotateXMatrix(data.rotation.x) * MakeRotateYMatrix(data.rotation.y) * MakeRotateZMatrix(data.rotation.z);
	direction_ = Transform(direction_, rotateMat_);

	BaseCharactor::Update();
}

Vector3 BaseCharactor::GetCenterPos() const {
	Vector3 centerPos{};

	centerPos.x = obj_->GetWorldPos().x;
	centerPos.y = obj_->GetWorldPos().y + collider_.size.y;
	centerPos.z = obj_->GetWorldPos().z;

	return centerPos;
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
