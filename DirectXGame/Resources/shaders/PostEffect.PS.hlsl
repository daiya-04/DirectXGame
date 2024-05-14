#include"PostEffect.hlsli"

struct Material{
	float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct DeadEffect {
	int32_t isEffect;
	float32_t param;
	float32_t root;
	float32_t brightness;
};

ConstantBuffer<DeadEffect> gDeadEffect : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
//Texture2D<float32_t4> gBloomTex : register(t1);

SamplerState gSampler : register(s0);
//SamplerState gBloomSampler : register(s1);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);

	float32_t3 baseColor = output.color.rgb;

	if(gDeadEffect.isEffect != 0){
		float32_t grayScale = dot(baseColor, float32_t3(0.2125f,0.7154f,0.0712f));
		output.color.rgb = lerp(baseColor, grayScale, gDeadEffect.param);
		float32_t2 correct = input.texcoord * (1.0f - input.texcoord.xy);
		float brightness = lerp(20.0f, gDeadEffect.brightness, gDeadEffect.param);
		float32_t vignette = correct.x * correct.y * brightness;
		float root = lerp(0.0f, gDeadEffect.root, gDeadEffect.param);
		vignette = saturate(pow(vignette, root));
		output.color.rgb *= vignette;
		
	}
	

	return output;
}