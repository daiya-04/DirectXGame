#include "BaseGraphicsPipeline.h"
#include "DirectXCommon.h"

namespace DaiEngine {
	void BaseGraphicsPipeline::Init() {

		device_ = DirectXCommon::GetInstance()->GetDevice();
		dxCompiler_ = DXCompiler::GetInstance();

	}

	void BaseGraphicsPipeline::preDraw(BlendMode blendMode) {

		ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
		//コマンドリストにセット
		commandList->SetGraphicsRootSignature(rootSignature_.Get());
		commandList->SetPipelineState(pipelineState_[static_cast<size_t>(blendMode)].Get());
		commandList->IASetPrimitiveTopology(primitiveTopology_);
	}

	D3D12_BLEND_DESC BaseGraphicsPipeline::GetBlendDisc(BlendMode blendNode) {

		D3D12_BLEND_DESC blendDesc{};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;

		switch (blendNode) {
			case BlendMode::kAlpha:
				blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				break;
			case BlendMode::kAdd:
				blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				break;
			case BlendMode::kSub:
				blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
				blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				break;
			case BlendMode::kMultiply:
				blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
				break;
			case BlendMode::kScreen:
				blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
				blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				break;
			case BlendMode::kNone:
				blendDesc.RenderTarget[0].BlendEnable = FALSE;
				break;
		}

		//α値のブレンド設定で基本的に使わないからいじらない
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


		return blendDesc;
	}
}
