#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "Camera.h"
#include "CollisionShapes.h"

//ワイヤーフレームでの基本形状の描画
namespace ShapesDraw {

	//球体描画
	void DrawSphere(const Shapes::Sphere& sphere, const Camera& camera, const Vector4& color = {1.0f,1.0f,1.0f,1.0f});
	//平面描画
	void DrawPlane(const Shapes::Plane& plane, const Camera& camera, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
	//三角形
	void DrawTriangle(const Shapes::Triangle& triangle, const Camera, const Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	//AABB描画
	void DrawAABB(const Shapes::AABB& aabb, const Camera& camera, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
	//OBB描画
	void DrawOBB(const Shapes::OBB& obb, const Camera& camera, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });


}
