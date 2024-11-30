#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "Animation.h"
#include "SkinCluster.h"
#include "LevelLoader.h"
#include "CollisionShapes.h"
#include "Vec3.h"
#include "Matrix44.h"
#include "Sprite.h"

#include <memory>
#include <vector>

class BaseCharactor {
public:
	//初期化
	virtual void Init(const std::vector<std::shared_ptr<Model>>& models);
	//更新
	virtual void Update();
	//UI更新
	virtual void UpdateUI() {};
	//コライダー更新
	void UpdateCollider();

	//モデル描画
	virtual void Draw(const Camera& camera);
	//UI描画
	virtual void DrawUI();

	//データ設定
	void SetData(const LevelData::ObjectData& data);

	//キャラの中心位置取得
	Vector3 GetCenterPos() const;
	//死亡しているか
	bool IsDead() const { return isDead_; }
	Shapes::OBB GetCollider() const { return collider_; }
	const WorldTransform& GetWorldTransform() { return obj_->worldTransform_; }

protected:

	std::unique_ptr<SkinningObject> obj_;
	std::vector<std::shared_ptr<Model>> animationModels_;
	std::vector<Animation> animations_;
	std::vector<Skeleton> skeletons_;
	std::vector<SkinCluster> skinClusters_;

	int32_t actionIndex_ = 0;
	//コライダー(衝突形状)
	Shapes::OBB collider_;

	//回転行列
	Matrix4x4 rotateMat_;
	//向いている方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };

	//HP
	int32_t hp_;
	//HP用UI
	std::unique_ptr<Sprite> hpBar_;

	//死亡フラグ
	bool isDead_ = false;

};

