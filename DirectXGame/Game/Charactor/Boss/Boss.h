#pragma once
///---------------------------------------------------------------------------------------------
// 
// ボス
// 攻撃などの行動を管理
// 
///---------------------------------------------------------------------------------------------


#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "SkinCluster.h"
#include "ModelManager.h"
#include "Animation.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include "LevelLoader.h"
#include "Particle.h"
#include "Sprite.h"
#include "GPUParticle.h"
#include "IcicleManager.h"
#include "PlasmaShotManager.h"
#include "ElementBallManager.h"
#include "GroundFlare.h"
#include "BaseCharactor.h"

#include <memory>
#include <vector>
#include <array>
#include <optional>
#include <functional>
#include <map>


//ボスクラス
class Boss : public BaseCharactor {
public:
	//行動
	enum Behavior {
		//通常
		kRoot,
		//攻撃
		kAttack,
		//登場
		kAppear,
		//死亡
		kDead,
	};
	//現在の行動
	Behavior behavior_ = Behavior::kAppear;
	//次の行動リクエスト
	std::optional<Behavior> behaviorRequest_ = Behavior::kAppear;

	//行動初期化テーブル
	std::map<Behavior, std::function<void()>> behaviorInitTable_{
		{Behavior::kRoot,[this]() {RootInit(); }},
		{Behavior::kAttack,[this]() {AttackInit(); }},
		{Behavior::kAppear,[this]() {AppearInit(); }},
		{Behavior::kDead,[this]() {DeadInit(); }},
	};

	//行動更新テーブル
	std::map<Behavior, std::function<void()>> behaviorUpdateTable_{
		{Behavior::kRoot,[this]() {RootUpdate(); }},
		{Behavior::kAttack,[this]() {AttackUpdate(); }},
		{Behavior::kAppear,[this]() {AppearUpdate(); }},
		{Behavior::kDead,[this]() {DeadUpdate(); }},
	};

private:
	/// <summary>
	/// 通常行動初期化
	/// </summary>
	void RootInit();
	/// <summary>
	/// 通常行動更新
	/// </summary>
	void RootUpdate();
	/// <summary>
	/// 登場行動初期化
	/// </summary>
	void AppearInit();
	/// <summary>
	/// 登場行動更新
	/// </summary>
	void AppearUpdate();
	/// <summary>
	/// 攻撃行動初期化
	/// </summary>
	void AttackInit();
	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void AttackUpdate();
	/// <summary>
	/// 死亡行動初期化
	/// </summary>
	void DeadInit();
	/// <summary>
	/// 死亡行動更新
	/// </summary>
	void DeadUpdate();

public:
	//アクション(アニメーション)
	enum Action {
		//立ち
		Standing,
		//攻撃セット(予備動作)
		AttackSet,
		//攻撃
		Attack,
		//死亡
		Dead,

		//アクション総数
		ActionNum,
	};

	//攻撃の種類
	enum AttackType {
		//火の玉
		kElementBall,
		//地面から炎
		kGroundFlare,
		//つらら
		kIcicle,
		//電気玉
		kPlasmaShot,
	};

private:
	//登場に必要なパラメータ
	struct WorkAppear{
		//開始位置
		Vector3 startPos_ = { 0.0f,-15.0f,50.0f };
		//終了位置
		Vector3 endPos_ = { 0.0f, 0.0f,50.0f };
		//時間
		float param_ = 0.0f;
		//スピード
		float speed_ = 0.005f;
	};
	//攻撃に必要なパラメータ
	struct WorkAttack {
		//攻撃間隔
		uint32_t coolTime_ = 60 * 3;
		//時間
		uint32_t param_ = 0;
	};

private:
	//登場演出
	std::map<std::string, std::unique_ptr<GPUParticle>> effect_;
	//攻撃種類
	AttackType attackType_ = AttackType::kElementBall;
	//最大HP
	uint32_t maxHp_ = 15;
	//死亡アニメーション終了フラグ
	bool isFinishDeadMotion_ = false;
	//攻撃のターゲット(プレイヤー)
	const WorldTransform* target_ = nullptr;
	//登場用のパラメータ
	WorkAppear workAppear_;
	//攻撃用のパラメータ
	WorkAttack workAttack_;

	/// 攻撃の各種マネージャ
	//火の玉
	ElementBallManager* elementBall_ = nullptr;
	//つらら
	IcicleManager* icicle_ = nullptr;
	//電気玉
	PlasmaShotManager* plasmaShot_ = nullptr;
	//地面から炎
	GroundFlare* groundFlare_ = nullptr;
	///

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Boss() {};
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルのベクター配列</param>
	void Init(const std::vector<std::shared_ptr<Model>>& models) override;
	/// <summary>/
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// UI更新
	/// </summary>
	void UpdateUI() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera) override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawParticle(const Camera& camera);
	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI() override;
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision();
	/// <summary>
	/// ターゲット設定
	/// </summary>
	/// <param name="target">ターゲットのワールドトランスフォーム</param>
	void SetTarget(const WorldTransform* target) { target_ = target; }
	/// <summary>
	/// 火の玉攻撃のセット
	/// </summary>
	/// <param name="elementBall"></param>
	void SetElementBall(ElementBallManager* elementBall) { elementBall_ = elementBall; }
	/// <summary>
	/// つらら攻撃のセット
	/// </summary>
	/// <param name="icicle"></param>
	void SetIcicle(IcicleManager* icicle) { icicle_ = icicle; }
	/// <summary>
	/// 電気玉攻撃のセット
	/// </summary>
	/// <param name="plasmaShot"></param>
	void SetPlasmaShot(PlasmaShotManager* plasmaShot) { plasmaShot_ = plasmaShot; }
	/// <summary>
	/// 地面から炎の攻撃のセット
	/// </summary>
	/// <param name="groundFlare"></param>
	void SetGroudFlare(GroundFlare* groundFlare) { groundFlare_ = groundFlare; }
	/// <summary>
	/// 攻撃しているか
	/// </summary>
	/// <returns>攻撃中ならtrue、それ以外false</returns>
	bool IsAttack() const { return (behavior_ == Behavior::kAttack) ? true : false; }
	/// <summary>
	/// 死亡アニメーションが終わったか
	/// </summary>
	/// <returns>死亡アニメーションが終わっていたらtrue、それ以外false</returns>
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }

};

