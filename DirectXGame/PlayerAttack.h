#pragma once
#include "IPlayerBehavior.h"

#include <array>

class PlayerAttack : public IPlayerBehavior {
public:

	PlayerAttack(Player* player) : player_(player) {}

	~PlayerAttack() override {}

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;

	//攻撃アニメーションのスピード
	float attackAnimeSpeed_ = 1.0f / 30.0f;
	//攻撃中の現在の時間
	uint32_t count_ = 0;
	//現在のコンボ数(何段目か)
	uint32_t comboIndex_ = 0;
	//一段の中のどのフェーズか
	uint32_t inComboPhase_ = 0;
	//コンボが続くか
	bool comboNext_ = false;
	//攻撃の速さ
	float speed_ = 0.0f;

	//攻撃の射程
	float attackRange_ = 20.0f;

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

};

