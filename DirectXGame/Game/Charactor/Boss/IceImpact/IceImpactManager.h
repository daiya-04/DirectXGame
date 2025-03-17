#pragma once
#include "BaseBossAttackManager.h"
#include "IceImpact.h"
#include "IceScar.h"


class IceImpactManager : public BaseBossAttackManager {
public:

	~IceImpactManager() override {}

	void Init() override;

	void Update() override;

	void DrawParticle(const DaiEngine::Camera& camera)override;

	void DrawScar(const DaiEngine::Camera& camera);

	void AttackStart();

	void SetAttackData(const Vector3& pos);

	/// <summary>
	/// 攻撃が終了したか
	/// </summary>
	/// <returns></returns>
	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }

private:

	std::unique_ptr<IceImpact> iceImpact_;

	//氷の跡
	std::unique_ptr<IceScar> iceScar_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;


};

