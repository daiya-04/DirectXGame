#pragma once
#include <cstdint>
#include "Camera.h"
#include "Sprite.h"
#include "Vec2.h"

#include <memory>

class GameTime {
public:

	void Init();

	void Update();

	void Draw();

	void PrepareTime();

private:

	void SetGlobalVariables();

	void ApllyGlobalVariables();

	uint32_t GetSeconds() const;

	uint32_t GetMinutes() const;

private:

	enum TimerDigitIndex {
		SecondsOnes, //秒の1の位
		SecondsTens, //秒の10の位
		MinutesOnes, //分の1の位
		MinutesTens, //分の10の位
		
		TimerDigitCount,
	};

private:

	const float kDeltaTime_ = 1.0f / 60.0f;
	float timer_ = 0.0f;

	float timerUIScale_ = 2.5f;

	std::array<std::unique_ptr<DaiEngine::Sprite>, TimerDigitCount> timerDigits_;
	const float kNumberTexSize_ = 64.0f;

	std::array<Vector2, TimerDigitCount> digitOffset_;
	
	//コロン
	std::unique_ptr<DaiEngine::Sprite> colonUI_;

	//基準座標
	Vector2 BasePos_ = {};

};

