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

float32_t3 RandomVec(float32_t3 p) {
    float32_t3 angle = float32_t3(
				dot(p, float32_t3(127.1f, 311.7f, 74.7f)),
                dot(p, float32_t3(269.5f, 183.3f, 246.1f)),
                dot(p, float32_t3(113.5f, 271.9f, 52.7f)));
    return frac(sin(angle) * 43758.5453123f) * 2.0f - 1.0f;
}

// パーリンノイズの計算
float32_t PerlinNoise(float32_t density, float32_t3 position) {
    float32_t3 posFloor = floor(position * density);
    float32_t3 posFrac = frac(position * density);
    
    // フェード関数
    float32_t3 u = posFrac * posFrac * (3.0f - 2.0f * posFrac);
    
    // グラデーションベクトル
    float32_t3 g000 = RandomVec(posFloor);
    float32_t3 g100 = RandomVec(posFloor + float32_t3(1.0f, 0.0f, 0.0f));
    float32_t3 g010 = RandomVec(posFloor + float32_t3(0.0f, 1.0f, 0.0f));
    float32_t3 g110 = RandomVec(posFloor + float32_t3(1.0f, 1.0f, 0.0f));
    float32_t3 g001 = RandomVec(posFloor + float32_t3(0.0f, 0.0f, 1.0f));
    float32_t3 g101 = RandomVec(posFloor + float32_t3(1.0f, 0.0f, 1.0f));
    float32_t3 g011 = RandomVec(posFloor + float32_t3(0.0f, 1.0f, 1.0f));
    float32_t3 g111 = RandomVec(posFloor + float32_t3(1.0f, 1.0f, 1.0f));

    
    // ドットプロダクト
    float32_t n000 = dot(g000, posFrac);
    float32_t n100 = dot(g100, posFrac - float32_t3(1.0f, 0.0f, 0.0f));
    float32_t n010 = dot(g010, posFrac - float32_t3(0.0f, 1.0f, 0.0f));
    float32_t n110 = dot(g110, posFrac - float32_t3(1.0f, 1.0f, 0.0f));
    float32_t n001 = dot(g001, posFrac - float32_t3(0.0f, 0.0f, 1.0f));
    float32_t n101 = dot(g101, posFrac - float32_t3(1.0f, 0.0f, 1.0f));
    float32_t n011 = dot(g011, posFrac - float32_t3(0.0f, 1.0f, 1.0f));
    float32_t n111 = dot(g111, posFrac - float32_t3(1.0f, 1.0f, 1.0f));
    
    // 補間
    float32_t nx00 = lerp(n000, n100, u.x);
    float32_t nx10 = lerp(n010, n110, u.x);
    float32_t nx01 = lerp(n001, n101, u.x);
    float32_t nx11 = lerp(n011, n111, u.x);

    float32_t nxy0 = lerp(nx00, nx10, u.y);
    float32_t nxy1 = lerp(nx01, nx11, u.y);

    
    return lerp(nxy0, nxy1, u.z);
}

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

            if(gOverLifeTime.isTransSpeed){
                gParticles[particleIndex].velocity = normalize(gParticles[particleIndex].velocity) * lerp(gOverLifeTime.startSpeed, gOverLifeTime.endSpeed, param);
            }
            
            gParticles[particleIndex].velocity.y -= gOverLifeTime.gravity;

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

            gParticles[particleIndex].rotate += gParticles[particleIndex].roringSpeed * gPerFrame.deltaTime;

            if(gOverLifeTime.isNoise){
                float32_t time = gPerFrame.time;
                float32_t noise0 = PerlinNoise(gOverLifeTime.density, gParticles[particleIndex].translate + float32_t3(time, time, time));
                float32_t noise1 = PerlinNoise(gOverLifeTime.density, gParticles[particleIndex].translate + float32_t3(time, time, time));
                float32_t noise2 = PerlinNoise(gOverLifeTime.density, gParticles[particleIndex].translate + float32_t3(time, time, time));
                gParticles[particleIndex].noiseOffset = float32_t3(noise0, noise1, noise2) * gOverLifeTime.strength;
            }

        }

        if(gParticles[particleIndex].currentTime >= gParticles[particleIndex].lifeTime){
            gParticles[particleIndex] = (Particle)0;
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

