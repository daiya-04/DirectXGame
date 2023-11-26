#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Sprite.h"
#include "Vec2.h"
#include "Vec3.h"

#include <memory>
#include <list>
#include <numbers>

class Enemy;

class LockOn{
private:

	std::unique_ptr<Sprite> lockOnMark_;

	const Enemy* target_ = nullptr;

	const float kDegreeToRadian_ = std::numbers::pi_v<float> / 180.0f;

	//最小距離
	float minDistance_ = 30.0f;
	//最大距離
	float maxDistance_ = 60.0f;
	//角度範囲
	float angleRange_ = 20.0f * kDegreeToRadian_;


public:

	void Initialize();

	void Update(const std::list<std::unique_ptr<Enemy>>& enemies,const ViewProjection& viewProjection);

	void Draw();

	void Search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	bool OutSideDecision(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	Vector3 GetTargetPos()const;

	bool ExistTarget() const { return target_ ? true : false; }

};

