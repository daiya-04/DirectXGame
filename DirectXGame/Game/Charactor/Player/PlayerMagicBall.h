#pragma once
///---------------------------------------------------------------------------------------------
// 
// プレイヤーの攻撃クラス
// 
///---------------------------------------------------------------------------------------------

#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include "GPUParticle.h"
#include <memory>


//プレイヤー攻撃クラス
class PlayerMagicBall {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="startPos">開始位置座標</param>
	/// <param name="direction">発射方向</param>
	void Init(std::shared_ptr<Model> model, const Vector3& startPos, const Vector3& direction);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera);
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawParticle(const Camera& camera);
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision();
	/// <summary>
	/// /存在しているか
	/// </summary>
	/// <returns>存在していればtrue、それ以外はfalse</returns>
	bool IsLife() const { return isLife_; }
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	Shapes::Sphere GetCollider() const { return collider_; }
	/// <summary>
	/// ワールド座標取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos() const;

private:
	//オブジェクト
	std::unique_ptr<Object3d> obj_;
	//衝突判定用
	Shapes::Sphere collider_;
	//スピード
	float speed_ = 0.5f;
	//速度
	Vector3 velocity_{};
	//生存フラグ
	bool isLife_ = true;
	//エフェクト
	std::unique_ptr<GPUParticle> particle_;

	//攻撃開始地点
	Vector3 startPos_{};

	//射程
	float firingRange_ = 20.0f;

};

