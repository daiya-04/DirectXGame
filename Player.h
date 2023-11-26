#pragma once
#include <memory>
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Vec3.h"
#include "Input.h"
#include <vector>
#include <array>
#include <optional>
#include "FollowCamera.h"

class LockOn;

class Player{
private: //振る舞い用メンバ変数

	enum class Behavior {
		kRoot,
		kAttack,
		kDash,
	};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	static void(Player:: *BehaviorTable[])();

public: //振る舞い用メンバ関数

	//通常行動初期化
	void RootInitialize();
	//通常行動更新
	void RootUpdate();
	//攻撃行動初期化
	void AttackInitialize();
	//攻撃行動更新
	void AttackUpdate();
	//ダッシュ初期化
	void DashInitialize();
	//ダッシュ更新
	void DashUpdate();

public:

	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
		Vector3 dashDirection_{};
		uint32_t dashTime_ = 0;
	};

	struct WorkAttack {
		float swingParam_ = 0.0f;
		uint32_t attackParameter_ = 0;
		float theta_ = 0.0f;
		bool isAttack_ = false;
		uint32_t comboIndex_ = 0;
		uint32_t inComboPhase_ = 0;
		bool comboNext_ = false;
	};

	struct ConstAttack {
		//振りかぶりの時間
		uint32_t anticipationTime_;
		//溜めの時間
		uint32_t chargeTime_;
		//攻撃振りの時間
		uint32_t swingTime_;
		//硬直時間
		uint32_t recoveryTime_;
		//振りかぶりの移動速さ
		float anticipationSpeed_;
		//溜めの移動速さ
		float chargeSpeed_;
		//攻撃振りの移動速さ
		float swingSpeed_;
	};

	static const int comboNum_ = 3;
	static const std::array<ConstAttack, comboNum_> kConstAttacks_;

	enum Parts {
		Body,
		Head,

		partsNum,
	};

private:

	std::vector<std::unique_ptr<Object3d>> objects_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, partsNum> partsWorldTransform_;
	WorldTransform weaponWorldTransform_;
	WorldTransform weaponCollision_;
	Vector3 size_ = { 1.0f,2.0f,1.0f };

	float speed = 0.5f;
	Vector3 rotate_ = {};
	Matrix4x4 rotateMat{};
	Vector3 velocity_ = {};

	bool isJamp_ = false;
	bool onGround_ = false;

	WorkDash workDash_;
	WorkAttack workAttack_;

	FollowCamera* followCamera_ = nullptr;
	const ViewProjection* viewProjection_;
	const LockOn* lockOn_ = nullptr;
	

public:

	void Initialize(std::vector<uint32_t> modelHandles);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void OnGround();

	void ReStart();

	void ApplyGlobalVariables();

	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
	void SetTranslationParent(const WorldTransform* parent) { worldTransform_.translationParent_ = parent; }

	void AddTransform(const Vector3& velocity) { worldTransform_.translation_ += velocity; }

	Vector3 GetWorldPos() const;
	Vector3 GetWeaponWorldPos() const;
	Vector3 GetSize() const { return size_; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	bool IsAttack() { return workAttack_.isAttack_; }

};

