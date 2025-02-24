#pragma once
#include "ModelManager.h"
#include "Camera.h"
#include "Collider.h"
#include "Vec3.h"
#include "Object3d.h"

#include <memory>

class BaseAttack {
public:

	virtual ~BaseAttack() {}
	/// <summary>
	/// 初期化モデルあり
	/// </summary>
	/// <param name="model">モデル</param>
	virtual void Init([[maybe_unused]] const std::shared_ptr <DaiEngine::Model>& model) {}
	/// <summary>
	/// 初期化モデルなし
	/// </summary>
	virtual void Init() {}
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() {}
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	virtual void Draw([[maybe_unused]] const DaiEngine::Camera& camera) {}
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void DrawParticle([[maybe_unused]] const DaiEngine::Camera& camera) {}
	/// <summary>
	/// 衝突時
	/// </summary>
	virtual void OnCollision([[maybe_unused]] DaiEngine::Collider* other) {}

};
