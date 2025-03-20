#pragma once
///---------------------------------------------------------------------------------------------
//
//　キャラクターの基底クラス
// スキニングアニメーションを使うキャラクターはこれを継承して使う
//
///---------------------------------------------------------------------------------------------

#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "Animation.h"
#include "SkinCluster.h"
#include "LevelLoader.h"
#include "CollisionShapes.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Matrix44.h"
#include "Sprite.h"
#include "OBBCollider.h"
#include "HP.h"

#include <memory>
#include <vector>
#include <string>

//キャラクターの基底クラス
class BaseCharactor {
public:

	virtual ~BaseCharactor();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルのベクター配列</param>
	virtual void Init(const std::vector<std::shared_ptr<DaiEngine::Model>>& models);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 中心座標の更新
	/// </summary>
	/// <param name="camera"></param>
	/// <returns></returns>
	virtual void UpdateCenterPos() {}

	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(const DaiEngine::Camera& camera);
	/// <summary>
	/// UI描画
	/// </summary>
	virtual void DrawUI();

	virtual void OnCollision([[maybe_unused]] DaiEngine::Collider* other) {};
	/// <summary>
	/// データ設定
	/// </summary>
	/// <param name="data">オブジェクトデータ</param>
	virtual void SetData(const LevelData::ObjectData& data);
	/// <summary>
	/// キャラの中心位置取得
	/// </summary>
	/// <returns>キャラの中心座標</returns>
	const Vector3& GetCenterPos() const { return centerPos_; }
	/// <summary>
	/// 死亡しているか
	/// </summary>
	/// <returns>死んでいたらtrue、生きていればfalse</returns>
	bool IsDead() const { return isDead_; }
	/// <summary>
	/// ワールドトランスフォーム取得
	/// </summary>
	/// <returns>オブジェクトのワールドトランスフォーム</returns>
	virtual const DaiEngine::WorldTransform& GetWorldTransform() = 0;
	/// <summary>
	/// アニメーションの設定
	/// </summary>
	virtual void SetAnimation(size_t actionIndex, bool isLoop = true);
	//キャラの向きの取得と設定
	const Vector3 GetDirection() const { return direction_; }
	void SetDirection(const Vector3& direction);

	const Matrix4x4 GetRotateMat() const { return rotateMat_; }
	/// <summary>
	/// 行動の切り替え
	/// </summary>
	/// <param name="behaviorName">切り替える行動の名前</param>
	virtual void ChangeBehavior(const std::string& behaviorName) = 0;
	
	/// <summary>
	/// 現在のアニメーションの取得
	/// </summary>
	/// <returns></returns>
	DaiEngine::Animation& GetNowAnimation() { return animations_[actionIndex_]; }
	const size_t GetActionIndex() const { return actionIndex_; }

	DaiEngine::Collider* GetCollider() { return collider_.get(); }

protected:

	//アニメーションモデルデータ
	std::vector<std::shared_ptr<DaiEngine::Model>> animationModels_;
	//アニメーション再生
	std::vector<DaiEngine::Animation> animations_;

	//アクションインデックス(今なんのアニメーションか)
	size_t actionIndex_ = 0;
	//コライダー
	std::unique_ptr<DaiEngine::OBBCollider> collider_;
	Vector3 centerPos_ = {};
	//回転行列
	Matrix4x4 rotateMat_;
	//向いている方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };

	////HP
	//int32_t hp_;
	////HP用UI
	//std::unique_ptr<DaiEngine::Sprite> hpBar_;
	////HP用UIサイズ
	//Vector2 hpBarSize_ = {};

	HP hp_;

	//死亡フラグ
	bool isDead_ = false;

};

