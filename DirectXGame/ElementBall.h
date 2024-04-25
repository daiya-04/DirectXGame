#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include <memory>
#include <optional>


class ElementBall {
private:

	enum class Phase {
		kSet,
		kCharge,
		kShot,
	};

	Phase phase_ = Phase::kSet;
	std::optional<Phase> phaseRequest_ = std::nullopt;

private:

	void SetInit();
	void SetUpdate();
	void ChargeInit();
	void ChargeUpdate();
	void ShotInit();
	void ShotUpdate();

private:

	struct WorkSet {
		Vector3 start{};
		Vector3 end{};
		float param = 0.0f;
	};

	struct WorkCharge {
		uint32_t coolTime = 60;
		uint32_t param = 0;
	};

	struct WorkShot {
		bool isTrack = true;
		const float trackingDist = 25;
		Vector3 move{};
	};

private:

	static const WorldTransform* target_;

	std::unique_ptr<Object3d> obj_;
	Vector3 size_ = { 2.0f,2.0f,2.0f };

	bool isLife_ = false;

	WorkSet workSet_;
	WorkCharge workCharge_;
	WorkShot workShot_;

public:

	void Init(std::shared_ptr<Model> model,const Vector3& startPos);

	void Update();

	void Draw(const Camera& camera);

	static void SetTarget(const WorldTransform* target) { target_ = target; }

	void SetShotCount(uint32_t seconds) { workCharge_.coolTime = 60 * seconds; }

	bool IsLife() const { return isLife_; }

};

