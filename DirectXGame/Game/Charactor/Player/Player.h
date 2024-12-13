#pragma once
///---------------------------------------------------------------------------------------------
// 
// プレイヤー
// 攻撃などの行動を管理
// 
///---------------------------------------------------------------------------------------------

#include "FollowCamera.h"
#include "BaseCharactor.h"

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
		kRoot,
		//攻撃
		kAttack,
		//ダッシュ
		kDash,
		//死亡
		kDead,
	};
	//現在の行動
	Behavior behavior_ = Behavior::kRoot;
	//次の行動リクエスト
	std::optional<Behavior> behaviorRequest_ = Behavior::kRoot;

	//行動初期化テーブル
	std::map<Behavior, std::function<void()>> behaviorInitTable_{
		{Behavior::kRoot,[this]() {RootInit(); }},
		{Behavior::kAttack,[this]() {AttackInit(); }},
		{Behavior::kDash,[this]() {DashInit(); }},
		{Behavior::kDead,[this]() {DeadInit(); }},
	};
	//行動更新テーブル
	std::map<Behavior, std::function<void()>> behaviorUpdateTable_{
		{Behavior::kRoot,[this]() {RootUpdate(); }},
		{Behavior::kAttack,[this]() {AttackUpdate(); }},
		{Behavior::kDash,[this]() {DashUpdate(); }},
		{Behavior::kDead,[this]() {DeadUpdate(); }},
	};
	

public: //ふるまい用メンバ関数

	/// <summary>
	/// 通常行動初期化
	/// </summary>
	void RootInit();
	/// <summary>
	/// 通常行動更新
	/// </summary>
	void RootUpdate();
	/// <summary>
	/// 攻撃行動初期化
	/// </summary>
	void AttackInit();
	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void AttackUpdate();
	/// <summary>
	/// ダッシュ初期化
	/// </summary>
	void DashInit();
	/// <summary>
	/// ダッシュ更新
	/// </summary>
	void DashUpdate();
	/// <summary>
	/// 死亡初期化
	/// </summary>
	void DeadInit();
	/// <summary>
	/// 死亡更新
	/// </summary>
	void DeadUpdate();

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
		//歩き
		Walking,
		//攻撃
		Attack,
		//死亡
		Dead,

		//アクション総数
		kActionNum,
	};

private:
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
	/// カメラの設定
	/// </summary>
	/// <param name="followCamera">追従カメラ</param>
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	/// <summary>
	/// ターゲット設定
	/// </summary>
	/// <param name="target">ターゲットのワールドトランスフォーム</param>
	void SetTerget(const WorldTransform* target) { target_ = target; }
	/// <summary>
	/// ゲームシーン設定
	/// </summary>
	/// <param name="gameScene">ゲームシーンのポインタ</param>
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
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

