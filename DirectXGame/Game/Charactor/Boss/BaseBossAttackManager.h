#pragma once
#include "Camera.h"
#include "Vec3.h"

class BaseBossAttackManager {
public:

	virtual ~BaseBossAttackManager() = default;
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画
	/// </summary>
	/// 使わない場合があるかもだから空実装
	/// <param name="camera"></param>
	virtual void Draw([[maybe_unused]] const DaiEngine::Camera& camera) {}
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// 使わない場合があるかもだから空実装
	/// <param name="camera"></param>
	virtual void DrawParticle([[maybe_unused]] const DaiEngine::Camera& camera) {}
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target"></param>
	virtual void SetTarget([[maybe_unused]] const Vector3* target) {}

};
