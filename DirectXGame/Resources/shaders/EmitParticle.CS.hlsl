static const uint32_t kMaxParticles = 1024;

float32_t rand3dTo1d(float32_t3 value, float32_t3 dotDir = float32_t3(12.9898, 78.233, 37.719)){
    //make value smaller to avoid artefacts
    float32_t3 smallValue = sin(value);
    //get scalar value from 3d vector
    float32_t random = dot(smallValue, dotDir);
    //make value more random by making it bigger and then taking teh factional part
    random = frac(sin(random) * 143758.5453);
    return random;
}

float32_t3 rand3dTo3d(float32_t3 value){
    return float32_t3(
        rand3dTo1d(value, float32_t3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float32_t3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, float32_t3(73.156, 52.235, 09.151))
    );
}

class RandomGenerator {
    float32_t3 seed;
    float32_t3 Generate3d(){
        seed = rand3dTo3d(seed);
        return seed;
    }
    float32_t Generate1d(){
        float32_t result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};


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

struct EmitterSphere {
    float32_t3 translate;
    float32_t radius;
    uint32_t count;
    float32_t frequency;
    float32_t frequencyTime;
    uint32_t emit;
};

ConstantBuffer<EmitterSphere> gEmitter : register(b0);

struct PerFrame {
    float32_t time;
    float32_t deltaTime;
};

ConstantBuffer<PerFrame> gPerFrame : register(b1);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {

    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

    if(gEmitter.emit != 0){
        for(uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex){
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

            if((0 <= freeListIndex) && (freeListIndex < kMaxParticles)) {
                uint32_t particleIndex = gFreeList[freeListIndex];
                gParticles[particleIndex].translate = (generator.Generate3d() - 0.5f) * 0;
                gParticles[particleIndex].scale = float32_t3(0.5f, 0.5f, 0.5f);
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].velocity = (generator.Generate3d() - 0.5f) * 2;
                gParticles[particleIndex].lifeTime = generator.Generate1d() * 2;
                gParticles[particleIndex].currentTime = 0.0f;
            }else {
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }
}

