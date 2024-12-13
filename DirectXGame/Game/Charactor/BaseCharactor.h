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

#include <memory>
#include <vector>

//キャラクターの基底クラス
class BaseCharactor {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルのベクター配列</param>
	virtual void Init(const std::vector<std::shared_ptr<Model>>& models);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// UI更新
	/// </summary>
	virtual void UpdateUI() {};
	/// <summary>
	/// コライダー更新
	/// </summary>
	void UpdateCollider();

	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(const Camera& camera);
	/// <summary>
	/// UI描画
	/// </summary>
	virtual void DrawUI();

	/// <summary>
	/// データ設定
	/// </summary>
	/// <param name="data">オブジェクトデータ</param>
	void SetData(const LevelData::ObjectData& data);

	/// <summary>
	/// キャラの中心位置取得
	/// </summary>
	/// <returns>キャラの中心座標</returns>
	Vector3 GetCenterPos() const;
	/// <summary>
	/// 死亡しているか
	/// </summary>
	/// <returns>死んでいたらtrue、生きていればfalse</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	Shapes::OBB GetCollider() const { return collider_; }
	/// <summary>
	/// ワールドトランスフォーム取得
	/// </summary>
	/// <returns>オブジェクトのワールドトランスフォーム</returns>
	const WorldTransform& GetWorldTransform() { return obj_->worldTransform_; }

protected:

	///スキニングアニメーションに必要なもの
	//スキニングオブジェクト
	std::unique_ptr<SkinningObject> obj_;
	//スキニングアニメーションデータ
	std::vector<std::shared_ptr<Model>> animationModels_;
	//アニメーション再生
	std::vector<Animation> animations_;
	//スケルトン
	std::vector<Skeleton> skeletons_;
	//スキンクラスター
	std::vector<SkinCluster> skinClusters_;

	//アクションインデックス(今なんのアニメーションか)
	int32_t actionIndex_ = 0;
	//コライダー(形状)
	Shapes::OBB collider_;

	//回転行列
	Matrix4x4 rotateMat_;
	//向いている方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };

	//HP
	int32_t hp_;
	//HP用UI
	std::unique_ptr<Sprite> hpBar_;
	//HP用UIサイズ
	Vector2 hpBarSize_ = {};

	//死亡フラグ
	bool isDead_ = false;

};

