static const uint32_t kMaxParticles = 1024;

struct Particle {
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

struct PerFrame {
    float32_t time;
    float32_t deltaTime;
};

ConstantBuffer<PerFrame> gPerFrame : register(b0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {

    uint32_t particleIndex = DTid.x;

    if(particleIndex < kMaxParticles){
        if(gParticles[particleIndex].color.a != 0){
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity * gPerFrame.deltaTime;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float32_t alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
    }

}

