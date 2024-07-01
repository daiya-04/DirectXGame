
struct Well {
	float32_t4x4 skeletonSpaceMat;
	float32_t4x4 skeletonSpaceInverseTransposeMat;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);

struct Vertex {
	float32_t4 position;
	float32_t2 texcoord;
	float32_t3 normal;
};

StructuredBuffer<Vertex> gInputVertex : register(t1);
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

struct VertexInfluence {
	float32_t4 weight;
	int32_t4 index;
};

StructuredBuffer<VertexInfluence> gInfluences : register(t2);

struct SkinningInformation {
	uint32_t numVertices;
};

ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);


[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID){

	uint32_t vertexIndex = DTid.x;
	if(vertexIndex < gSkinningInformation.numVertices){
		Vertex input = gInputVertex[vertexIndex];
		VertexInfluence influence = gInfluences[vertexIndex];

		Vertex skinned;
		skinned.texcoord = input.texcoord;

		//位置の変換
		skinned.position = mul(input.position, gMatrixPalette[influence.index.x].skeletonSpaceMat) * influence.weight.x;
		skinned.position += mul(input.position, gMatrixPalette[influence.index.y].skeletonSpaceMat) * influence.weight.y;
		skinned.position += mul(input.position, gMatrixPalette[influence.index.z].skeletonSpaceMat) * influence.weight.z;
		skinned.position += mul(input.position, gMatrixPalette[influence.index.w].skeletonSpaceMat) * influence.weight.w;
		skinned.position.w = 1.0f;
		
		//法線の変換
		skinned.normal = mul(input.normal, (float32_t3x3)gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMat) * influence.weight.x;
		skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMat) * influence.weight.y;
		skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMat) * influence.weight.z;
		skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMat) * influence.weight.w;
		skinned.normal = normalize(skinned.normal);

		gOutputVertices[vertexIndex] = skinned;
	}

}
