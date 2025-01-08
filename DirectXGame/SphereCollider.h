#pragma once
#include "Collider.h"


class SphereCollider : public Collider {
public:

	void Init(const std::string& tag, const WorldTransform& parent, float radius);

	void Update();

	void SetRadius(float radius) { sphere_->radius = radius; }
	const float GetRadius() const { return sphere_->radius; }

private:

	Shapes::Sphere* sphere_ = nullptr;

};

