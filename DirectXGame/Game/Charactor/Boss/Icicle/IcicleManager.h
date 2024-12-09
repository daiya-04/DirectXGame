#pragma once
#include "Icicle.h"
#include "ModelManager.h"
#include "Camera.h"
#include "IceScar.h"

#include <array>


class IcicleManager {
public:
	//初期化
	void Init(const std::shared_ptr<Model>& model);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	void DrawScar(const Camera& camera);
	//衝突時
	void OnCollision(uint32_t index);

	void SetTarget(const WorldTransform* target);

	void AttackStart();
	void SetAttackData(const Vector3& pos, const Vector3& direction);

	bool AttackFinish() const {return (!isAttack_ && preIsAttack_);}
	bool IsAttack()const { return isAttack_; }

	bool IsLife(uint32_t index) { return icicles_[index]->IsLife(); }

	Shapes::Sphere GetCollider(uint32_t index) { return icicles_[index]->GetCollider(); }

	uint32_t GetIcicleCount() const { return icicleNum_; }

private:
	//つららの数
	static const uint32_t icicleNum_ = 4;
	
	std::array<std::unique_ptr<Icicle>, icicleNum_> icicles_;
	std::array<std::unique_ptr<IceScar>, icicleNum_> iceScars_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

};
