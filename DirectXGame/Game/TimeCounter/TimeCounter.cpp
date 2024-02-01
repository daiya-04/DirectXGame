#include "TimeCounter.h"

void TimeCounter::Init()
{
	uint32_t numberTex = TextureManager::Load("numbers.png");
	Sprite* sprite = new Sprite(numberTex, { 640.0f,360.0f }, { 1280.0f,720.0f },
		0.0f, { 0.5f,0.5f }, { 0.2f,0.2f,0.2f,1.0f });
	sprite->Initialize();

	numbers_.push_back(std::unique_ptr<Sprite>(sprite));
}

void TimeCounter::Update()
{
	if (IsTimeCount) {
		count++;
	}
#ifdef _DEBUG
	ImGui::Begin("Timer");
	ImGui::InputFloat("Count", &count);
	ImGui::InputInt("Count", &number);
	ImGui::End();
#endif
	number = (int)(count / 60);
}

void TimeCounter::Draw()
{
	//スプライトのn番を描画
	for (std::unique_ptr<Sprite>& sprite : numbers_) {
		sprite->Draw();
	}
}
