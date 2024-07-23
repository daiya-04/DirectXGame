#include"PostEffect.hlsli"
#include "ColorConversion.hlsli"

struct HSVFactor {
	float32_t hue;
	float32_t saturation;
	float32_t value;
};

ConstantBuffer<HSVFactor> gFactor : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

float32_t WrapValue(float32_t value, float32_t minRange, float32_t maxRange){

	float32_t range = maxRange - minRange;
	float32_t modValue = fmod(value - minRange, range);

	if(modValue < 0.0f){
		modValue += range;
	}

	return minRange + modValue;
}

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	float32_t3 hsv = RGBToHSV(textureColor.rgb);

	hsv.x += gFactor.hue;
	hsv.y += gFactor.saturation;
	hsv.z += gFactor.value;

	hsv.x = WrapValue(hsv.x, 0.0f, 1.0f);
	hsv.y = saturate(hsv.y);
	hsv.z = saturate(hsv.z);

	float32_t3 rgb = HSVToRGB(hsv);
	
	output.color.rgb = rgb;
	output.color.a = 1.0f;

	return output;
}