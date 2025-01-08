#include "SphereCollider.h"


void SphereCollider::Init(const std::string&tag, const WorldTransform& parent, float radius) {

	tag_ = tag;

	worldTransform_.Init();
	shapeType_ = Type::Sphere;

	worldTransform_.parent_ = &parent;

	shape_ = Shapes::Sphere();

	sphere_ = std::get_if<Shapes::Sphere>(&shape_);
	sphere_->radius = radius;

}

void SphereCollider::Update() {

	worldTransform_.UpdateMatrix();

	sphere_->center = {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]
	};

}
