#pragma once
#include "Sprite.h"
#include "Vec2.h"

#include <memory>

class HP {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureHandle"></param>
	/// <param name="pos"></param>
	/// <param name="size"></param>
	void Init(uint32_t textureHandle, const Vector2& pos, const Vector2& size);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// HPセット
	/// </summary>
	/// <param name="hp"></param>
	void SetMaxHP(uint32_t hp) { maxHp_ = hp_ = hp; }
	/// <summary>
	/// ダメージを受ける(HP減少)
	/// </summary>
	void TakeDamage(uint32_t damage = 1) { hp_ -= damage; }

	uint32_t GetHP() const { return hp_; }

private:
	//ゲージ
	std::unique_ptr<DaiEngine::Sprite> gauge_;
	//ダメージ用ゲージ
	std::unique_ptr<DaiEngine::Sprite> damageGauge_;
	//ゲージの大きさ
	Vector2 gaugeSize_;

	//HP
	uint32_t hp_;
	//最大HP
	uint32_t maxHp_;

	float percent_ = 1.0f;
	float currentPer_ = 1.0f;

	float dampingRate_ = 0.2f;

};

