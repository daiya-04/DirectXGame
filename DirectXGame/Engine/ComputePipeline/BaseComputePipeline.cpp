#include "BaseComputePipeline.h"
#include "DirectXCommon.h"

void BaseComputePipeline::Init() {

	device_ = DirectXCommon::GetInstance()->GetDevice();
	dxCompiler_ = DXCompiler::GetInstance();

}


void BaseComputePipeline::preDispatch() {

	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->SetComputeRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());

}
