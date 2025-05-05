#pragma once

#include "MagicCannon.h"
#include "BasePlayerAttackManager.h"

#include <array>

class MagicCannonManager : public BasePlayerAttackManager {
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
	/// <param name="startPos"></param>
	/// <param name="direction"></param>
	void AttackStart(const Vector3& startPos, const Vector3& direction);

private:
	//発射できる最大数
	static const uint32_t kMagicCannonNum_ = 3;
	//弾の配列
	std::array<std::unique_ptr<MagicCannon>, kMagicCannonNum_> magicCannons_;

};

