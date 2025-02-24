#pragma once
#include "ModelManager.h"
#include "Camera.h"
#include "Collider.h"
#include "Vec3.h"
#include "Object3d.h"

#include <memory>

class BaseAttack {
public:

	virtual ~BaseAttack() = default;
	/// <summary>
	/// 初期化モデルあり
	/// </summary>
	/// <param name="model">モデル</param>
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
	/// <param name="camera">カメラ</param>
	virtual void DrawParticle([[maybe_unused]] const DaiEngine::Camera& camera) {}
	/// <summary>
	/// 衝突時
	/// </summary>
	virtual void OnCollision(DaiEngine::Collider* other) = 0;

};
