#pragma once
///---------------------------------------------------------------------------------------------
// 
// プレイヤー
// 攻撃などの行動を管理
// 
///---------------------------------------------------------------------------------------------

#include "FollowCamera.h"
#include "BaseCharactor.h"
#include "IPlayerBehavior.h"
#include "PlayerMagicBall.h"
#include "GroundBurst.h"

#include <memory>
#include <list>
#include <vector>
#include <array>
#include <optional>
#include <map>
#include <functional>

//プレイヤークラス
class Player : public BaseCharactor {
private: //ふるまい用メンバ変数
	//行動
	enum class Behavior {
		//通常
		kIdel,
		//攻撃
		kAttack,
		//移動(ジョギング)
		kJog,
		//ダッシュ
		kDash,
		//死亡
		kDead,
	};

public:
	
	//アクション(アニメーション)
	enum Action {
		//立ち
		Standing,
		//ジョギング
		Jogging,
		//攻撃1
		AttackCombo1,
		//攻撃2
		AttackCombo2,
		//攻撃3
		AttackCombo3,
		//死亡
		Dead,
		//加速
		Accel,
		//ノックバック
		KnockBack,

		//アクション総数
		kActionNum,
	};

private:

	std::unique_ptr<IPlayerBehavior> behavior_;
	std::unique_ptr<IPlayerBehavior> behaviorRequest_;

	//ターゲット(ボス)
	const DaiEngine::WorldTransform* target_ = nullptr;
	//最大HP
	int32_t maxHp_ = 15;
	//攻撃の射程
	float attackRange_ = 15.0f;

	//攻撃アニメーションのスピード
	float attackAnimeSpeed_ = 1.0f / 30.0f;

	Vector3 knockBackBaseDict_ = {};

	//追従カメラ
	FollowCamera* followCamera_ = nullptr;
	//デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;

	//死亡演出(アニメーション)が終わったか
	bool isFinishDeadMotion_ = false;
	
	std::array<std::unique_ptr<PlayerMagicBall>, 10> attacks_;
	uint32_t shotIndex = 0;

	std::array<std::unique_ptr<GroundBurst>, 5> groundBursts_;
	uint32_t attackIndex_ = 0;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player(){}

	~Player() override {}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルのベクター配列</param>
	void Init(const std::vector<std::shared_ptr<DaiEngine::Model>>& models) override;
	/// <summary>
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
	void Draw(const DaiEngine::Camera& camera) override;

	void DrawAttack(const DaiEngine::Camera& camera);

	void DrawParticle(const DaiEngine::Camera& camera);
	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI() override;
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision(DaiEngine::Collider* other) override;
	/// <summary>
	/// 行動の切り替え
	/// </summary>
	/// <param name="behaviorName">切り替える行動の名前</param>
	void ChangeBehavior(const std::string& behaviorName) override;
	/// <summary>
	/// 攻撃の発射
	/// </summary>
	void ShotMagicBall();
	/// <summary>
	/// 攻撃の発射2
	/// </summary>
	void AttackGroundBurst();

	//カメラの取得と設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return followCamera_; }
	//
	void SetTerget(const DaiEngine::WorldTransform* target) { target_ = target; }
	const DaiEngine::WorldTransform* GetTarget() { return target_; }
	/// <summary>
	/// 死亡アニメーションの終了
	/// </summary>
	void FinishDeadMotion() { isFinishDeadMotion_ = true; }
	/// <summary>
	/// 死亡アニメーションが終わったか
	/// </summary>
	/// <returns>終わってたらture, それ以外false</returns>
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }
	/// <summary>
	/// 現在のHPの取得
	/// </summary>
	/// <returns>現在のHP</returns>
	uint32_t GetHP() const { return hp_; }

	Vector3 GetKnockBackBaseDict() { return knockBackBaseDict_; }

};

