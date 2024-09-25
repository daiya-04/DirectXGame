#include "Random.hlsli"
#include "Math.hlsli"

struct Particle {
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<int32_t> gFreeList : register(u2);

struct Emitter {
    float32_t3 translate;
    float32_t3 size;
    float32_t scale;
    uint32_t count;
    float32_t frequency;
    float32_t frequencyTime;
    uint32_t emit;
    float32_t3 direction;
    float32_t angle;
    float32_t4 color;
    float32_t lifeTime;
    float32_t speed;
    uint32_t emitterType;
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

float32_t3 ShotDirection(RandomGenerator generator) {

    float32_t angleX = (generator.Generate1d() - 0.5f) * gEmitter.angle;
    float32_t angleY = (generator.Generate1d() - 0.5f) * gEmitter.angle;
    float32_t angleZ = (generator.Generate1d() - 0.5f) * gEmitter.angle;

    float32_t4x4 rotateMat = mul(mul(MakeRotateXMat(radians(angleX)),MakeRotateYMat(radians(angleY))), MakeRotateZMat(radians(angleZ)));

    float32_t3 direction = normalize(mul(float32_t4(gEmitter.direction, 1.0f), rotateMat).xyz);

    if(gEmitter.emitterType == 1){
        if(direction.y < 0.0f){
            direction.y *= -1;
        }
    }

    return direction;

}

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {

    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

    if(gEmitter.emit != 0){
        for(uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex){
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

            if((0 <= freeListIndex) && (freeListIndex < gMaxParticles.maxNum)) {
                uint32_t particleIndex = gFreeList[freeListIndex];
                if(gEmitter.emitterType == 0) { // Sphere(球)

                    gParticles[particleIndex].translate = gEmitter.translate + (normalize(generator.GeneratedRange3(-1.0f, 1.0f)) * (generator.Generate1d() * gEmitter.size.x));

                } else if(gEmitter.emitterType == 1) { //Hemisphere(半球)

                    float32_t3 generatedPos = gEmitter.translate + (normalize(generator.GeneratedRange3(-1.0f, 1.0f)) * (generator.Generate1d() * gEmitter.size.x));
                    if(generatedPos.y < 0.0f){
                        generatedPos.y *= -1;
                    }
                    gParticles[particleIndex].translate = generatedPos;

                } else if(gEmitter.emitterType == 2) { //Box(立方体)

                    gParticles[particleIndex].translate = gEmitter.translate + (generator.GeneratedRange3(-1.0f, 1.0f) * gEmitter.size);

                } else if(gEmitter.emitterType == 3) { //Square(平面)

                    float32_t2 pos = generator.GeneratedRange2(-1.0f, 1.0f) * float32_t2(gEmitter.size.x, gEmitter.size.z);
                    gParticles[particleIndex].translate = gEmitter.translate + float32_t3(pos.x, 0.0f, pos.y);

                } else if(gEmitter.emitterType == 4) { //circle(円形)

                    float32_t2 pos = normalize(generator.GeneratedRange2(-1.0f, 1.0f)) * (generator.Generate1d() * gEmitter.size.x);
                    gParticles[particleIndex].translate = gEmitter.translate + float32_t3(pos.x, 0.0f, pos.y);

                }
                
                gParticles[particleIndex].scale = float32_t3(gEmitter.scale, gEmitter.scale, gEmitter.scale);
                gParticles[particleIndex].color = gEmitter.color;
                gParticles[particleIndex].velocity = ShotDirection(generator) * (generator.Generate1d() * gEmitter.speed);
                gParticles[particleIndex].lifeTime = generator.Generate1d() * gEmitter.lifeTime;
                gParticles[particleIndex].currentTime = 0.0f;
            }else {
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }
}

