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
#include "MagicBallManager.h"
#include "GroundBurstManager.h"
#include "GPUParticle.h"
#include "Stamina.h"
#include "IPlayerState.h"

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
		//回避
		Avoid,
	};

public:
	
	//アクション(アニメーション)
	enum Action {
		//立ち
		kStanding,
		//ジョギング
		kJogging,
		//攻撃1
		kAttackCombo1,
		//攻撃2
		kAttackCombo2,
		//攻撃3
		kAttackCombo3,
		//死亡
		kDead,
		//走り
		kDash,
		//ノックバック
		kKnockBack,
		//回避
		kAvoid,

		//アクション総数
		kActionNum,
	};

private:
	
	//スキニングオブジェクト
	std::unique_ptr<DaiEngine::SkinningObject> obj_;
	//スケルトン
	std::vector<DaiEngine::Skeleton> skeletons_;
	//スキンクラスター
	std::vector<DaiEngine::SkinCluster> skinClusters_;
	//行動
	std::unique_ptr<IPlayerBehavior> behavior_;
	std::unique_ptr<IPlayerBehavior> behaviorRequest_;
	//状態
	std::unique_ptr<IPlayerState> state_;

	//ターゲット(ボス)
	const DaiEngine::WorldTransform* target_ = nullptr;
	//最大HP
	int32_t maxHp_ = 60;
	//攻撃の射程
	float attackRange_ = 20.0f;
	//スタミナ
	Stamina stamina_;
	//スタミナ最大値
	float maxStamina_ = 0.0f;
	//ダッシュ時消費スタミナの基準
	float dashStamina_ = 0.0f;
	//回避時の消費スタミナの基準
	float avoidStamina_ = 0.0f;
	//スタミナ回復量
	float healStamina_ = 0.0f;

	//攻撃アニメーションのスピード
	float attackAnimeSpeed_ = 1.0f / 30.0f;

	float baseAnimationSpeed_ = 1.0f / 60.0f;

	Vector3 knockBackBaseDict_ = {};

	//追従カメラ
	FollowCamera* followCamera_ = nullptr;
	//デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;

	//死亡演出(アニメーション)が終わったか
	bool isFinishDeadMotion_ = false;
	//攻撃
	std::vector<BasePlayerAttackManager*> attacks_;
	//速度
	float speed_ = 3.0f * kDeltaTime_;

	
	float speedRate_ = 1.0f;

	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> handEff_;
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> burnEff_;
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> chilledEff_;


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
	/// 中心座標の更新
	/// </summary>
	/// <param name="camera"></param>
	/// <returns></returns>
	void UpdateCenterPos() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera"></param>
	void DrawParticle(const DaiEngine::Camera& camera);
	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI() override;
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision(DaiEngine::Collider* other) override;

	void EnterCollision(DaiEngine::Collider* other);
	void ExitCollision(DaiEngine::Collider* other);
	/// <summary>
	/// 行動の切り替え
	/// </summary>
	/// <param name="behaviorName">切り替える行動の名前</param>
	void ChangeBehavior(const std::string& behaviorName) override;
	/// <summary>
	/// 状態の切り替え
	/// </summary>
	/// <param name="stateName"></param>
	void ChangeState(const std::string& stateName);
	/// <summary>
	/// データ設定
	/// </summary>
	/// <param name="data">オブジェクトデータ</param>
	void SetData(const LevelData::ObjectData& data) override;
	/// <summary>
	/// 攻撃の発射
	/// </summary>
	void ShotMagicBall();
	/// <summary>
	/// 攻撃の発射2
	/// </summary>
	void AttackGroundBurst();
	/// <summary>
	/// アニメーションの設定
	/// </summary>
	void SetAnimation(size_t actionIndex, bool isLoop = true) override;
	//カメラの取得と設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return followCamera_; }
	//ターゲットセット
	void SetTerget(const DaiEngine::WorldTransform* target) { target_ = target; }
	const DaiEngine::WorldTransform* GetTarget() { return target_; }
	/// <summary>
	/// 攻撃のセット
	/// </summary>
	/// <param name="attack"></param>
	void SetAttack(BasePlayerAttackManager* attack) { attacks_.push_back(attack); }
	/// <summary>
	/// 死亡アニメーションの終了
	/// </summary>
	void FinishDeadMotion() { isFinishDeadMotion_ = true; }
	/// <summary>
	/// 死亡アニメーションが終わったか
	/// </summary>
	/// <returns>終わってたらture, それ以外false</returns>
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }

	Vector3 GetKnockBackBaseDict() { return knockBackBaseDict_; }

	float GetAttackRange() const { return attackRange_; }
	/// <summary>
	/// ワールドトランスフォーム取得
	/// </summary>
	/// <returns>オブジェクトのワールドトランスフォーム</returns>
	const DaiEngine::WorldTransform& GetWorldTransform() override { return obj_->worldTransform_; }
	/// <summary>
	/// obj_の取得
	/// </summary>
	/// <returns></returns>
	DaiEngine::SkinningObject* GetObj() { return obj_.get(); }

	DaiEngine::Skeleton& GetNowSkelton() { return skeletons_[actionIndex_]; }
	/// <summary>
	/// キャラがディゾルブで消えていく
	/// </summary>
	void DissolveUpdate();
	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	void Dead();

	void SetSpeed(float speed) { speed_ = speed; }
	void SetSpeedRate(float rate) { speedRate_ = rate; }

	void StartHandEff();

	void EndHandEff();

	void HandEffPosUpdate(const std::string& side);

	void StartBurnEff();

	void EndBurnEff();

	void StartChilledEff();

	void EndChilledEff();

	Stamina& GetStamina() { return stamina_; }
	bool IsDash() { return stamina_.GetStamina() > dashStamina_; }
	bool IsAvoid() { return stamina_.GetStamina() > avoidStamina_; }
	void StaminaHeal() { stamina_.Healing(healStamina_); }

	HP& GetHP() { return hp_; }

private:
	/// <summary>
	/// 攻撃の配列から特定の型取り出し
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template <typename T>
	T* GetAttackType() {
		for (auto* attack : attacks_) {
			T* attackType = dynamic_cast<T*>(attack);
			if (attackType) {
				//見つかったら返す
				return attackType;
			}
		}
		//見つからない...
		return nullptr;
	}

};

