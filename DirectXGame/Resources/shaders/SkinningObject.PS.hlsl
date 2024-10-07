#include"SkinningObject.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct Camera {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 cameraPos;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b3);

struct DeadEffect {
	float32_t threshold;
};

ConstantBuffer<DeadEffect> gDeadEffect : register(b4);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTex : register(t1);
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
	//float4 transformedUV = mul(float32_t4(input.texcoord,0.0f,1.0f),gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	

	if(textureColor.a <= 0.5){
		discard;
	}

	float32_t pn = FractalSumNoize(15.0f, input.texcoord);

	if(pn < gDeadEffect.threshold){
		discard;
	}

	if(gMaterial.enableLighting != 0) {
		float32_t3 toEye = normalize(gCamera.cameraPos - input.worldPosition);

		float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);

		float NdotH = dot(normalize(input.normal), halfVector);
		float specularPow = pow(saturate(NdotH), gMaterial.shininess);

		///Half Lambert
	    float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
	    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		float32_t3 diffuseDL = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
		float32_t3 specularDL = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f,1.0f,1.0f);

		//環境マップ
		float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.cameraPos);
		float32_t3 reflectedVector = reflect(cameraToPosition,normalize(input.normal));
		float32_t4 environmentColor = gEnvironmentTex.Sample(gSampler,reflectedVector);
		
		//合計
		output.color.rgb = diffuseDL + specularDL + (environmentColor.rgb * 0.01f);
        output.color.a = gMaterial.color.a * textureColor.a;
	} else {
	    output.color = gMaterial.color * textureColor;
	}
	return output;
}