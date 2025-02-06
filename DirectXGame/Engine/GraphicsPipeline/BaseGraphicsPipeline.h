#pragma once
///---------------------------------------------------------------------------------------------
//
// BaseGraphicsPipeline
// パイプラインステート生成の基底クラス
//
///---------------------------------------------------------------------------------------------


#include <d3d12.h>
#include <wrl.h>
#include "DXCompiler.h"
#include <array>

namespace DaiEngine {

	enum class BlendMode {
		kAlpha,
		kAdd,
		kSub,
		kMultiply,
		kScreen,
		kNone,

		BlendModeNum,
	};

	class BaseGraphicsPipeline {
	protected:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Init();
		/// <summary>
		/// 描画前
		/// </summary>
		virtual void preDraw(BlendMode blendMode = BlendMode::kAlpha);

	protected:

		D3D12_BLEND_DESC GetBlendDisc(BlendMode blendMode);

	protected:

		ID3D12Device* device_ = nullptr;
		DXCompiler* dxCompiler_ = nullptr;
		ComPtr<ID3D12RootSignature> rootSignature_;
		std::array<ComPtr<ID3D12PipelineState>, static_cast<size_t>(BlendMode::BlendModeNum)> pipelineState_;
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	};
}
