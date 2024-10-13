#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "Animation.h"
#include "SkinCluster.h"
#include "Vec3.h"
#include "Matrix44.h"
#include "CollisionShapes.h"
#include "LevelLoader.h"
#include "FollowCamera.h"
#include "Particle.h"
#include "Sprite.h"
#include "GPUParticle.h"

#include <memory>
#include <list>
#include <vector>
#include <array>
#include <optional>
#include <map>
#include <functional>

class GameScene;

class Player{
private: //ふるまい用メンバ変数

	enum class Behavior {
		kRoot,
		kAttack,
		kDash,
		kDead,
	};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	std::map<Behavior, std::function<void()>> behaviorInitTable_{
		{Behavior::kRoot,[this]() {RootInit(); }},
		{Behavior::kAttack,[this]() {AttackInit(); }},
		{Behavior::kDash,[this]() {DashInit(); }},
		{Behavior::kDead,[this]() {DeadInit(); }},
	};

	std::map<Behavior, std::function<void()>> behaviorUpdateTable_{
		{Behavior::kRoot,[this]() {RootUpdate(); }},
		{Behavior::kAttack,[this]() {AttackUpdate(); }},
		{Behavior::kDash,[this]() {DashUpdate(); }},
		{Behavior::kDead,[this]() {DeadUpdate(); }},
	};
	

public: //ふるまい用メンバ関数

	//通常行動初期化
	void RootInit();
	//通常行動更新
	void RootUpdate();
	//攻撃行動初期化
	void AttackInit();
	//攻撃行動更新
	void AttackUpdate();
	//ダッシュ初期化
	void DashInit();
	//ダッシュ更新
	void DashUpdate();
	//死亡初期化
	void DeadInit();
	//死亡更新
	void DeadUpdate();

public:

	struct WorkDash {
		uint32_t dashParam_ = 0;
		Vector3 dashDirection_{};
		uint32_t dashTime_ = 10;
	};

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

	struct ComboAttack {
		//チャージの時間
		uint32_t chargeTime_;
		//攻撃(パーティクルの生存)時間
		uint32_t attackTime_;
		//攻撃後の硬直時間
		uint32_t recoveryTime_;
	};

	static const int comboNum_ = 3;
	static const std::array<ComboAttack, comboNum_> kComboAttacks_;

	enum Action {
		Standing,
		Walking,
		Attack,
		Dead,

		kActionNum,
	};

private:

	const WorldTransform* target_ = nullptr;

	std::unique_ptr<SkinningObject> obj_;
	std::vector<std::shared_ptr<Model>> animationModels_;
	std::vector<Animation> animations_;
	std::vector<Skeleton> skeletons_;
	std::vector<SkinCluster> skinClusters_;

	Action action_ = Action::Standing;

	Vector3 size_{};

	Shapes::AABB collider_{};

	int32_t maxHp_ = 10;
	int32_t life_ = maxHp_;

	float attackRange_ = 20.0f;

	Matrix4x4 rotateMat_ = MakeIdentity44();
	Vector3 rotate_ = { 0.0f,0.0f,1.0f };
	Vector3 from_ = { 0.0f,0.0f,1.0f };
	//Vector3 velocity_ = {};

	WorkDash workDash_;
	WorkAttack workAttack_;

	FollowCamera* followCamera_ = nullptr;

	const float kDeltaTime_ = 1.0f / 60.0f;

	bool isAttack_ = false;
	bool isDead_ = false;
	bool isFinishDeadMotion_ = false;

	GameScene* gameScene_ = nullptr;

	uint32_t hpBerTex_ = 0;
	std::unique_ptr<Sprite> hpBer_;

	//std::unique_ptr<GPUParticle> attackEff_;

public:

	Player(){}

	//初期化
	void Init(std::vector<std::shared_ptr<Model>> models);
	//更新
	void Update();
	//Collider更新
	void ColliderUpdate() {
		collider_.max = GetWorldPos() + size_;
		collider_.min = GetWorldPos() - size_;
	}
	void UIUpdate();
	//描画
	void Draw(const Camera& camera);
	//パーティクル描画
	void DrawParticle(const Camera& camera);

	void DrawUI();

	void SetData(const LevelData::ObjectData& data);

	void OnCollision();

	//カメラの設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }

	void SetTerget(const WorldTransform* target) { target_ = target; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	const WorldTransform& GetWorldTransform() { return obj_->worldTransform_; }
	Vector3 GetWorldPos() const;
	bool IsAttack() const { return isAttack_; }
	bool IsDead() const { return isDead_; }
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }
	Shapes::AABB GetCollider() const { return collider_; }
	uint32_t GetLife() const { return life_; }

};

