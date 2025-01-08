#pragma once
#include "Collider.h"
#include "Matrix44.h"

class OBBCollider : public Collider {
public:

	void Init(const std::string& tag, const WorldTransform& parent, const Vector3& size);

	void Update(const Matrix4x4& rotateMat);

	void SetSize(const Vector3& size) { obb_->size = size; }
	const Vector3 GetSize() const { return obb_->size; }

private:

	Shapes::OBB* obb_ = nullptr;

};

