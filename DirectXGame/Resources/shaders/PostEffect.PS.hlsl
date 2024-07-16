#include"PostEffect.hlsli"

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct DeadEffect {
	int32_t isEffect;
	float32_t param;
	float32_t root;
	float32_t brightness;
};

ConstantBuffer<DeadEffect> gDeadEffect : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
//Texture2D<float32_t4> gBloomTex : register(t1);

SamplerState gSampler : register(s0);
//SamplerState gBloomSampler : register(s1);

/*static const float32_t kKernel5x5[5][5] = {
	{1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f},
	{1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f},
	{1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f},
	{1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f},
	{1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f},
};*/

static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma){
	float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
	float denominator = 2.0f * PI * sigma * sigma;
	return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);

	float32_t4 baseColor = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);

	if(gDeadEffect.isEffect != 0){

		float32_t weight = 0.0f;
		float32_t kernel5x5[5][5];

		//ガウシアンフィルター(ぼかし処理)
		uint32_t widht, height;
		gTexture.GetDimensions(widht,height);
		float32_t2 uvStepSize = float32_t2(rcp(widht), rcp(height));

		for(int32_t x = 0; x < 5; ++x){
			for(int32_t y = 0; y < 5; ++y){
				kernel5x5[x][y] = gauss(x - 2, y - 2, 3.0f);
				weight += kernel5x5[x][y];

				float32_t2 texcoord = input.texcoord + float32_t2(x - 2, y - 2) * uvStepSize;
				float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
				baseColor.rgb += fetchColor * kernel5x5[x][y];
			}
		}
		baseColor.rgb *= rcp(weight);

		//グレースケール化
		float32_t grayScale = dot(baseColor.rgb, float32_t3(0.2125f,0.7154f,0.0712f));
		output.color.rgb = lerp(baseColor.rgb, grayScale, gDeadEffect.param);

		//ヴィネット
		float32_t2 correct = input.texcoord * (1.0f - input.texcoord.xy);
		float brightness = lerp(20.0f, gDeadEffect.brightness, gDeadEffect.param);
		float32_t vignette = correct.x * correct.y * brightness;
		float root = lerp(0.0f, gDeadEffect.root, gDeadEffect.param);
		vignette = saturate(pow(vignette, root));
		output.color.rgb *= vignette;
		
	}
	
	output.color.a = 1.0f;
	
	return output;
}