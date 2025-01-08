#include "Collider.h"

#include <cassert>


int32_t Collider::GetIntValue(const std::string& name) {

	assert(items_.find(name) != items_.end());

	return std::get<int32_t>(items_.find(name)->second.value);
}

float Collider::GetFloatValue(const std::string& name) {

	assert(items_.find(name) != items_.end());

	return std::get<float>(items_.find(name)->second.value);
}

Vector3 Collider::GetVec3Value(const std::string& name) {

	assert(items_.find(name) != items_.end());

	return std::get<Vector3>(items_.find(name)->second.value);
}

Vector3 Collider::GetWorldPos() const {
	/*Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;*/

	return {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2],
	};
}
