#pragma once
#include "Matrix44.h"
#include "CollisionShapes.h"

using Shapes::Segment;
using Shapes::StraightLine;
using Shapes::Ray;
using Shapes::Sphere;
using Shapes::Plane;
using Shapes::Triangle;
using Shapes::AABB;
using Shapes::OBB;

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
bool IsCollision(const StraightLine& line, const Plane& plane);
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
bool IsCollision(const Triangle& triangle, const StraightLine& line);
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
bool IsCollision(const AABB& aabb, const StraightLine& segment);
/// <summary>
///AABBと半直線との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="segment">半直線</param>
/// <returns></returns>
bool IsCollision(const AABB& aabb, const Ray& segment);
/// <summary>
/// AABBと点との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="point">点</param>
/// <returns></returns>
bool IsCollision(const AABB& aabb, const Vector3& point);
/// <summary>
/// OBBと球との衝突判定
/// </summary>
/// <param name="obb"></param>
/// <param name="sphere"></param>
/// <returns></returns>
bool IsCollision(const OBB& obb, const Sphere& sphere);
/// <summary>
/// OBBと線分との衝突判定
/// </summary>
/// <param name="obb">OBB</param>
/// <param name="segment">線分</param>
/// <returns></returns>
bool IsCollision(const OBB& obb, const Segment& segment);
/// <summary>
/// OBB同士の衝突判定
/// </summary>
/// <param name="obb1"></param>
/// <param name="obb2"></param>
/// <returns></returns>
bool IsCollision(const OBB& obb1, const OBB& obb2);
