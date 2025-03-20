#include "IceImpactManager.h"

#include "TextureManager.h"

void IceImpactManager::Init() {

	iceScar_.reset(IceScar::Create(DaiEngine::TextureManager::Load("ScarBase.png")));
	iceScar_->SetScale(20.0f);

	iceImpact_ = std::make_unique<IceImpact>();
	iceImpact_->Init();
	iceImpact_->SetIceScar(iceScar_.get());
	

	isAttack_ = false;
	preIsAttack_ = false;
}

void IceImpactManager::Update() {
	preIsAttack_ = isAttack_;

	iceImpact_->Update();

	iceScar_->Update();

	if (!iceImpact_->IsAttack()) {
		isAttack_ = false;
	}

}

void IceImpactManager::DrawParticle(const DaiEngine::Camera& camera) {

	iceImpact_->DrawParticle(camera);

}

void IceImpactManager::DrawScar(const DaiEngine::Camera& camera) {

	iceScar_->Draw(camera);

}

void IceImpactManager::AttackStart() {

	iceImpact_->AttackStart();
	isAttack_ = true;

}

void IceImpactManager::SetAttackData(const Vector3& pos) {

	iceImpact_->SetAttackData(pos);

}
