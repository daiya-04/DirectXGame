#include "ShapesDraw.h"
#include <cmath>
#include <numbers>
#include "Line.h"

using namespace Shapes;

namespace ShapesDraw {

	void DrawSphere(const Shapes::Sphere& sphere, const Camera& camera, const Vector4& color) {

		const uint32_t kSubdivision = 16; //分割数
		const float kLonEvery = std::numbers::pi_v<float> / kSubdivision; //経度分割1つ分の角度
		const float kLatEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision; //緯度分割1つ分の角度

		for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
			float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;
			for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
				float lon = lonIndex * kLonEvery;

				Vector3 a = { std::cosf(lat) * std::cosf(lon),std::sinf(lat),std::cosf(lat) * std::sinf(lon) };
				Vector3 b = { std::cosf(lat + kLatEvery) * std::cosf(lon),std::sinf(lat + kLatEvery),std::cosf(lat + kLatEvery) * std::sinf(lon) };
				Vector3 c = { std::cosf(lat) * std::cosf(lon + kLonEvery),std::sinf(lat),std::cosf(lat) * std::sinf(lon + kLonEvery) };

				Vector3 worldA = sphere.center + sphere.radius * a;
				Vector3 worldB = sphere.center + sphere.radius * b;
				Vector3 worldC = sphere.center + sphere.radius * c;

				Line::Draw(worldA, worldB, camera, color);
				Line::Draw(worldA, worldC, camera, color);
			}
		}
	}

	void DrawPlane(const Shapes::Plane& plane, const Camera& camera, const Vector4& color) {

		Vector3 center = plane.distance * plane.normal;
		Vector3 perpendicular[4];
		perpendicular[0] = (plane.normal.Perpendicular()).Normalize();
		perpendicular[1] = -perpendicular[0];
		perpendicular[2] = Cross(plane.normal, perpendicular[0]);
		perpendicular[3] = -perpendicular[2];

		Vector3 points[4];
		for (uint32_t index = 0; index < 4; ++index) {
			Vector3 extend = 2.0f * perpendicular[index];
			points[index] = center + extend;
		}

		Line::Draw(points[0], points[2], camera, color);
		Line::Draw(points[2], points[1], camera, color);
		Line::Draw(points[1], points[3], camera, color);
		Line::Draw(points[3], points[0], camera, color);
	}

	void DrawTriangle(const Shapes::Triangle& triangle, const Camera, const Vector4 color) {



	}

	void DrawAABB(const Shapes::AABB& aabb, const Camera& camera, const Vector4& color) {

		Vector3 points[8];
		for (size_t index = 0; index < 8; index++) {
			if (index < 4) {
				points[index] = aabb.min;
			}else {
				points[index] = aabb.max;
			}
		}

		Vector3 diff = {
			aabb.max.x - aabb.min.x,
			aabb.max.y - aabb.min.y,
			aabb.max.z - aabb.min.z
		};

		//左下奥
		points[1].z += diff.z;
		//左上奥
		points[2].y += diff.y;
		points[2].z += diff.z;
		//左上手前
		points[3].y += diff.y;

		//右上手前
		points[5].z -= diff.z;
		//右下手前
		points[6].y -= diff.y;
		points[6].z -= diff.z;
		//右下奥
		points[7].y -= diff.y;

		Line::Draw(points[0], points[1], camera, color);
		Line::Draw(points[1], points[2], camera, color);
		Line::Draw(points[2], points[3], camera, color);
		Line::Draw(points[3], points[0], camera, color);
		Line::Draw(points[4], points[5], camera, color);
		Line::Draw(points[5], points[6], camera, color);
		Line::Draw(points[6], points[7], camera, color);
		Line::Draw(points[7], points[4], camera, color);
		Line::Draw(points[0], points[6], camera, color);
		Line::Draw(points[1], points[7], camera, color);
		Line::Draw(points[2], points[4], camera, color);
		Line::Draw(points[3], points[5], camera, color);
	}

	void DrawOBB(const Shapes::OBB& obb, const Camera& camera, const Vector4& color) {

		Vector3 oriented[3];
		oriented[0] = obb.size.x * obb.orientation[0].Normalize();
		oriented[1] = obb.size.y * obb.orientation[1].Normalize();
		oriented[2] = obb.size.z * obb.orientation[2].Normalize();

		Vector3 points[8];
		//左下前
		points[0] = obb.center - oriented[0] - oriented[1] - oriented[2];
		//左下奥
		points[1] = obb.center - oriented[0] - oriented[1] + oriented[2];
		//左上奥
		points[2] = obb.center - oriented[0] + oriented[1] + oriented[2];
		//左上前
		points[3] = obb.center - oriented[0] + oriented[1] - oriented[2];
		//右上奥
		points[4] = obb.center + oriented[0] + oriented[1] + oriented[2];
		//右上前
		points[5] = obb.center + oriented[0] + oriented[1] - oriented[2];
		//右下前
		points[6] = obb.center + oriented[0] - oriented[1] - oriented[2];
		//右下奥
		points[7] = obb.center + oriented[0] - oriented[1] + oriented[2];

		Line::Draw(points[0], points[1], camera, color);
		Line::Draw(points[1], points[2], camera, color);
		Line::Draw(points[2], points[3], camera, color);
		Line::Draw(points[3], points[0], camera, color);
		Line::Draw(points[4], points[5], camera, color);
		Line::Draw(points[5], points[6], camera, color);
		Line::Draw(points[6], points[7], camera, color);
		Line::Draw(points[7], points[4], camera, color);
		Line::Draw(points[0], points[6], camera, color);
		Line::Draw(points[1], points[7], camera, color);
		Line::Draw(points[2], points[4], camera, color);
		Line::Draw(points[3], points[5], camera, color);
	}

}


