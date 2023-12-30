#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "Matrix44.h"
#include <memory>
#include <list>
#include <vector>
#include <array>
#include <optional>
#include "FollowCamera.h"
#include "Particle.h"


class Player{
private: //ふるまい用メンバ変数

	enum class Behavior {
		kRoot,
		kAttack,
		kDash,
	};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	static void(Player::* BehaviorTable[])();
	static void(Player::* RequestTable[])();

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

	enum Parts {
		Body,
		Head,

		kPartsNum,
	};

private:

	std::vector<std::unique_ptr<Object3d>> obj_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, kPartsNum> partsWorldTransform_;
	Vector3 size_ = { 1.0f,2.0f,1.0f };

	Matrix4x4 rotateMat_ = MakeIdentity44();
	Vector3 rotate_ = { 0.0f,0.0f,1.0f };
	Vector3 from_ = { 0.0f,0.0f,1.0f };
	//Vector3 velocity_ = {};

	WorkDash workDash_;
	WorkAttack workAttack_;

	std::unique_ptr<Particle> magicParticle_;

	std::list<Particle::ParticleData> particles_;
	Particle::Emitter emitter_{};

	float particleVelocity_ = 0.3f;

	FollowCamera* followCamera_ = nullptr;

	const float kDeltaTime_ = 1.0f / 60.0f;
	std::random_device seedGenerator;
	std::mt19937 randomEngine;

public:

	Player() :randomEngine(seedGenerator()) {}

	//初期化
	void Init(std::vector<uint32_t> modelHandles);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);
	//パーティクル描画
	void DrawParticle(const Camera& camera);

	//カメラの設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }

	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetWorldPos() const;

};

