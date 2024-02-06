#pragma once
#include "Particle.h"
#include "TextureManager.h"
#include "Camera.h"
#include "ImGuiManager.h"
class RingParticle
{
public:

	void Init();
	void Update();
	void Draw(const Camera& camera);

private:
	std::unique_ptr<Particle> particle;
	std::unique_ptr<Particle> particle2;
	Particle::Emitter emitter;
	std::list<Particle::ParticleData> particles_;
	std::list<Particle::ParticleData> particles2_;
	const float kDeltaTime = 1.0f / 60.0f;
	const int kDirection = 16;

	Vector3 translate[16] = {
		{0.0f,1.0f,0.0f},
		{0.25f,0.75f,0.0f},
		{0.5f,0.5f,0.0f},
		{0.75f,0.25f,0.0f},

		{1.0f,0.0f,0.0f},
		{0.75f,-0.25f,0.0f},
		{0.5f,-0.5f,0.0f},
		{0.25f,-0.75f,0.0f},

		{0.0f,-1.0f,0.0f},
		{-0.25f,-0.75f,0.0f},
		{-0.5f,-0.5f,0.0f},
		{-0.75f,-0.25f,0.0f},

		{-1.0f,0.0f,0.0f},
		{-0.75f,0.25f,0.0f},
		{-0.5f,0.5f,0.0f},
		{-0.25f,0.75f,0.0f},
	};
	float kTranslate = 4.5f;
	float lifeTime = 0.4f;
	float particleScale = 1.5f;
	Vector3 particleColor = {55.0f,30.0f,1.0f};
	Vector3 particle2Color = {1.0f,30.0f,55.0f};
};
