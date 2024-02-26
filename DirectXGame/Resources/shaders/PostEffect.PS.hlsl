#include"PostEffect.hlsli"

struct Material{
	float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
//Texture2D<float32_t4> gBloomTex : register(t1);

SamplerState gSampler : register(s0);
//SamplerState gBloomSampler : register(s1);

float Gaussian(float32_t2 drawUV, float32_t2 pickUV, float sigma){
	float d = distance(drawUV, pickUV);
	return exp(-(d * d) / (2 * 	sigma * sigma));
}

float smoothStep(float edge0, float edge1, float x) {
    x = saturate((x - edge0) / (edge1 - edge0));
    return x * x * (3.0 - 2.0 * x);
}

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	float32_t4 highLumiColor = gTexture.Sample(gSampler, input.texcoord);
	float grayScale = highLumiColor.r * 0.299 * highLumiColor.g * 0.587 * highLumiColor.b * 0.114;
	float extract = smoothStep(0.4, 0.9, grayScale);
	highLumiColor.rgb *= extract;

	// ガウシアンブラーを適用
    float4 blurredColor = float4(0.0, 0.0, 0.0, 0.0);
    float totalWeight = 0.0;
    float sigma = 0.01;
    float stepWidth = 0.002;

    for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth) {
        for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth) {
            float2 pickUV = input.texcoord + float2(px, py);
            float weight = Gaussian(input.texcoord, pickUV, sigma);
            blurredColor.rgb += gTexture.Sample(gSampler, pickUV) * weight;
            totalWeight += weight;
        }
    }

    blurredColor /= totalWeight;

    // 3. オリジナルのテクスチャとガウシアンブラーを適用した結果を合成
    output.color = gTexture.Sample(gSampler, input.texcoord) + highLumiColor + blurredColor;

	output.color.rgb *= 5.0;
	
	/*float totalWeight = 0, sigma = 0.005, stepWidth = 0.001;
	float32_t4 sampleColor = float32_t4(0.0, 0.0, 0.0, 0.0);

	for(float py = -sigma * 2; py <= sigma * 2; py += stepWidth){
		for(float px = -sigma * 2; px <= sigma * 2; px += stepWidth){
			float32_t2 pickUV = input.texcoord + float32_t2(px, py);
			float weight = Gaussian(input.texcoord, pickUV, sigma);
			sampleColor += gTexture.Sample(gSampler, pickUV) * weight;
			totalWeight += weight;
		}
	}

	sampleColor /= totalWeight;

	output.color.rgb = sampleColor.rgb * 2;
	output.color.a = sampleColor.a;*/
	
	return output;
}