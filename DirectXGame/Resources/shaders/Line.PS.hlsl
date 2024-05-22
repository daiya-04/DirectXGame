#include"Line.hlsli"

float32_t4 main(VertexShaderOutput output) : SV_TARGET0{
	return output.color;
}