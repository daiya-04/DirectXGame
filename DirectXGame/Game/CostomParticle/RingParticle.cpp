#include "RingParticle.h"

void RingParticle::Init()
{
	uint32_t circle = TextureManager::Load("circle.png");
	particle = std::make_unique<Particle>();
	particle.reset(Particle::Create(circle, 100));
	emitter.count_ = 12;
	emitter.frequency_ = 1.0f;
	emitter.translate_ = {0.0f,0.0f,0.0f};
	uint32_t uv = TextureManager::Load("circle.png");
	particle2 = std::make_unique<Particle>();
	particle2.reset(Particle::Create(uv, 100));
}

void RingParticle::Update()
{
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
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
	it = 0;
	for (std::list<Particle::ParticleData>::iterator itParticle = particles2_.begin(); itParticle != particles2_.end(); itParticle++) {
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
	if (ImGui::Button("Addcircle1")) {
		Particle::ParticleData particle;
		particle.worldTransform_.translation_ = { 0.0f,5.0f,0.0f };
		particle.worldTransform_.scale_ = { particleScale,particleScale,1.0f };
		particle.lifeTime_ = { lifeTime };
		particle.color_ = { particleColor.x,particleColor.y,particleColor.z,1.0f };
		std::list<Particle::ParticleData>particleData;

		for (int it = 0; it < kDirection; it++) {
				particleData.push_back(particle);	
		}
		particles_.splice(particles_.end(), particleData);
	}
	if (ImGui::Button("Addcircle2")) {
		Particle::ParticleData particle;
		particle.worldTransform_.translation_ = { 0.0f,5.0f,0.0f };
		particle.worldTransform_.scale_ = { particleScale,particleScale,1.0f };
		particle.lifeTime_ = { lifeTime };
		particle.color_ = { particle2Color.x,particle2Color.y,particle2Color.z,1.0f };
		std::list<Particle::ParticleData>particleData;

		for (int it = 0; it < kDirection; it++) {
			particleData.push_back(particle);
		}
		particles2_.splice(particles2_.end(), particleData);
	}
	ImGui::SliderFloat("Trans", &kTranslate,0,10);
	ImGui::SliderFloat("life", &lifeTime,0,1);
	ImGui::SliderFloat("Scale", &particleScale,0,10);
	ImGui::SliderFloat3("Color1", &particleColor.x,0,255);
	ImGui::SliderFloat3("Color2", &particle2Color.x,0,255);
	ImGui::End();
#endif
}

void RingParticle::Draw(const Camera& camera)
{
	particle->Draw(particles_,camera);
	particle2->Draw(particles2_,camera);
}
