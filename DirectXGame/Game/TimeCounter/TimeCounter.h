#pragma once
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "Sprite.h"

#include <chrono>

class TimeCounter {
public:

	void Init();
	void Update();
	void Draw();

	void Reset() { count = 0; };

#pragma region
	void IsTimerAnable() { IsTimeCount = true; }
	void IsTimerStop() { IsTimeCount = false; }
#pragma endregion

private:
	bool IsTimeCount = false;
	
	float count = 0;
	int number;

	std::vector<std::unique_ptr<Sprite>>numbers_;

};