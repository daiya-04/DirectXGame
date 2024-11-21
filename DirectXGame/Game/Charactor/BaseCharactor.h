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

	virtual void Init(const std::vector<std::shared_ptr<Model>>& models);

	virtual void Update();
	virtual void UpdateUI() {};
	void UpdateCollider();

	virtual void Draw(const Camera& camera);
	virtual void DrawUI();

	void SetData(const LevelData::ObjectData& data);

	Vector3 GetCenterPos() const;
	bool IsDead() const { return isDead_; }
	Shapes::OBB GetCollider() const { return collider_; }
	virtual bool IsDeadAction() { return false; };
	const WorldTransform& GetWorldTransform() { return obj_->worldTransform_; }
	int32_t GetActionIndex() const { return actionIndex_; }

protected:

	std::unique_ptr<SkinningObject> obj_;
	std::vector<std::shared_ptr<Model>> animationModels_;
	std::vector<Animation> animations_;
	std::vector<Skeleton> skeletons_;
	std::vector<SkinCluster> skinClusters_;

	int32_t actionIndex_ = 0;

	Shapes::OBB collider_;

	Matrix4x4 rotateMat_;
	Vector3 direction_ = { 0.0f,0.0f,1.0f };

	int32_t hp_;

	std::unique_ptr<Sprite> hpBar_;

	bool isDead_ = false;

};

