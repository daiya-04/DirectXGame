#pragma once
#include "ElementBall.h"
#include "BurnScar.h"
#include "ModelManager.h"
#include "GPUParticle.h"
#include <Camera.h>

#include <array>

class ElementBallManager {
public:
	//初期化
	void Init(const std::shared_ptr<Model>& model, uint32_t tex);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	void DrawBurnScars(const Camera& camera);
	//衝突時
	void OnCollision(uint32_t index);

	void SetTartget(const WorldTransform* target);
	//データセット
	void SetAttackData(const Vector3& pos);

	void AttackStart();
	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }

	bool IsAttack() const { return isAttack_; }
	bool ShotStart() const { return (!isShot_ && preIsShot_); }

	bool IsLife(uint32_t index)const { return elementBalls_[index]->IsLife(); }

	Shapes::Sphere GetCollider(uint32_t index) { return elementBalls_[index]->GetCollider(); }

	uint32_t GetElementballCount() const { return elementBallNum_; }
	

private:
	//玉の数
	static const uint32_t elementBallNum_ = 4;

	std::array<std::unique_ptr<ElementBall>, elementBallNum_> elementBalls_;
	std::array<std::unique_ptr<BurnScar>, elementBallNum_> burnScareses_;

	std::array<std::unique_ptr<GPUParticle>, elementBallNum_> fireFields_;
	std::array<std::unique_ptr<GPUParticle>, elementBallNum_> splashes_;
	std::array<std::unique_ptr<GPUParticle>, elementBallNum_> fireSparks_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	bool isShot_ = false;
	bool preIsShot_ = false;

};

