#include "ParticleInitComputePipeline.h"
#include "RootParameters.h"
#include "Log.h"
#include <cassert>

using namespace GPUParticleParam;

void ParticleInitComputePipeline::Init() {

	BaseComputePipeline::Init();

	ComPtr<IDxcBlob> computeShaderBlob = dxCompiler_->ShaderCompile(L"InitGPUParticle.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob != nullptr);

	//init用のcomputeShader設定
	D3D12_DESCRIPTOR_RANGE DescRangeForParticles[1] = {};
	DescRangeForParticles[0].BaseShaderRegister = 0; //0から始まる
	DescRangeForParticles[0].NumDescriptors = 1; //数は1つ
	DescRangeForParticles[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; //SRVを使う
	DescRangeForParticles[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE DescRangeForFreeListIndex[1] = {};
	DescRangeForFreeListIndex[0].BaseShaderRegister = 1; //0から始まる
	DescRangeForFreeListIndex[0].NumDescriptors = 1; //数は1つ
	DescRangeForFreeListIndex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; //SRVを使う
	DescRangeForFreeListIndex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE DescRangeForFreeList[1] = {};
	DescRangeForFreeList[0].BaseShaderRegister = 2; //0から始まる
	DescRangeForFreeList[0].NumDescriptors = 1; //数は1つ
	DescRangeForFreeList[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; //SRVを使う
	DescRangeForFreeList[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER rootParameters[static_cast<size_t>(InitRootParam::kParamNum)] = {};

	rootParameters[static_cast<size_t>(InitRootParam::kParticles)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(InitRootParam::kParticles)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	rootParameters[static_cast<size_t>(InitRootParam::kParticles)].DescriptorTable.pDescriptorRanges = DescRangeForParticles; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(InitRootParam::kParticles)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForParticles); //Tableで利用する数

	rootParameters[static_cast<size_t>(InitRootParam::kFreeListIndex)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(InitRootParam::kFreeListIndex)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	rootParameters[static_cast<size_t>(InitRootParam::kFreeListIndex)].DescriptorTable.pDescriptorRanges = DescRangeForFreeListIndex; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(InitRootParam::kFreeListIndex)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeListIndex); //Tableで利用する数

	rootParameters[static_cast<size_t>(InitRootParam::kFreeList)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(InitRootParam::kFreeList)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	rootParameters[static_cast<size_t>(InitRootParam::kFreeList)].DescriptorTable.pDescriptorRanges = DescRangeForFreeList; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(InitRootParam::kFreeList)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeList); //Tableで利用する数

	rootParameters[static_cast<size_t>(InitRootParam::kMaxNum)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<size_t>(InitRootParam::kMaxNum)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(InitRootParam::kMaxNum)].Descriptor.ShaderRegister = 0;

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

void ParticleInitComputePipeline::preDispatch() {
	BaseComputePipeline::preDispatch();
}
