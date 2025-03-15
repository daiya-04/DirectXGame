#include "HP.h"

#include "TextureManager.h"

void HP::Init(uint32_t textureHandle, const Vector2& pos, const Vector2& size) {

	gauge_.reset(DaiEngine::Sprite::Create(textureHandle,pos));
	gauge_->SetAnchorpoint({ 0.0f,0.5f });
	gaugeSize_ = size;
	gauge_->SetSize(gaugeSize_);

	damageGauge_.reset(DaiEngine::Sprite::Create(DaiEngine::TextureManager::Load("DamageGauge.png"), pos));
	damageGauge_->SetAnchorpoint({ 0.0f,0.5f });
	damageGauge_->SetSize(gaugeSize_);

	dampingRate_ = 0.05f;

}

void HP::Update() {
	//HPの割合計算
	persent_ = static_cast<float>(hp_) / static_cast<float>(maxHp_);

	currectPer_ = Lerp(dampingRate_, currectPer_, persent_);
	
	gauge_->SetSize({ gaugeSize_.x * persent_, gaugeSize_.y });
	damageGauge_->SetSize({ gaugeSize_.x * currectPer_, gaugeSize_.y });
}

void HP::Draw() {

	damageGauge_->Draw();
	gauge_->Draw();

}
