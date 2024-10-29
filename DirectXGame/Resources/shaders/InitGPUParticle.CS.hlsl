#include "ParticleInfo.hlsli"


RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<int32_t> gFreeList : register(u2);

struct MaxParticleNum{
    int32_t maxNum;
};

ConstantBuffer<MaxParticleNum> gMaxParticles : register(b0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
    uint32_t particleIndex = DTid.x;
    if(particleIndex < gMaxParticles.maxNum) {
         gParticles[particleIndex] = (Particle)0;
         gFreeList[particleIndex] = particleIndex;
    }
    if(particleIndex == 0){
        gFreeListIndex[0] = gMaxParticles.maxNum - 1;
    }
}

