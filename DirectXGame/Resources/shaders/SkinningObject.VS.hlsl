#include"SkinningObject.hlsli"

struct WorldTransform {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
};
ConstantBuffer<WorldTransform> gWorldTransform : register(b1);

struct Camera {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 cameraPos;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct Well {
	float32_t4x4 skeletonSpaceMat;
	float32_t4x4 skeletonSpaceInverseTransposeMat;
};

StructuredBuffer<Well> gMatrixPalette : register(t1);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 weight : WEIGHT0;
	int32_t4 index : INDEX0;
};

struct Skinned {
	float32_t4 position;
	float32_t3 normal;
};

Skinned Skinning(VertexShaderInput input){
	Skinned skinned;
	//位置の変換
	skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMat) * input.weight.x;
	skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMat) * input.weight.y;
	skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMat) * input.weight.z;
	skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMat) * input.weight.w;
	skinned.position.w = 1.0f;

	//法線の変換
	skinned.normal = mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMat) * input.weight.x;
	skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMat) * input.weight.y;
	skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMat) * input.weight.z;
	skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMat) * input.weight.w;
	skinned.normal = normalize(skinned.normal);

	return skinned;
}

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	Skinned skinned = Skinning(input);

	output.position = mul(skinned.position, mul(gWorldTransform.world, mul(gCamera.view, gCamera.projection)));
	output.worldPosition = mul(skinned.position, gWorldTransform.world).xyz;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(skinned.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose));

	return output;
}