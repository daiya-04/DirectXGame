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
	//ダッシュに必要なパラメータ
	struct WorkDash {
		//ダッシュ中の現在の時間
		uint32_t dashParam_ = 0;
		//ダッシュ方向
		Vector3 dashDirection_{};
		//ダッシュの時間
		uint32_t dashTime_ = 10;
	};
	//攻撃に必要なパラメータ
	struct WorkAttack {
		//攻撃中の現在の時間
		uint32_t attackParam_ = 0;
		//現在のコンボ数(何段目か)
		uint32_t comboIndex_ = 0;
		//一段の中のどのフェーズか
		uint32_t InComboPhase_ = 0;
		//コンボが続くか
		bool comboNext_ = false;
		//攻撃の速さ
		float speed_ = 0.0f;
		//攻撃の速度
		Vector3 velocity_{};
	};
	//コンボに必要なパラメータ
	struct ComboAttack {
		//チャージの時間
		uint32_t chargeTime_;
		//攻撃(パーティクルの生存)時間
		uint32_t attackTime_;
		//攻撃後の硬直時間
		uint32_t recoveryTime_;
	};

	enum class ComboPhase {
		//溜め
		Charge,
		//攻撃
		Attack,
		//硬直
		Recovery,
	};

	enum class ComboIndex {
		//1コンボ目
		First,
		//2コンボ目
		Second,
		//3コンボ目
		Third,
	};

	//コンボ数
	static const int comboNum_ = 3;
	static const std::array<ComboAttack, comboNum_> kComboAttacks_;

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

	//ダッシュ用パラメータ
	WorkDash workDash_;
	//攻撃用パラメータ
	WorkAttack workAttack_;
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
	void OnCollision();
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

};

