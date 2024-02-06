#pragma once
#include "Vec3.h"
#include "WorldTransform.h"
#include "Quaternion.h"
#include "Object3d.h"
#include <vector>
#include "ImGuiManager.h"

class Signpost {
public:

	void Init(std::vector<Object3d*> models);
	void Update();
	void Draw(const Camera& camera);

	void SetStert(Vector3 pos) { posStert = pos; }
	void SetEnd(Vector3 pos) { posEnd = pos; }

private:
	Vector3 OffsetCalc();

	Vector3 posStert;
	Vector3 posEnd;

	WorldTransform world_;
	Quaternion forTargetQua;

	std::vector<Object3d*> models_;
};