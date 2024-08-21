#pragma once
#include "Icicle.h"
#include "ModelManager.h"
#include "Camera.h"

#include <array>


class IcicleManager {
public:

	static IcicleManager* GetInstanse();

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);

	void OnCollision(uint32_t index);

	void SetTarget(const WorldTransform* target);

	void AttackStart();
	void SetAttackData(const Vector3& pos, const Vector3& direction);

	bool AttackFinish() const {return (!isAttack_ && preIsAttack_);}
	bool IsAttack()const { return isAttack_; }

	bool IsLife(uint32_t index) { return icicles_[index]->IsLife(); }

	Shapes::Sphere GetCollider(uint32_t index) { return icicles_[index]->GetCollider(); }


private:
	
	std::array<std::unique_ptr<Icicle>, 4> icicles_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

};

