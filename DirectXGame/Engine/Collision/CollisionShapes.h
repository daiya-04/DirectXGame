#pragma once
#include "Vec3.h"

namespace Shapes{
	// 線分
	typedef struct {
		Vector3 origin; // 始点
		Vector3 diff;   // 終点との差分ベクトル
	} Segment;

	// 直線
	typedef struct {
		Vector3 origin; // 始点
		Vector3 diff;   // 終点との差分ベクトル
	} StraightLine;

	// 半直線
	typedef struct {
		Vector3 origin; // 始点
		Vector3 diff;   // 終点との差分ベクトル
	} Ray;

	// 球
	typedef struct {
		Vector3 center;  // 中心点
		float radius; // 半径
	} Sphere;

	// 平面
	typedef struct {
		Vector3 normal;    // 法線
		float distance; // 原点からの距離
	} Plane;

	// 三角形
	typedef struct {
		Vector3 vertices[3]; // 頂点
	} Triangle;

	typedef struct {
		Vector3 min;
		Vector3 max;
	} AABB;

	typedef struct {
		Vector3 center;
		Vector3 orientation[3];
		Vector3 size;
	} OBB;
}
