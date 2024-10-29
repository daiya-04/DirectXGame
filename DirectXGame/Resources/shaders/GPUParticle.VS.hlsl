#include "Particle.hlsli"
#include "Math.hlsli"
#include "ParticleInfo.hlsli"

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
	/*float32_t4x4 worldMat = gPerView.billboardMat;
	worldMat[0] *= particle.scale.x;
	worldMat[1] *= particle.scale.y;
	worldMat[2] *= particle.scale.z;
	worldMat[3].xyz = particle.translate;*/

	float32_t4x4 worldMat = MakeIdentity44();
	float32_t4x4 rotateMat = mul(MakeRotateMat(particle.rotate), gPerView.billboardMat);
	worldMat = mul(mul(MakeScaleMat(particle.scale), rotateMat), MakeTranslateMat(particle.translate));

	output.position = mul(input.position, mul(worldMat, gPerView.viewProjMat));
	output.texcoord = input.texcoord;
	output.color = particle.color;
	
	return output;
}