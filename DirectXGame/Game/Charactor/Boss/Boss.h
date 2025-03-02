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
#include "GroundFlareManager.h"
#include "BaseCharactor.h"
#include "IBossBehavior.h"
#include "BaseBossAttackManager.h"

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
		kIdel,
		//攻撃
		kAttack,
		//登場
		kAppear,
		//死亡
		kDead,
	};

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
		//移動
		Move,

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

	//オブジェクト
	std::unique_ptr<DaiEngine::Object3d> obj_;

	std::unique_ptr<IBossBehavior> behavior_;
	std::unique_ptr<IBossBehavior> behaviorRequest_;

	//登場演出
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> effect_;
	//攻撃種類
	AttackType attackType_ = AttackType::kElementBall;
	//最大HP
	uint32_t maxHp_ = 30;
	//死亡アニメーション終了フラグ
	bool isFinishDeadMotion_ = false;
	//攻撃のターゲット(プレイヤー)
	const DaiEngine::WorldTransform* target_ = nullptr;

	/// 攻撃の各種マネージャ
	std::vector<BaseBossAttackManager*> attacks_;
	///

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Boss() {};

	~Boss() override {}
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルのベクター配列</param>
	void Init(const std::vector<std::shared_ptr<DaiEngine::Model>>& models) override;
	/// <summary>/
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// UI更新
	/// </summary>
	void UpdateUI() override;
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
	/// <param name="camera">カメラ</param>
	void DrawParticle(const DaiEngine::Camera& camera);
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision(DaiEngine::Collider* other) override;
	/// <summary>
	/// データ設定
	/// </summary>
	/// <param name="data">オブジェクトデータ</param>
	void SetData(const LevelData::ObjectData& data) override;
	
	const DaiEngine::WorldTransform* GetTarget() { return target_; }
	void SetTarget(const DaiEngine::WorldTransform* target) { target_ = target; }
	
	void SetAttack(BaseBossAttackManager* attack) { attacks_.push_back(attack); }
	std::vector<BaseBossAttackManager*>& GetAttacks() { return attacks_; }
	/// <summary>
	/// 死亡アニメーションの終了
	/// </summary>
	void FinishDeadMotion() { isFinishDeadMotion_ = true; }
	/// <summary>
	/// 死亡アニメーションが終わったか
	/// </summary>
	/// <returns>死亡アニメーションが終わっていたらtrue、それ以外false</returns>
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }
	/// <summary>
	/// 行動の切り替え
	/// </summary>
	/// <param name="behaviorName">切り替える行動の名前</param>
	void ChangeBehavior(const std::string& behaviorName) override;
	/// <summary>
	/// 登場演出開始
	/// </summary>
	void AppearEffectStart();
	/// <summary>
	/// 登場演出終了
	/// </summary>
	void AppearEffectEnd();
	//攻撃の種類の取得と設定
	AttackType GetAttackType() { return attackType_; }
	void SetAttackType(const AttackType& attackType) { attackType_ = attackType; }
	/// <summary>
	/// ワールドトランスフォーム取得
	/// </summary>
	/// <returns>オブジェクトのワールドトランスフォーム</returns>
	const DaiEngine::WorldTransform& GetWorldTransform() override { return obj_->worldTransform_; }
	/// <summary>
	/// obj_の取得
	/// </summary>
	/// <returns></returns>
	DaiEngine::Object3d* GetObj() { return obj_.get(); }
	/// <summary>
	/// ターゲットの方に向く
	/// </summary>
	void LookAtTarget();

	/// <summary>
	/// 攻撃配列から特定の型取り出し
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

