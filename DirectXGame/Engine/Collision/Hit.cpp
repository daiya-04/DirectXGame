#include "Hit.h"
#include <cmath>
#include <algorithm>

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	return (segment.origin + Project((point - segment.origin), segment.diff));
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {

	float distance = (s1.center - s2.center).Length();

	if (distance <= s1.radius + s2.radius) {
		return true;
	}

	return false;
}

bool IsCollision(const Plane& plane, const Sphere& sphere) {

	float distance = std::fabs(Dot(plane.normal, sphere.center) - plane.distance);

	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

bool IsCollision(const Segment& segment, const Plane& plane) {

	float dot = Dot(plane.normal, segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const Line& line, const Plane& plane) {

	float dot = Dot(plane.normal, line.diff);

	if (dot == 0.0f) {
		return false;
	}

	//float t = (plane.distance - Dot(line.origin, plane.normal)) / dot;

	return true;
}

bool IsCollision(const Ray& ray, const Plane& plane) {

	float dot = Dot(plane.normal, ray.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(ray.origin, plane.normal)) / dot;

	if (t >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Segment& segment) {

	Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
	Vector3 v12 = triangle.vertices[2] - triangle.vertices[1];
	Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];

	Vector3 normal = (Cross(v01, v12)).Normalize();
	float distance = Dot(triangle.vertices[0], normal);

	float dot = Dot(normal, segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (distance - Dot(segment.origin, normal)) / dot;

	Vector3 p = segment.origin + (t * segment.diff);

	Vector3 v0p = p - triangle.vertices[0];
	Vector3 v1p = p - triangle.vertices[1];
	Vector3 v2p = p - triangle.vertices[2];

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f) {
		if (0.0f <= t && t <= 1.0f) {
			return true;
		}
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Line& line) {
	Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
	Vector3 v12 = triangle.vertices[2] - triangle.vertices[1];
	Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];

	Vector3 normal = (Cross(v01, v12)).Normalize();
	float distance = Dot(triangle.vertices[0], normal);

	float dot = Dot(normal, line.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (distance - Dot(line.origin, normal)) / dot;

	Vector3 p = line.origin + (t * line.diff);

	Vector3 v0p = p - triangle.vertices[0];
	Vector3 v1p = p - triangle.vertices[1];
	Vector3 v2p = p - triangle.vertices[2];

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Ray& ray) {
	Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
	Vector3 v12 = triangle.vertices[2] - triangle.vertices[1];
	Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];

	Vector3 normal = (Cross(v01, v12)).Normalize();
	float distance = Dot(triangle.vertices[0], normal);

	float dot = Dot(normal, ray.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (distance - Dot(ray.origin, normal)) / dot;

	Vector3 p = ray.origin + (t * ray.diff);

	Vector3 v0p = p - triangle.vertices[0];
	Vector3 v1p = p - triangle.vertices[1];
	Vector3 v2p = p - triangle.vertices[2];

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f) {
		if (0.0f <= t) {
			return true;
		}
	}
	return false;
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}

	return false;
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {

	Vector3 closestPoint = {
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z)
	};

	float distance = (closestPoint - sphere.center).Length();

	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Segment& segment) {

	float tXmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYmin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYmax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(tXmin, tXmax);
	float tFarX = std::max(tXmin, tXmax);
	float tNearY = std::min(tYmin, tYmax);
	float tFarY = std::max(tYmin, tYmax);
	float tNearZ = std::min(tZmin, tZmax);
	float tFarZ = std::max(tZmin, tZmax);

	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax) {
		if ((0.0f <= tmin && tmin <= 1.0f)||(0.0f <= tmax && tmax <= 1.0f)) { return true; }
		if ((aabb.min.x <= segment.origin.x && segment.origin.x <= aabb.max.x) &&
			(aabb.min.y <= segment.origin.y && segment.origin.y <= aabb.max.y) &&
			(aabb.min.z <= segment.origin.z && segment.origin.z <= aabb.max.z)) {
			return true;
		}
	}

	return false;
}

