#pragma once
#include "ElementBall.h"
#include "BurnScar.h"
#include "ModelManager.h"
#include "GPUParticle.h"
#include <Camera.h>

#include <array>

class ElementBallManager {
public:

	static ElementBallManager* GetInstance();

	void Init(const std::shared_ptr<Model>& model, uint32_t tex);

	void Update();

	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	void DrawBurnScars(const Camera& camera);

	void OnCollision(uint32_t index);

	void SetTartget(const WorldTransform* target);

	void SetAttackData(const Vector3& pos);

	void AttackStart();
	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }

	bool IsAttack() const { return isAttack_; }
	bool ShotStart() const { return (!isShot_ && preIsShot_); }

	bool IsLife(uint32_t index)const { return elementBalls_[index]->IsLife(); }

	Shapes::Sphere GetCollider(uint32_t index) { return elementBalls_[index]->GetCollider(); }

	
	

private:

	std::array<std::unique_ptr<ElementBall>, 4> elementBalls_;
	std::array<std::unique_ptr<BurnScar>, 4> burnScareses_;

	std::array<std::unique_ptr<GPUParticle>, 4> fireFields_;
	std::array<std::unique_ptr<GPUParticle>, 4> splashes_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	bool isShot_ = false;
	bool preIsShot_ = false;

};

