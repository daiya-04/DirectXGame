#include"PostEffect.hlsli"

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

static const float32_t kPHKernel[3][3] = {
	{-1.0f / 6.0f, 0.0f, 1.0f / 6.0f},
	{-1.0f / 6.0f, 0.0f, 1.0f / 6.0f},
	{-1.0f / 6.0f, 0.0f, 1.0f / 6.0f},
};

static const float32_t kPVKernel[3][3] = {
	{-1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f},
	{0.0f, 0.0f, 0.0f},
	{1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f},
};

float32_t Luminance(float32_t3 v){
	return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	uint32_t widht, height;
	gTexture.GetDimensions(widht,height);
	float32_t2 uvStepSize = float32_t2(rcp(widht), rcp(height));
	
	float32_t2 difference = float32_t2(0.0f, 0.0f);

	for(int32_t x = 0; x < 3; ++x){
		for(int32_t y = 0; y < 3; ++y){
			float32_t2 texcoord = input.texcoord + float32_t2(x - 1, y - 1) * uvStepSize;
			float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
			float32_t luminance = Luminance(fetchColor);
			difference.x += luminance * kPHKernel[x][y];
			difference.y += luminance * kPVKernel[x][y];
		}
	}

	float32_t weight = length(difference);
	weight = saturate(weight * 6.0f);

	output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
	output.color.a = 1.0f;

	return output;
}