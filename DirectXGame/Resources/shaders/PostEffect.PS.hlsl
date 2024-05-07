#include"PostEffect.hlsli"

struct Material{
	float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct GrayScale {
	int32_t isGrayScale;
	float32_t param;
};

ConstantBuffer<GrayScale> gGrayScale : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
//Texture2D<float32_t4> gBloomTex : register(t1);

SamplerState gSampler : register(s0);
//SamplerState gBloomSampler : register(s1);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);

	float32_t3 baseColor = output.color.rgb;

	if(gGrayScale.isGrayScale != 0){
		float32_t grayScale = dot(baseColor, float32_t3(0.2125f,0.7154f,0.0712f));
		output.color.rgb = lerp(baseColor, grayScale, gGrayScale.param);
	}
	

	return output;
}