#pragma once
#include "Sprite.h"
#include "Vec2.h"

#include <memory>

class Stamina {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="size"></param>
	void Init(const Vector2& pos, const Vector2& size);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// スタミナセット
	/// </summary>
	/// <param name="stamina"></param>
	void SetMaxStamina(float stamina) { maxStamina_ = stamina_ = stamina; }
	/// <summary>
	/// ダッシュ時スタミナ消費
	/// </summary>
	/// <param name="value"></param>
	void ConsumeDash(float value) { stamina_ -= value * deltaTime_; }
	/// <summary>
	/// 回避時スタミナ消費
	/// </summary>
	/// <param name="value"></param>
	void ConsumeAvoid(float value) { stamina_ -= value; }
	/// <summary>
	/// スタミナ回復
	/// </summary>
	/// <param name="value"></param>
	void Healing(float value) { stamina_ += value * deltaTime_; }

	float GetStamina() const { return stamina_; }


private:
	//ゲージ
	std::unique_ptr<DaiEngine::Sprite> gauge_;
	//ゲージサイズ
	Vector2 gaugeSize_;

	//スタミナ
	float stamina_ = 0;
	//スタミナ最大値
	float maxStamina_ = 0;

	float percent_ = 1.0f;
	float currentPer_ = 1.0f;

	float decrementSpeed_ = 0.0f;

	float deltaTime_ = 1.0f / 60.0f;

};

