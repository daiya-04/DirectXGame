#include "Stamina.h"

#include "TextureManager.h"
#include "ImGuiManager.h"

void Stamina::Init(const Vector2& pos, const Vector2& size) {

	gauge_.reset(DaiEngine::Sprite::Create(DaiEngine::TextureManager::Load("StaminaGauge.png"), pos));
	gauge_->SetAnchorpoint({ 0.0f,0.5f });
	gaugeSize_ = size;
	gauge_->SetSize(gaugeSize_);

	decrementSpeed_ = 0.01f;
}

void Stamina::Update() {

	stamina_ = std::clamp(stamina_, 0.0f, maxStamina_);

	percent_ = stamina_ / maxStamina_;

	if (percent_ < currentPer_) {
		currentPer_ = std::clamp(currentPer_ - decrementSpeed_, percent_, currentPer_);
	}else {
		currentPer_ = std::clamp(currentPer_ + decrementSpeed_, currentPer_, percent_);
	}

	gauge_->SetSize({ gaugeSize_.x * currentPer_, gaugeSize_.y });

}

void Stamina::Draw() {
	gauge_->Draw();
}
