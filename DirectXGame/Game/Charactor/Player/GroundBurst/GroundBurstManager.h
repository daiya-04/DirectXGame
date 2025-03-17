#pragma once
#include "GroundBurst.h"
#include "BasePlayerAttackManager.h"

#include <array>

class GroundBurstManager : public BasePlayerAttackManager {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera"></param>
	void DrawParticle(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// 攻撃開始
	/// </summary>
	/// <param name="attackPos"></param>
	void AttackStart(const Vector3& attackPos);

private:
	//地面からバーン出来る総数
	static const uint32_t kGroundBurstNum_ = 2;
	//攻撃の配列
	std::array<std::unique_ptr<GroundBurst>, kGroundBurstNum_> groundBursts_;

};

