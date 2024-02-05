#pragma once
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "Sprite.h"

#define ONEPLACE 0
#define TENPLACE 1

struct NumberSprite {
	Vector2 Base;
	Vector2 Size;
};

class TimeCounter {
public:

	void Init();
	void Update();
	void Draw();

	void Reset() { flameCount = 0; };

#pragma region
	void IsTimerAnable() { IsTimeCount = true; }
	void IsTimerStop() { IsTimeCount = false; }
#pragma endregion

private:
	bool IsTimeCount = false;
	
	float flameCount = 0;
	float NumberCount = 0;
	int onePlaceNumber = 0;
	int tenPlaceNumber = 0;

	Vector2 Anchor;
	Vector2 Pos;

	std::vector<std::unique_ptr<Sprite>>numbers_;
	NumberSprite NumberValue[10] = {
		{{0.0f,0.0f},{64.0f,64.0f}},	//0番
		{{64.0f,0.0f},{64.0f,64.0f}},	//1番
		{{128.0f,0.0f},{64.0f,64.0f}},	//2番
		{{192.0f,0.0f},{64.0f,64.0f}},	//3番
		{{256.0f,0.0f},{64.0f,64.0f}},	//4番
		{{320.0f,0.0f},{64.0f,64.0f}},	//5番
		{{384.0f,0.0f},{64.0f,64.0f}},	//6番
		{{448.0f,0.0f},{64.0f,64.0f}},	//7番
		{{512.0f,0.0f},{64.0f,64.0f}},	//8番
		{{576.0f,0.0f},{64.0f,64.0f}},	//9番
	};

};