bool IsCollision(const AABB& aabb, const Line& segment) {

	float tXmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYmin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYmax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(tXmin, tXmax);
	float tFarX = std::max(tXmin, tXmax);
	float tNearY = std::min(tYmin, tYmax);
	float tFarY = std::max(tYmin, tYmax);
	float tNearZ = std::min(tZmin, tZmax);
	float tFarZ = std::max(tZmin, tZmax);

	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax) {
		return true;
	}

	return false;
}

bool IsCollision(const AABB& aabb, const Ray& segment) {

	float tXmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYmin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYmax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(tXmin, tXmax);
	float tFarX = std::max(tXmin, tXmax);
	float tNearY = std::min(tYmin, tYmax);
	float tFarY = std::max(tYmin, tYmax);
	float tNearZ = std::min(tZmin, tZmax);
	float tFarZ = std::max(tZmin, tZmax);

	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax) {
		if (0.0f <= tmin || 0.0f <= tmax) { return true; }
		
	}

	return false;
}

bool IsCollision(const AABB& aabb, const Vector3& point){

	Vector3 closestPoint = {
		std::clamp(point.x,aabb.min.x,aabb.max.x),
		std::clamp(point.y,aabb.min.y,aabb.max.y),
		std::clamp(point.z,aabb.min.z,aabb.max.z)
	};

	float distance = (closestPoint - point).Length();

	if (distance <= 0.0f) {
		return true;
	}

	return false;
}

bool IsCollision(const OBB& obb, const Sphere& sphere) {

	Matrix4x4 obbWorldMatrix = {
		obb.orientation[0].x,obb.orientation[0].y,obb.orientation[0].z,0.0f,
		obb.orientation[1].x,obb.orientation[1].y,obb.orientation[1].z,0.0f,
		obb.orientation[2].x,obb.orientation[2].y,obb.orientation[2].z,0.0f,
		obb.center.x,obb.center.y,obb.center.z,1.0f
	};

	Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbWorldMatrix.Inverse());

	AABB aabbOBBLocal{ -obb.size,obb.size };
	Sphere sphereOBBLocal{ centerInOBBLocalSpace,sphere.radius };

	if (IsCollision(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}

	return false;
}

bool IsCollision(const OBB& obb, const Segment& segment) {

	Matrix4x4 obbWorldMatrix = {
		obb.orientation[0].x,obb.orientation[0].y,obb.orientation[0].z,0.0f,
		obb.orientation[1].x,obb.orientation[1].y,obb.orientation[1].z,0.0f,
		obb.orientation[2].x,obb.orientation[2].y,obb.orientation[2].z,0.0f,
		obb.center.x,obb.center.y,obb.center.z,1.0f
	};

	Vector3 localOrigin = Transform(segment.origin, obbWorldMatrix.Inverse());
	Vector3 localEnd = Transform(segment.origin + segment.diff, obbWorldMatrix.Inverse());

	AABB localAABB = { -obb.size,obb.size };
	Segment localSegment = { localOrigin,localEnd - localOrigin };

	return IsCollision(localAABB, localSegment);
}

