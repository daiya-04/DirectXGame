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

#include <memory>
#include <list>
#include <vector>
#include <array>
#include <optional>
#include <map>
#include <functional>
//ゲームシーン前方宣言
class GameScene;

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
		//攻撃
		Attack,
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
	const WorldTransform* target_ = nullptr;
	//最大HP
	int32_t maxHp_ = 20;
	//攻撃の射程
	float attackRange_ = 20.0f;

	//攻撃アニメーションのスピード
	float attackAnimeSpeed_ = 1.0f / 30.0f;

	Vector3 knockBackBaseDict_ = {};

	//追従カメラ
	FollowCamera* followCamera_ = nullptr;
	//デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;

	//死亡演出(アニメーション)が終わったか
	bool isFinishDeadMotion_ = false;
	//ゲームシーン
	GameScene* gameScene_ = nullptr;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player(){}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルのベクター配列</param>
	void Init(const std::vector<std::shared_ptr<Model>>& models) override;
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
	void Draw(const Camera& camera) override;
	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI() override;
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision(const Vector3& hitPos);
	/// <summary>
	/// 行動の切り替え
	/// </summary>
	/// <param name="behaviorName">切り替える行動の名前</param>
	void ChangeBehavior(const std::string& behaviorName) override;
	/// <summary>
	/// 攻撃の発射
	/// </summary>
	void ShotMagicBall();

	//カメラの取得と設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return followCamera_; }
	//
	void SetTerget(const WorldTransform* target) { target_ = target; }
	const WorldTransform* GetTarget() { return target_; }
	/// <summary>
	/// ゲームシーン設定
	/// </summary>
	/// <param name="gameScene">ゲームシーンのポインタ</param>
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
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

