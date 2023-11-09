#pragma once
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Vec3.h"
#include "Input.h"
#include <vector>
#include <array>
#include <optional>


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

private:

	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
		Vector3 dashDirection_{};
	};

	struct WorkAttack {
		uint32_t coolTime_ = 0;
		float attackParameter_ = 0.0f;
	};

	enum Parts {
		Body,
		Head,

		partsNum,
	};

	std::vector<Object3d*> models_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, partsNum> partsWorldTransform_;
	WorldTransform weaponWorldTransform_;
	Vector3 size_ = { 1.0f,2.0f,1.0f };

	float speed = 0.5f;
	Vector3 rotate_ = {};
	Vector3 velocity_ = {};

	bool isJamp_ = false;
	bool onGround_ = false;

	WorkDash workDash_;
	WorkAttack workAttack_;

	const ViewProjection* viewProjection_;

public:

	void Initialize(const std::vector<Object3d*>& models);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void OnGround();

	void ReStart();

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetTranslationParent(const WorldTransform* parent) { worldTransform_.translationParent_ = parent; }

	void AddTransform(const Vector3& velocity) { worldTransform_.translation_ += velocity; }

	Vector3 GetWorldPos() const;
	Vector3 GetWeaponWorldPos() const;
	Vector3 GetSize() const { return size_; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

};

