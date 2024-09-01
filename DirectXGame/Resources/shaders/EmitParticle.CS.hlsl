//static const uint32_t kMaxParticles = 100000;

float32_t rand3dTo1d(float32_t3 value, float32_t3 dotDir = float32_t3(12.9898, 78.233, 37.719)){
    
    float32_t3 smallValue = sin(value);
    
    float32_t random = dot(smallValue, dotDir);
    
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
};

ConstantBuffer<EmitterSphere> gEmitter : register(b0);

struct PerFrame {
    float32_t time;
    float32_t deltaTime;
};

ConstantBuffer<PerFrame> gPerFrame : register(b1);

struct MaxParticleNum{
    int32_t maxNum;
};

ConstantBuffer<MaxParticleNum> gMaxParticles : register(b2);

float32_t4x4 MakeRotateXMat(float32_t angleRad){
    return float32_t4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cos(angleRad), sin(angleRad), 0.0f,
        0.0f,-sin(angleRad), cos(angleRad), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

float32_t4x4 MakeRotateYMat(float32_t angleRad){
    return float32_t4x4(
        cos(angleRad), 0.0f,-sin(angleRad), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sin(angleRad), 0.0f, cos(angleRad), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

float32_t4x4 MakeRotateZMat(float32_t angleRad){
    return float32_t4x4(
        cos(angleRad), sin(angleRad), 0.0f, 0.0f,
        -sin(angleRad), cos(angleRad), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

float32_t3 ShotDirection(RandomGenerator generator) {

    float32_t angleX = (generator.Generate1d() - 0.5f) * gEmitter.angle;
    float32_t angleY = (generator.Generate1d() - 0.5f) * gEmitter.angle;
    float32_t angleZ = (generator.Generate1d() - 0.5f) * gEmitter.angle;

    float32_t4x4 rotateMat = mul(mul(MakeRotateXMat(radians(angleX)),MakeRotateYMat(radians(angleY))), MakeRotateZMat(radians(angleZ)));

    return mul(float32_t4(gEmitter.direction, 1.0f), rotateMat).xyz;

    //Vector3 a = { std::cosf(lat) * std::cosf(lon),std::sinf(lat),std::cosf(lat) * std::sinf(lon) };
    //lon = theta;
    //lat = phi;

    //float32_t theta = 

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
                gParticles[particleIndex].translate = gEmitter.translate + (generator.Generate3d() - 0.5f) * gEmitter.size;
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

