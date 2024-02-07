#include "RingParticle.h"

void RingParticle::Init()
{
	uint32_t ring = TextureManager::Load("star.png");
	particle = std::make_unique<Particle>();
	particle.reset(Particle::Create(ring, 1000));
	uint32_t circle = TextureManager::Load("circle.png");
	Circleparticle = std::make_unique<Particle>();
	Circleparticle.reset(Particle::Create(circle, 1000));

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
		(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_ * kDeltaTime * kTranslate;
		float alpha = 1.0f - ((*itParticle).currentTime_ / (*itParticle).lifeTime_);
		(*itParticle).color_.w = alpha;
		(*itParticle).currentTime_ += kDeltaTime;
		it++;
	}

	it = 0;
	for (std::list<Particle::ParticleData>::iterator itParticle = Circleparticles_.begin(); itParticle != Circleparticles_.end(); itParticle++) {
		if (it == kDirection) {
			it = 0;
		}
		(*itParticle).worldTransform_.translation_.z = 0;
		(*itParticle).velocity_.z = 0;
		(*itParticle).worldTransform_.translation_ += translate[it] * kDeltaTime * kCircleTranslate;
		float alpha = 1.0f - ((*itParticle).currentTime_ / (*itParticle).lifeTime_);
		(*itParticle).color_.w = alpha;
		(*itParticle).currentTime_ += kDeltaTime;
		it++;
	}

#ifdef _DEBUG
	ImGui::Begin("GameParticle");
	ImGui::SliderFloat("Trans", &kCircleTranslate, 0, 10);
	ImGui::SliderFloat("life", &CirclelifeTime, 0, 1);
	ImGui::SliderFloat("Scale", &CircleparticleScale, 0, 10);
	ImGui::SliderFloat3("Color1", &particleColor[0].x, 0, 255);
	ImGui::End();
#endif
}

void RingParticle::Draw(const Camera& camera)
{
	particle->Draw(particles_,camera);
	Circleparticle->Draw(Circleparticles_,camera);
}

void RingParticle::addParticle(Vector3 pos,Vector3 color)
{
	Particle::ParticleData particle;
	particle.worldTransform_.translation_ = pos;
	particle.worldTransform_.scale_ = { particleScale,particleScale,1.0f };
	particle.lifeTime_ = { lifeTime };
	particle.color_ = { color.x,color.y,color.z,1.0f };
	std::list<Particle::ParticleData>particleData;

	emitter.count_ = 10;
	emitter.translate_ = pos;
	emitter.frequency_ = 10;
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::list<Particle::ParticleData>temp;
	temp = Particle::Emit(emitter, randomEngine);
	std::list<Particle::ParticleData>::iterator Tempit = temp.begin();
	for (int it = 0; it < 10; it++) {
		particle.velocity_ = Tempit->velocity_;
		particleData.push_back(particle);
		Tempit++;
	}
	particles_.splice(particles_.end(), particleData);
}

void RingParticle::addCircleParticle(Vector3 pos, Vector3 color)
{
	Particle::ParticleData particle;
	particle.worldTransform_.translation_ = pos;
	particle.worldTransform_.scale_ = { CircleparticleScale,CircleparticleScale,1.0f };
	particle.lifeTime_ = { CirclelifeTime };
	particle.color_ = { color.x,color.y,color.z,1.0f };
	std::list<Particle::ParticleData>particleData;

	for (int it = 0; it < kDirection; it++) {
		particle.worldTransform_.translation_ = pos + (translate[it] * 2);
		particleData.push_back(particle);
	}
	Circleparticles_.splice(Circleparticles_.end(), particleData);
}
