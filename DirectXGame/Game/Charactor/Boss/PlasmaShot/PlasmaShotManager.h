#pragma once
#include "PlasmaShot.h"
#include "ModelManager.h"
#include "Camera.h"

#include <array>

class PlasmaShotManager {
public:
	//初期化
	void Init(const std::shared_ptr<Model>& model);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	//衝突時
	void OnCollision(uint32_t index);

	void SetTarget(const WorldTransform* target);

	void AttackStart();
	void SetAttackData(const Vector3& pos);

	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }
	bool IsAttack()const { return isAttack_; }

	bool IsLife(uint32_t index) const { return plasmaShots_[index]->IsLife(); }

	Shapes::Sphere GetCollider(uint32_t index) { return plasmaShots_[index]->GetCollider(); }

	uint32_t GetPlasmaShotCount() const { return plasmaShotNum_; }

private:
	//電気玉の数
	static const uint32_t plasmaShotNum_ = 3;

	std::array<std::unique_ptr<PlasmaShot>, plasmaShotNum_> plasmaShots_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

};

