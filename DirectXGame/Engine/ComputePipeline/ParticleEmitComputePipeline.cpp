#include "ParticleEmitComputePipeline.h"
#include "RootParameters.h"
#include "Log.h"
#include <cassert>

using namespace GPUParticleParam;

void ParticleEmitComputePipeline::Init() {

	BaseComputePipeline::Init();

	ComPtr<IDxcBlob> computeShaderBlob = dxCompiler_->ShaderCompile(L"EmitParticle.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob != nullptr);

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

	//Emitter用ComputeShaderの設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER rootParameters[static_cast<size_t>(EmitterRootParam::kParamNum)] = {};

	rootParameters[static_cast<size_t>(EmitterRootParam::kParticles)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(EmitterRootParam::kParticles)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	rootParameters[static_cast<size_t>(EmitterRootParam::kParticles)].DescriptorTable.pDescriptorRanges = DescRangeForParticles; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(EmitterRootParam::kParticles)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForParticles); //Tableで利用する数

	rootParameters[static_cast<size_t>(EmitterRootParam::kEmitter)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<size_t>(EmitterRootParam::kEmitter)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(EmitterRootParam::kEmitter)].Descriptor.ShaderRegister = 0;

	rootParameters[static_cast<size_t>(EmitterRootParam::kPerFrame)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<size_t>(EmitterRootParam::kPerFrame)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(EmitterRootParam::kPerFrame)].Descriptor.ShaderRegister = 1;

	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeListIndex)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeListIndex)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeListIndex)].DescriptorTable.pDescriptorRanges = DescRangeForFreeListIndex; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeListIndex)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeListIndex); //Tableで利用する数

	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeList)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeList)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeList)].DescriptorTable.pDescriptorRanges = DescRangeForFreeList; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(EmitterRootParam::kFreeList)].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeList); //Tableで利用する数

	rootParameters[static_cast<size_t>(EmitterRootParam::kMaxNum)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[static_cast<size_t>(EmitterRootParam::kMaxNum)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(EmitterRootParam::kMaxNum)].Descriptor.ShaderRegister = 2;

	rootParameters[static_cast<size_t>(EmitterRootParam::kOverLifeTime)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[static_cast<size_t>(EmitterRootParam::kOverLifeTime)].Descriptor.ShaderRegister = 3;
	rootParameters[static_cast<size_t>(EmitterRootParam::kOverLifeTime)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

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

void ParticleEmitComputePipeline::preDispatch() {
	BaseComputePipeline::preDispatch();
}
