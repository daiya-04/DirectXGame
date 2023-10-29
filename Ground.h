#pragma once
#include "Vec3.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

enum Type {
	Dynamic,
	Static,
};

class Ground{
private:

	Object3d* model_ = nullptr;
	WorldTransform worldTransform_;
	Type type_;
	Vector3 size_{};
	float speed = 0.1f;

	float amplitude = 0.3f;
	int cycle = 360;
	float moveingParameter = 0.0f;

	Vector3 velocity_{};

public:

	void Initialize(Object3d* model, Type type, const Vector3& position);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	Vector3 GetWorldPos() const;
	Vector3 GetSize()const { return size_; }
	Type GetType() { return type_;}
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetVelocity() const { return velocity_; }

};

