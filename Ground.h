#pragma once
#include <memory>
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

	std::unique_ptr<Object3d> object_;
	WorldTransform worldTransform_;
	Type type_;
	Vector3 size_{};
	float speed = 0.1f;

	float amplitude = 0.3f;
	int cycle = 360;
	float moveingParameter = 0.0f;

	Vector3 velocity_{};

public:

	void Initialize(uint32_t modelHandle, Type type, const Vector3& position,const Vector3& scale = {1.0f,1.0f,1.0f});

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; }
	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

	Vector3 GetWorldPos() const;
	Vector3 GetSize()const { return size_; }
	Type GetType() { return type_;}
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetVelocity() const { return velocity_; }

	Vector3 GetPos() const { return worldTransform_.translation_; }
	Vector3 GetScale() const { return worldTransform_.scale_; }

};

