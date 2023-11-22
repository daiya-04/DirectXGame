#pragma once
#include <memory>
#include "Object3d.h"
#include "Vec3.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Goal{
private:

	std::unique_ptr<Object3d> object_;
	WorldTransform worldTransform_;

public:
	
	void Initialize(uint32_t modelHandle);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void ApplyGlobalVariables();

	Vector3 GetSize() const { return worldTransform_.scale_; }
	Vector3 GetWorldPos() const;

};

