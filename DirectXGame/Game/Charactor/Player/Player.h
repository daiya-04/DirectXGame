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
	
	//スキニングオブジェクト
	std::unique_ptr<DaiEngine::SkinningObject> obj_;
	//スケルトン
	std::vector<DaiEngine::Skeleton> skeletons_;
	//スキンクラスター
	std::vector<DaiEngine::SkinCluster> skinClusters_;

	std::unique_ptr<IPlayerBehavior> behavior_;
	std::unique_ptr<IPlayerBehavior> behaviorRequest_;

	//ターゲット(ボス)
	const DaiEngine::WorldTransform* target_ = nullptr;
	//最大HP
	int32_t maxHp_ = 10;
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
	//攻撃
	std::vector<BasePlayerAttackManager*> attacks_;

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
	/// 衝突時
	/// </summary>
	void OnCollision(DaiEngine::Collider* other) override;
	/// <summary>
	/// 行動の切り替え
	/// </summary>
	/// <param name="behaviorName">切り替える行動の名前</param>
	void ChangeBehavior(const std::string& behaviorName) override;
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

