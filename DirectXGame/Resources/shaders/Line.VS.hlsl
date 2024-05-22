#include"Line.hlsli"

struct LineData{
	float32_t4x4 wvp;
	float32_t4 color;
};

StructuredBuffer<LineData> gLineData : register(t0);

static const float32_t4 kPositions[2u] = {
    {0.0f,0.0f,0.0f,1.0f},
    {1.0f,0.0f,0.0f,1.0f}
};

VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	output.position = mul(kPositions[vertexID],gLineData[instanceId].wvp);
    output.color = gLineData[instanceId].color;
;
	return output;
}