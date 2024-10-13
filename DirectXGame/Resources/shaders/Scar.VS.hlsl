#include"Sprite.hlsli"

struct WorldTransform {
	float32_t4x4 world;
};
ConstantBuffer<WorldTransform> gWorldTransform : register(b0);

struct Camera {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 cameraPos;
};
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(input.position, mul(gWorldTransform.world, mul(gCamera.view, gCamera.projection)));
	output.texcoord = input.texcoord;
	return output;
}