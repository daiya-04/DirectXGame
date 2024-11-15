#include "Particle.hlsli"
#include "Math.hlsli"
#include "ParticleInfo.hlsli"

#define Billboard 0
#define HorizontalBillboard 1
#define NON 2

StructuredBuffer<Particle> gParticles : register(t0);

struct PerView {
	float32_t4x4 viewProjMat;
	float32_t4x4 billboardMat;
};

ConstantBuffer<PerView> gPerView : register(b0);


struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	Particle particle = gParticles[instanceId];

	float32_t4x4 worldMat = MakeIdentity44();
	float32_t4x4 rotateMat = MakeRotateMat(particle.rotate);

	if(particle.billboardType == Billboard){
		rotateMat = mul(MakeRotateMat(particle.rotate), gPerView.billboardMat);
	}else if(particle.billboardType == HorizontalBillboard){
		rotateMat = mul(MakeRotateMat(particle.rotate), MakeRotateMat(float32_t3( radians(90.0f), 0.0f, 0.0f )));
	}



	worldMat = mul(mul(MakeScaleMat(particle.scale), rotateMat), MakeTranslateMat(particle.translate));

	output.position = mul(input.position, mul(worldMat, gPerView.viewProjMat));
	output.texcoord = input.texcoord;
	output.color = particle.color;
	
	return output;
}