#include "TimeCounter.h"

void TimeCounter::Init()
{
	uint32_t numberTex = TextureManager::Load("numbers.png");
	Sprite* spriteOne = new Sprite(numberTex, { 1186.0f,658.0f }, { 64.0f,64.0f }, 0.0f, { 0.0f,0.5f }, { 0.2f,0.2f,0.2f,1.0f });
	spriteOne->Initialize();
	numbers_.push_back(std::unique_ptr<Sprite>(spriteOne));

	Sprite* spriteTwo = new Sprite(numberTex, { 1186.0f,658.0f }, { 64.0f,64.0f }, 0.0f, { 1.0f,0.5f }, { 0.2f,0.2f,0.2f,1.0f });
	spriteTwo->Initialize();
	numbers_.push_back(std::unique_ptr<Sprite>(spriteTwo));
}

void TimeCounter::Update()
{
	if (IsTimeCount) {
		flameCount++;

	}
#ifdef _DEBUG
	ImGui::Begin("Timer");
	ImGui::InputFloat("Count", &flameCount);
	ImGui::InputFloat("NumberCount", &NumberCount);
	ImGui::InputFloat2("Anchor", &Anchor.x);
	ImGui::InputFloat2("Pos", &Pos.x);
	ImGui::End();
#endif
	NumberCount = flameCount / 60;
	if (NumberCount > 99) {
		NumberCount = 99;
	}


}

void TimeCounter::Draw()
{
	tenPlaceNumber = (int)NumberCount / 10;
	onePlaceNumber = (int)NumberCount % 10;
	//スプライトのn番を描画
	numbers_[ONEPLACE]->SetTextureArea(NumberValue[onePlaceNumber].Base, NumberValue[onePlaceNumber].Size);
	numbers_[ONEPLACE]->Draw();
	numbers_[TENPLACE]->SetTextureArea(NumberValue[tenPlaceNumber].Base, NumberValue[tenPlaceNumber].Size);
	numbers_[TENPLACE]->Draw();
	
}
