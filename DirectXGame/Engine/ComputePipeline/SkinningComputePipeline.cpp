#include "SkinningComputePipeline.h"
#include "RootParameters.h"
#include "Log.h"
#include <cassert>

using namespace SkinningObjParam;

void SkinningComputePipeline::Init() {

	BaseComputePipeline::Init();

	ComPtr<IDxcBlob> computeShaderBlob = dxCompiler_->ShaderCompile(L"SkinningObject.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE DescRangeForPalette[1] = {};
	DescRangeForPalette[0].BaseShaderRegister = 0; //0から始まる
	DescRangeForPalette[0].NumDescriptors = 1; //数は1つ
	DescRangeForPalette[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	DescRangeForPalette[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE DescRangeForInputVertex[1] = {};
	DescRangeForInputVertex[0].BaseShaderRegister = 1; //0から始まる
	DescRangeForInputVertex[0].NumDescriptors = 1; //数は1つ
	DescRangeForInputVertex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	DescRangeForInputVertex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE DescRangeForInfluence[1] = {};
	DescRangeForInfluence[0].BaseShaderRegister = 2; //0から始まる
	DescRangeForInfluence[0].NumDescriptors = 1; //数は1つ
	DescRangeForInfluence[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	DescRangeForInfluence[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE DescRangeForOutputVertex[1] = {};
	DescRangeForOutputVertex[0].BaseShaderRegister = 0; //0から始まる
	DescRangeForOutputVertex[0].NumDescriptors = 1; //数は1つ
	DescRangeForOutputVertex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; //SRVを使う
	DescRangeForOutputVertex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_ROOT_PARAMETER rootParameters[static_cast<size_t>(ComputeRootParam::kParamNum)] = {};

	rootParameters[static_cast<size_t>(ComputeRootParam::kPalette)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[static_cast<size_t>(ComputeRootParam::kPalette)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(ComputeRootParam::kPalette)].DescriptorTable.pDescriptorRanges = DescRangeForPalette;
	rootParameters[static_cast<size_t>(ComputeRootParam::kPalette)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForPalette);

	rootParameters[static_cast<size_t>(ComputeRootParam::kInputVertex)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[static_cast<size_t>(ComputeRootParam::kInputVertex)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(ComputeRootParam::kInputVertex)].DescriptorTable.pDescriptorRanges = DescRangeForInputVertex;
	rootParameters[static_cast<size_t>(ComputeRootParam::kInputVertex)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForInputVertex);

	rootParameters[static_cast<size_t>(ComputeRootParam::kInfluence)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[static_cast<size_t>(ComputeRootParam::kInfluence)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(ComputeRootParam::kInfluence)].DescriptorTable.pDescriptorRanges = DescRangeForInfluence;
	rootParameters[static_cast<size_t>(ComputeRootParam::kInfluence)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForInfluence);

	rootParameters[static_cast<size_t>(ComputeRootParam::kOutputVertex)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[static_cast<size_t>(ComputeRootParam::kOutputVertex)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(ComputeRootParam::kOutputVertex)].DescriptorTable.pDescriptorRanges = DescRangeForOutputVertex;
	rootParameters[static_cast<size_t>(ComputeRootParam::kOutputVertex)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForOutputVertex);

	rootParameters[static_cast<size_t>(ComputeRootParam::kSkinningInfo)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<size_t>(ComputeRootParam::kSkinningInfo)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(ComputeRootParam::kSkinningInfo)].Descriptor.ShaderRegister = 0;

	descriptionRootSignature.pParameters = rootParameters;   //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);  //配列の長さ


	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.pRootSignature = rootSignature_.Get();
	pipelineStateDesc.CS = { computeShaderBlob->GetBufferPointer(),computeShaderBlob->GetBufferSize() };

	hr = device_->CreateComputePipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));

}

void SkinningComputePipeline::preDispatch() {
	BaseComputePipeline::preDispatch();
}
