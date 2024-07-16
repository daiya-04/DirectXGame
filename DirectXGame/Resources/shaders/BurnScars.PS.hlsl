#include"Sprite.hlsli"

struct CBuffData {
	float32_t threshold;
};

ConstantBuffer<CBuffData> gData : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTex : register(t1);
SamplerState gSampler : register(s0);

float32_t2 RandomVec(float32_t2 p) {
    float32_t2 angle = float32_t2(
				dot(p, float32_t2(127.1, 311.7)),
                dot(p, float32_t2(269.5, 183.3)));
    return frac(sin(angle) * 43758.5453123) * 2 - 1;
}

// パーリンノイズの計算
float32_t PerlinNoise(float32_t density, float32_t2 uv) {
    float32_t2 uvFloor = floor(uv * density);
    float32_t2 uvFrac = frac(uv * density);
    
    // フェード関数
    float32_t2 u = uvFrac * uvFrac * (3.0 - 2.0 * uvFrac);
    
    // グラデーションベクトル
    float32_t2 g00 = RandomVec(uvFloor);
    float32_t2 g10 = RandomVec(uvFloor + float32_t2(1.0, 0.0));
    float32_t2 g01 = RandomVec(uvFloor + float32_t2(0.0, 1.0));
    float32_t2 g11 = RandomVec(uvFloor + float32_t2(1.0, 1.0));
    
    // ドットプロダクト
    float32_t n00 = dot(g00, uvFrac);
    float32_t n10 = dot(g10, uvFrac - float32_t2(1.0, 0.0));
    float32_t n01 = dot(g01, uvFrac - float32_t2(0.0, 1.0));
    float32_t n11 = dot(g11, uvFrac - float32_t2(1.0, 1.0));
    
    // 補間
    float32_t n0 = lerp(n00, n10, u.x);
    float32_t n1 = lerp(n01, n11, u.x);
    
    return lerp(n0, n1, u.y) / 2 + 0.5;
}

float32_t FractalSumNoize(float32_t density, float32_t2 uv){

	float32_t fn;
	fn = PerlinNoise(density, uv) / 2;
	fn += PerlinNoise(density * 2, uv) / 4;
	fn += PerlinNoise(density * 4, uv) / 8;
	fn += PerlinNoise(density * 8, uv) / 16;
	return fn;
}

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);

	float32_t n1 = FractalSumNoize(15.0f, input.texcoord + float32_t2(1.5f,1.5f));
	float32_t n2 = FractalSumNoize(15.0f, input.texcoord + float32_t2(-1.5f,-1.5f));

	float n = step(0.955,1-abs(n1-n2));

	output.color += float32_t4(n,n,n,output.color.a) * float32_t4(0.96f,0.13f,0.04f,output.color.a);

	float32_t mask = gMaskTex.Sample(gSampler, input.texcoord);
	
	if(mask < gData.threshold){
		discard;
	}

	float32_t edge = 1.0f - smoothstep(gData.threshold, gData.threshold + 0.1f, mask);
	
	output.color.rgb -= edge * float32_t3(1.0f, 1.0f, 1.0f);
	
	return output;
}