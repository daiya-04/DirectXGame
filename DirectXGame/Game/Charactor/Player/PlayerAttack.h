#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include "GPUParticle.h"
#include <memory>

class PlayerAttack {
public:
	//初期化
	void Init(std::shared_ptr<Model> model, const Vector3& startPos, const Vector3& direction);
	//更新
	void Update();
	//モデル描画
	void Draw(const Camera& camera);
	//パーティクル描画
	void DrawParticle(const Camera& camera);
	//衝突時
	void OnCollision();
	
	bool IsLife() const { return isLife_; }

	Shapes::Sphere GetCollider() const { return collider_; }

	Vector3 GetWorldPos() const;

private:

	std::unique_ptr<Object3d> obj_;
	//衝突判定用
	Shapes::Sphere collider_;

	float speed_ = 0.5f;
	Vector3 velocity_{};
	//生存フラグ
	bool isLife_ = true;

	std::unique_ptr<GPUParticle> particle_;

	//攻撃開始地点
	Vector3 startPos_{};

	//射程
	float firingRange_ = 20.0f;

};

