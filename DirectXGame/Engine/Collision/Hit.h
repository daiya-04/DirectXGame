#pragma once
#include "Vec3.h"
#include "Matrix44.h"

// 線分
typedef struct {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点との差分ベクトル
} Segment;

// 直線
typedef struct {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点との差分ベクトル
} Line;

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

Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

/// <summary>
/// 球体同士の衝突判定
/// </summary>
/// <param name="s1">球体A</param>
/// <param name="s2">球体B</param>
/// <returns></returns>
bool IsCollision(const Sphere& s1, const Sphere& s2);
/// <summary>
/// 平面と球体との衝突判定
/// </summary>
/// <param name="plane">平面</param>
/// <param name="sphere">球体</param>
/// <returns></returns>
bool IsCollision(const Plane& plane, const Sphere& sphere);
/// <summary>
/// 線分と平面との衝突判定
/// </summary>
/// <param name="segment">線分</param>
/// <param name="plane">平面</param>
/// <returns></returns>
bool IsCollision(const Segment& segment, const Plane& plane);
/// <summary>
/// 直線と平面との衝突判定
/// </summary>
/// <param name="line">直線</param>
/// <param name="plane">平面</param>
/// <returns></returns>
bool IsCollision(const Line& line, const Plane& plane);
/// <summary>
/// 半直線と平面との衝突判定
/// </summary>
/// <param name="ray">半直線</param>
/// <param name="plane">平面</param>
/// <returns></returns>
bool IsCollision(const Ray& ray, const Plane& plane);
/// <summary>
/// 三角形と線分の衝突判定
/// </summary>
/// <param name="triangle">三角形</param>
/// <param name="segment">平面</param>
/// <returns></returns>
bool IsCollision(const Triangle& triangle, const Segment& segment);
/// <summary>
/// 三角形と直線との衝突判定
/// </summary>
/// <param name="triangle">三角形</param>
/// <param name="line">直線</param>
/// <returns></returns>
bool IsCollision(const Triangle& triangle, const Line& line);
/// <summary>
/// 三角形と半直線との衝突判定
/// </summary>
/// <param name="triangle">三角形</param>
/// <param name="ray">半直線</param>
/// <returns></returns>
bool IsCollision(const Triangle& triangle, const Ray& ray);
/// <summary>
/// AABB同士の衝突判定
/// </summary>
/// <param name="aabb1">AABB1</param>
/// <param name="aabb2">AABB2</param>
/// <returns></returns>
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
/// <summary>
/// AABBと球体の衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="sphere">球体</param>
/// <returns></returns>
bool IsCollision(const AABB& aabb, const Sphere& sphere);
/// <summary>
/// AABBと線分との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="segment">線分</param>
/// <returns></returns>
bool IsCollision(const AABB& aabb, const Segment& segment);
/// <summary>
/// AABBと直線との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="segment">直線</param>
/// <returns></returns>
bool IsCollision(const AABB& aabb, const Line& segment);
/// <summary>
///AABBと半直線との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="segment">半直線</param>
/// <returns></returns>
bool IsCollision(const AABB& aabb, const Ray& segment);

bool IsCollision(const AABB& aabb, const Vector3& point);

bool IsCollision(const OBB& obb, const Sphere& sphere);