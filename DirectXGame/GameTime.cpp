#include "GameTime.h"
#include "TextureManager.h"
#include "GlobalVariables.h"

void GameTime::SetGlobalVariables() {

	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();
	
	std::string groupName = "TimerUI";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "SecOnesOffset", digitOffset_[SecondsOnes]);
	globalVariables->AddItem(groupName, "SecTensOffset", digitOffset_[SecondsTens]);
	globalVariables->AddItem(groupName, "MinOnesOffset", digitOffset_[MinutesOnes]);
	globalVariables->AddItem(groupName, "MinTensOffset", digitOffset_[MinutesTens]);
	globalVariables->AddItem(groupName, "BasePosition", BasePos_);
	globalVariables->AddItem(groupName, "UIScale", timerUIScale_);

	
}

void GameTime::ApllyGlobalVariables() {

	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();

	std::string groupName = "TimerUI";
	digitOffset_[SecondsOnes] = globalVariables->GetVec2Value(groupName, "SecOnesOffset");
	digitOffset_[SecondsTens] = globalVariables->GetVec2Value(groupName, "SecTensOffset");
	digitOffset_[MinutesOnes] = globalVariables->GetVec2Value(groupName, "MinOnesOffset");
	digitOffset_[MinutesTens] = globalVariables->GetVec2Value(groupName, "MinTensOffset");
	BasePos_ = globalVariables->GetVec2Value(groupName, "BasePosition");
	timerUIScale_ = globalVariables->GetFloatValue(groupName, "UIScale");

}

void GameTime::Init() {

	SetGlobalVariables();
	ApllyGlobalVariables();


	uint32_t numberTex = DaiEngine::TextureManager::Load("number.png");
	uint32_t colonTex = DaiEngine::TextureManager::Load("colon.png");

	for (auto& timerDigit : timerDigits_) {
		timerDigit.reset(DaiEngine::Sprite::Create(numberTex, {}));
		timerDigit->SetTextureArea({}, { kNumberTexSize_, kNumberTexSize_ });
		timerDigit->SetSize({ kNumberTexSize_,kNumberTexSize_ });
		timerDigit->SetScale(timerUIScale_);
	}

	colonUI_.reset(DaiEngine::Sprite::Create(colonTex, {}));
	colonUI_->SetScale(timerUIScale_);
	

}

void GameTime::Update() {
	ApllyGlobalVariables();

	timer_ += kDeltaTime_;

}

void GameTime::Draw() {

	for (auto& timerDigit : timerDigits_) {
		timerDigit->Draw();
	}
	colonUI_->Draw();

}

void GameTime::PrepareTime() {
	//秒数と分数を取得
	uint32_t seconds = GetSeconds();
	uint32_t minutes = GetMinutes();
	
	//各桁の数を算出
	std::array<uint32_t, TimerDigitCount> numbers{
		 seconds % 10,
		 seconds / 10,
		 minutes % 10,
		 minutes / 10,
	};

	Vector2 numberTexSize = { kNumberTexSize_ ,kNumberTexSize_ };

	for (size_t index = 0; index < TimerDigitCount; index++) {
		timerDigits_[index]->SetTextureArea({ kNumberTexSize_ * numbers[index], 0.0f}, numberTexSize);
		timerDigits_[index]->SetPosition(BasePos_ + digitOffset_[index]);
	}
	colonUI_->SetPosition(BasePos_);
}

uint32_t GameTime::GetSeconds() const {
	return static_cast<uint32_t>(timer_) % 60;
}

uint32_t GameTime::GetMinutes() const {
	return static_cast<uint32_t>(timer_) / 60;
}
