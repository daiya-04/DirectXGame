#include"Particle.hlsli"

struct ParticleGPU {
	float32_t4x4 world;
	float32_t4 color;
};

StructuredBuffer<ParticleGPU> gParticle : register(t0);

struct Camera {
	float32_t4x4 view;
	float32_t4x4 projection;
};

ConstantBuffer<Camera> gCamera : register(b1);


struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	output.position = mul(input.position, mul(gParticle[instanceId].world, mul(gCamera.view, gCamera.projection)));
	output.texcoord = input.texcoord;
	output.color = gParticle[instanceId].color;
	return output;
}