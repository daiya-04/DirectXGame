#pragma once
#include "Object3d.h"
#include "Vec3.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Goal{
private:

	Object3d* model_ = nullptr;
	WorldTransform worldTransform_;

public:
	
	void Initialize(Object3d* model,const Vector3& position);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void ApplyGlobalVariables();

	Vector3 GetSize() const { return worldTransform_.scale_; }
	Vector3 GetWorldPos() const;

};

