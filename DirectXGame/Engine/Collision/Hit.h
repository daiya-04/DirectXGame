#pragma once
#include "Matrix44.h"
#include "CollisionShapes.h"

Vector3 ClosestPoint(const Vector3& point, const Shapes::Segment& segment);

/// <summary>
/// 球体同士の衝突判定
/// </summary>
/// <param name="s1">球体A</param>
/// <param name="s2">球体B</param>
/// <returns></returns>
bool IsCollision(const Shapes::Sphere& s1, const Shapes::Sphere& s2);
/// <summary>
/// 平面と球体との衝突判定
/// </summary>
/// <param name="plane">平面</param>
/// <param name="sphere">球体</param>
/// <returns></returns>
bool IsCollision(const Shapes::Plane& plane, const Shapes::Sphere& sphere);
/// <summary>
/// 線分と平面との衝突判定
/// </summary>
/// <param name="segment">線分</param>
/// <param name="plane">平面</param>
/// <returns></returns>
bool IsCollision(const Shapes::Segment& segment, const Shapes::Plane& plane);
/// <summary>
/// 直線と平面との衝突判定
/// </summary>
/// <param name="line">直線</param>
/// <param name="plane">平面</param>
/// <returns></returns>
bool IsCollision(const Shapes::StraightLine& line, const Shapes::Plane& plane);
/// <summary>
/// 半直線と平面との衝突判定
/// </summary>
/// <param name="ray">半直線</param>
/// <param name="plane">平面</param>
/// <returns></returns>
bool IsCollision(const Shapes::Ray& ray, const Shapes::Plane& plane);
/// <summary>
/// 三角形と線分の衝突判定
/// </summary>
/// <param name="triangle">三角形</param>
/// <param name="segment">平面</param>
/// <returns></returns>
bool IsCollision(const Shapes::Triangle& triangle, const Shapes::Segment& segment);
/// <summary>
/// 三角形と直線との衝突判定
/// </summary>
/// <param name="triangle">三角形</param>
/// <param name="line">直線</param>
/// <returns></returns>
bool IsCollision(const Shapes::Triangle& triangle, const Shapes::StraightLine& line);
/// <summary>
/// 三角形と半直線との衝突判定
/// </summary>
/// <param name="triangle">三角形</param>
/// <param name="ray">半直線</param>
/// <returns></returns>
bool IsCollision(const Shapes::Triangle& triangle, const Shapes::Ray& ray);
/// <summary>
/// AABB同士の衝突判定
/// </summary>
/// <param name="aabb1">AABB1</param>
/// <param name="aabb2">AABB2</param>
/// <returns></returns>
bool IsCollision(const Shapes::AABB& aabb1, const Shapes::AABB& aabb2);
/// <summary>
/// AABBと球体の衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="sphere">球体</param>
/// <returns></returns>
bool IsCollision(const Shapes::AABB& aabb, const Shapes::Sphere& sphere);
/// <summary>
/// AABBと線分との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="segment">線分</param>
/// <returns></returns>
bool IsCollision(const Shapes::AABB& aabb, const Shapes::Segment& segment);
/// <summary>
/// AABBと直線との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="segment">直線</param>
/// <returns></returns>
bool IsCollision(const Shapes::AABB& aabb, const Shapes::StraightLine& segment);
/// <summary>
///AABBと半直線との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="segment">半直線</param>
/// <returns></returns>
bool IsCollision(const Shapes::AABB& aabb, const Shapes::Ray& segment);
/// <summary>
/// AABBと点との衝突判定
/// </summary>
/// <param name="aabb">AABB</param>
/// <param name="point">点</param>
/// <returns></returns>
bool IsCollision(const Shapes::AABB& aabb, const Vector3& point);
/// <summary>
/// OBBと球との衝突判定
/// </summary>
/// <param name="obb"></param>
/// <param name="sphere"></param>
/// <returns></returns>
bool IsCollision(const Shapes::OBB& obb, const Shapes::Sphere& sphere);
/// <summary>
/// OBBと線分との衝突判定
/// </summary>
/// <param name="obb">OBB</param>
/// <param name="segment">線分</param>
/// <returns></returns>
bool IsCollision(const Shapes::OBB& obb, const Shapes::Segment& segment);
/// <summary>
/// OBB同士の衝突判定
/// </summary>
/// <param name="obb1"></param>
/// <param name="obb2"></param>
/// <returns></returns>
bool IsCollision(const Shapes::OBB& obb1, const Shapes::OBB& obb2);
/// <summary>
/// 円と矩形の衝突判定
/// </summary>
/// <param name="circle"></param>
/// <param name=""></param>
/// <returns></returns>
bool IsCollision(const Shapes::Circle& circle, const Shapes::RectAngle& rect);
