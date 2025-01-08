#include "OBBCollider.h"


void OBBCollider::Init(const std::string& tag, const WorldTransform& parent, const Vector3& size) {

	tag_ = tag;
	worldTransform_.Init();
	shapeType_ = Type::OBB;

	worldTransform_.parent_ = &parent;

	shape_ = Shapes::OBB();

	obb_ = std::get_if<Shapes::OBB>(&shape_);
	obb_->size = size;

}

void OBBCollider::Update(const Matrix4x4& rotateMat) {

	worldTransform_.UpdateMatrixRotate(rotateMat);

	obb_->center = {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]
	};

	obb_->orientation[0] = { rotateMat.m[0][0], rotateMat.m[0][1] ,rotateMat.m[0][2] };
	obb_->orientation[1] = { rotateMat.m[1][0], rotateMat.m[1][1] ,rotateMat.m[1][2] };
	obb_->orientation[2] = { rotateMat.m[2][0], rotateMat.m[2][1] ,rotateMat.m[2][2] };

}
