#include "RingParticle.h"

void RingParticle::Init()
{
	uint32_t circle = TextureManager::Load("circle.png");
	particle = std::make_unique<Particle>();
	particle.reset(Particle::Create(circle, 1000));

}

void RingParticle::Update()
{
	int it = 0;
	for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
		if (it == kDirection) {
			it = 0;
		}
		(*itParticle).worldTransform_.translation_.z = 0;
		(*itParticle).velocity_.z = 0;
		(*itParticle).worldTransform_.translation_ += translate[it] * kDeltaTime * kTranslate;
		float alpha = 1.0f - ((*itParticle).currentTime_ / (*itParticle).lifeTime_);
		(*itParticle).color_.w = alpha;
		(*itParticle).currentTime_ += kDeltaTime;
		it++;
	}
#ifdef _DEBUG
	ImGui::Begin("GameParticle");
	ImGui::SliderFloat("Trans", &kTranslate, 0, 10);
	ImGui::SliderFloat("life", &lifeTime, 0, 1);
	ImGui::SliderFloat("Scale", &particleScale, 0, 10);
	ImGui::SliderFloat3("Color1", &particleColor[0].x, 0, 255);
	ImGui::SliderFloat3("Color2", &particleColor[0].x, 0, 255);
	ImGui::End();
#endif
}

void RingParticle::Draw(const Camera& camera)
{
	particle->Draw(particles_,camera);
}

void RingParticle::addParticle(Vector3 pos,Vector3 color)
{
	Particle::ParticleData particle;
	particle.worldTransform_.translation_ = pos;
	particle.worldTransform_.scale_ = { particleScale,particleScale,1.0f };
	particle.lifeTime_ = { lifeTime };
	particle.color_ = { color.x,color.y,color.z,1.0f };
	std::list<Particle::ParticleData>particleData;

	for (int it = 0; it < kDirection; it++) {
		particleData.push_back(particle);
	}
	particles_.splice(particles_.end(), particleData);
}
