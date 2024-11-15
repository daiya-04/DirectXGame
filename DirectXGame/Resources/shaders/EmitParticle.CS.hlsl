#include "Random.hlsli"
#include "Math.hlsli"
#include "ParticleInfo.hlsli"

#define NONSHAPE 0
#define SPHERE 1
#define HEMISPHERE 2
#define BOX 3
#define SQUARE 4
#define CIRCLE 5



RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<int32_t> gFreeList : register(u2);

struct Emitter {
    float32_t3 translate;
    float32_t3 size;
    float32_t radius;
    float32_t scale;
    float32_t rotate;
    uint32_t count;
    float32_t frequency;
    float32_t frequencyTime;
    uint32_t emit;
    float32_t4 color;
    float32_t lifeTime;
    float32_t speed;
    uint32_t emitterType;
    uint32_t billboardType;
};

ConstantBuffer<Emitter> gEmitter : register(b0);

struct PerFrame {
    float32_t time;
    float32_t deltaTime;
};

ConstantBuffer<PerFrame> gPerFrame : register(b1);

struct MaxParticleNum{
    int32_t maxNum;
};

ConstantBuffer<MaxParticleNum> gMaxParticles : register(b2);

ConstantBuffer<OverLifeTime> gOverLifeTime : register(b3);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {

    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

    if(gEmitter.emit != 0){
        for(uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex){
            int32_t freeListIndex = 0;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

            if((0 <= freeListIndex) && (freeListIndex < gMaxParticles.maxNum)) {
                uint32_t particleIndex = gFreeList[freeListIndex];

                if(gEmitter.emitterType == NONSHAPE){

                }else if(gEmitter.emitterType == SPHERE) { // Sphere(球)
                    float32_t3 generatedPos = gEmitter.translate + (normalize(generator.GeneratedRange3(-1.0f, 1.0f)) * (generator.Generate1d() * gEmitter.radius));
                    gParticles[particleIndex].translate = generatedPos;
                    float32_t3 direction = normalize(generatedPos - gEmitter.translate);
                    gParticles[particleIndex].velocity = direction * (generator.Generate1d() * gEmitter.speed);

                } else if(gEmitter.emitterType == HEMISPHERE) { //Hemisphere(半球)

                    float32_t3 generatedPos = gEmitter.translate + (normalize(generator.GeneratedRange3(-1.0f, 1.0f)) * (generator.Generate1d() * gEmitter.radius));
                    if(generatedPos.y < 0.0f){
                        generatedPos.y *= -1;
                    }
                    gParticles[particleIndex].translate = generatedPos;

                    float32_t3 direction = normalize(generatedPos - gEmitter.translate);
                    gParticles[particleIndex].velocity = direction * (generator.Generate1d() * gEmitter.speed);

                } else if(gEmitter.emitterType == BOX) { //Box(立方体)

                    gParticles[particleIndex].translate = gEmitter.translate + (generator.GeneratedRange3(-1.0f, 1.0f) * gEmitter.size);
                    gParticles[particleIndex].velocity = float32_t3(1.0f, 0.0f, 0.0f) * gEmitter.speed;

                } else if(gEmitter.emitterType == SQUARE) { //Square(平面)

                    float32_t2 pos = generator.GeneratedRange2(-1.0f, 1.0f) * float32_t2(gEmitter.size.x, gEmitter.size.z);
                    gParticles[particleIndex].translate = gEmitter.translate + float32_t3(pos.x, 0.0f, pos.y);
                    gParticles[particleIndex].velocity = float32_t3(1.0f, 0.0f, 0.0f) * gEmitter.speed;

                } else if(gEmitter.emitterType == CIRCLE) { //circle(円形)

                    float32_t2 generatedPos = normalize(generator.GeneratedRange2(-1.0f, 1.0f)) * (generator.Generate1d() * gEmitter.radius);
                    gParticles[particleIndex].translate = gEmitter.translate + float32_t3(generatedPos.x, 0.0f, generatedPos.y);

                    float32_t3 direction = normalize(float32_t3(generatedPos.x, 0.0f, generatedPos.y) - float32_t3(0.0f,0.0f,0.0f));
                    gParticles[particleIndex].velocity = direction * (generator.Generate1d() * gEmitter.speed);

                }

                if(gOverLifeTime.isConstantVelocity){
                        float32_t3 startVelo = gOverLifeTime.velocity * generator.Generate3d();
                        gParticles[particleIndex].velocity += startVelo;
                    }
                
                gParticles[particleIndex].scale = float32_t3(gEmitter.scale, gEmitter.scale, gEmitter.scale);
                gParticles[particleIndex].rotate = float32_t3(0.0f, 0.0f, radians(gEmitter.rotate * generator.Generate1d()));
                gParticles[particleIndex].color = gEmitter.color;
                gParticles[particleIndex].lifeTime = gEmitter.lifeTime;
                gParticles[particleIndex].currentTime = 0.0f;
                gParticles[particleIndex].billboardType = gEmitter.billboardType;
            }else {
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }
}

