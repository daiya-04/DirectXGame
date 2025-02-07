#include "Collider.h"

#include <cassert>

namespace DaiEngine {
	Vector3 Collider::GetWorldPos() const {
		return {
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2],
		};
	}
}
