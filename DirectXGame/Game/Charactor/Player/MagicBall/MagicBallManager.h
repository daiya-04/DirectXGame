#pragma once

#include "MagicBall.h"
#include "BasePlayerAttackManager.h"

#include <array>

class MagicBallManager : public BasePlayerAttackManager {
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
	/// <param name="shotPos"></param>
	/// <param name="direction"></param>
	void AttackStart(const Vector3& shotPos, const Vector3& direction);

private:
	//発射できる総数
	static const uint32_t kMagicBallNum_ = 5;
	//弾の配列
	std::array<std::unique_ptr<MagicBall>, kMagicBallNum_> magicBalls_;


};

