#pragma once
#include "PlasmaShot.h"
#include "ModelManager.h"
#include "Camera.h"

#include <array>

class PlasmaShotManager {
public:

	static PlasmaShotManager* GetInstance();

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);

	void OnCollision(uint32_t index);

	void SetTarget(const WorldTransform* target);

	void AttackStart();
	void SetAttackData(const Vector3& pos);

	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }
	bool IsAttack()const { return isAttack_; }

	bool IsLife(uint32_t index) const { return plasmaShots_[index]->IsLife(); }

	Shapes::Sphere GetCollider(uint32_t index) { return plasmaShots_[index]->GetCollider(); }

private:

	std::array<std::unique_ptr<PlasmaShot>,3> plasmaShots_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

};

