#include "ParticleInfo.hlsli"
#include "Random.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<int32_t> gFreeList : register(u2);

struct PerFrame {
    float32_t time;
    float32_t deltaTime;
};

ConstantBuffer<PerFrame> gPerFrame : register(b0);

struct MaxParticleNum{
    int32_t maxNum;
};

ConstantBuffer<MaxParticleNum> gMaxParticles : register(b1);

ConstantBuffer<OverLifeTime> gOverLifeTime : register(b2);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {

    uint32_t particleIndex = DTid.x;

    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

    if(particleIndex < gMaxParticles.maxNum){
        if(gParticles[particleIndex].currentTime < gParticles[particleIndex].lifeTime){
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float32_t param = gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime;

            float32_t3 velocity = float32_t3(0.0f, 0.0f, 0.0f);

            if(gOverLifeTime.isTransVelocity){
                velocity = lerp(gOverLifeTime.startVelocity, gOverLifeTime.endVelocity, param);
            }

            if(gOverLifeTime.isTransSpeed && all(gParticles[particleIndex].velocity != float32_t3(0.0f, 0.0f, 0.0f))){
                gParticles[particleIndex].velocity = normalize(gParticles[particleIndex].velocity) * lerp(gOverLifeTime.startSpeed, gOverLifeTime.endSpeed, param);
            }
            
            gParticles[particleIndex].translate += (gParticles[particleIndex].velocity + velocity) * gPerFrame.deltaTime;

            if(gOverLifeTime.isScale){
                gParticles[particleIndex].scale = lerp(gOverLifeTime.startScale, gOverLifeTime.endScale, param);
            }

            if(gOverLifeTime.isColor){
                gParticles[particleIndex].color.rgb = lerp(gOverLifeTime.startColor, gOverLifeTime.endColor, param);
            }
            
            float32_t alpha = 1.0f;

            if(gOverLifeTime.isAlpha){
                if(param <= 0.5f){
                    alpha = lerp(gOverLifeTime.startAlpha, gOverLifeTime.midAlpha, param * 2.0f);
                }else if(param > 0.5f){
                    alpha = lerp(gOverLifeTime.midAlpha, gOverLifeTime.endAlpha, (param - 0.5) * 2.0f);
                }
            }
            gParticles[particleIndex].color.a = saturate(alpha);
        }

        if(gParticles[particleIndex].currentTime >= gParticles[particleIndex].lifeTime){
            gParticles[particleIndex].scale = float32_t3(0.0f, 0.0f, 0.0f);
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);

            if((freeListIndex + 1) < gMaxParticles.maxNum){
                gFreeList[freeListIndex + 1] = particleIndex;
            }else {
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }

}

