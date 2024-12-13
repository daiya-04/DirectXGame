#include "BaseGraphicsPipeline.h"
#include "DirectXCommon.h"

void BaseGraphicsPipeline::Init() {

	device_ = DirectXCommon::GetInstance()->GetDevice();
	dxCompiler_ = DXCompiler::GetInstance();

}

void BaseGraphicsPipeline::preDraw() {

	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	//コマンドリストにセット
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());
	commandList->IASetPrimitiveTopology(primitiveTopology_);
}

void BaseGraphicsPipeline::preDrawScreen() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	//コマンドリストにセット
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(screenPS_.Get());
	commandList->IASetPrimitiveTopology(primitiveTopology_);
}