bool IsCollision(const OBB& obb1, const OBB& obb2) {

	Vector3 separationAxisCandidate[15];

	//面法線
	separationAxisCandidate[0] = obb1.orientation[0];
	separationAxisCandidate[1] = obb1.orientation[1];
	separationAxisCandidate[2] = obb1.orientation[2];
	separationAxisCandidate[3] = obb2.orientation[0];
	separationAxisCandidate[4] = obb2.orientation[1];
	separationAxisCandidate[5] = obb2.orientation[2];

	//クロス積
	separationAxisCandidate[6] = Cross(obb1.orientation[0], obb2.orientation[0]);
	separationAxisCandidate[7] = Cross(obb1.orientation[0], obb2.orientation[1]);
	separationAxisCandidate[8] = Cross(obb1.orientation[0], obb2.orientation[2]);

	separationAxisCandidate[9] = Cross(obb1.orientation[1], obb2.orientation[0]);
	separationAxisCandidate[10] = Cross(obb1.orientation[1], obb2.orientation[1]);
	separationAxisCandidate[11] = Cross(obb1.orientation[1], obb2.orientation[2]);

	separationAxisCandidate[12] = Cross(obb1.orientation[2], obb2.orientation[0]);
	separationAxisCandidate[13] = Cross(obb1.orientation[2], obb2.orientation[1]);
	separationAxisCandidate[14] = Cross(obb1.orientation[2], obb2.orientation[2]);

	//obb1のWorldMatrix
	Matrix4x4 obb1WorldMatrix = {
		obb1.orientation[0].x,obb1.orientation[0].y,obb1.orientation[0].z,0.0f,
		obb1.orientation[1].x,obb1.orientation[1].y,obb1.orientation[1].z,0.0f,
		obb1.orientation[2].x,obb1.orientation[2].y,obb1.orientation[2].z,0.0f,
		obb1.center.x,obb1.center.y,obb1.center.z,1.0f
	};

	//obb2のWorldMatrix
	Matrix4x4 obb2WorldMatrix = {
		obb2.orientation[0].x,obb2.orientation[0].y,obb2.orientation[0].z,0.0f,
		obb2.orientation[1].x,obb2.orientation[1].y,obb2.orientation[1].z,0.0f,
		obb2.orientation[2].x,obb2.orientation[2].y,obb2.orientation[2].z,0.0f,
		obb2.center.x,obb2.center.y,obb2.center.z,1.0f
	};

	//obb1の頂点
	Vector3 vertices1[8] = {
		-obb1.size,                                //左下奥
		{-obb1.size.x,-obb1.size.y,obb1.size.z},   //左下前
		{-obb1.size.x,obb1.size.y,obb1.size.z},    //左上前
		{-obb1.size.x,obb1.size.y,-obb1.size.z},   //左上奥

		{obb1.size.x,-obb1.size.y,-obb1.size.z},   //右下奥
		{obb1.size.x,-obb1.size.y,obb1.size.z},    //右下前
		obb1.size,                                 //右上前
		{obb1.size.x,obb1.size.y,-obb1.size.z}     //右上奥
	};

	//obb2の頂点
	Vector3 vertices2[8] = {
		-obb2.size,                                //左下奥
		{-obb2.size.x,-obb2.size.y,obb2.size.z},   //左下前
		{-obb2.size.x,obb2.size.y,obb2.size.z},    //左上前
		{-obb2.size.x,obb2.size.y,-obb2.size.z},   //左上奥

		{obb2.size.x,-obb2.size.y,-obb2.size.z},   //右下奥
		{obb2.size.x,-obb2.size.y,obb2.size.z},    //右下前
		obb2.size,                                 //右上前
		{obb2.size.x,obb2.size.y,-obb2.size.z}     //右上奥
	};


	for (size_t index = 0; index < 8; index++) {
		vertices1[index] = Transform(vertices1[index], obb1WorldMatrix);
		vertices2[index] = Transform(vertices2[index], obb2WorldMatrix);
	}

	for (size_t index = 0; index < 15; index++) {

		float proj1[8]{};
		float proj2[8]{};

		//すべての頂点を軸に対して射影
		for (size_t i = 0; i < 8; i++) {
			proj1[i] = Dot(separationAxisCandidate[index], vertices1[i]);
		}
		for (size_t j = 0; j < 8; j++) {
			proj2[j] = Dot(separationAxisCandidate[index], vertices2[j]);
		}

		//射影した点の最大値と最小値を求める
		float max1, max2, min1, min2;

		max1 = std::max({ proj1[0],proj1[1] ,proj1[2] ,proj1[3] ,proj1[4] ,proj1[5] ,proj1[6] ,proj1[7] });
		min1 = std::min({ proj1[0],proj1[1] ,proj1[2] ,proj1[3] ,proj1[4] ,proj1[5] ,proj1[6] ,proj1[7] });

		max2 = std::max({ proj2[0],proj2[1] ,proj2[2] ,proj2[3] ,proj2[4] ,proj2[5] ,proj2[6] ,proj2[7] });
		min2 = std::min({ proj2[0],proj2[1] ,proj2[2] ,proj2[3] ,proj2[4] ,proj2[5] ,proj2[6] ,proj2[7] });

		//差分が形状を分離軸に射影した長さを求める
		float L1 = max1 - min1;
		float L2 = max2 - min2;

		//陰の長さの合計
		float sumSpan = L1 + L2;
		float longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

		if (sumSpan < longSpan) {
			return false;
		}
	}
	return true;
}
