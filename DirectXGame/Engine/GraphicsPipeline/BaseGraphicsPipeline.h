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

namespace DaiEngine {
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
		virtual void preDraw();
		/// <summary>
		/// スクリーンブレンド描画前(苦し紛れ)
		/// </summary>
		void preDrawScreen();

	protected:

		ID3D12Device* device_ = nullptr;
		DXCompiler* dxCompiler_ = nullptr;
		ComPtr<ID3D12RootSignature> rootSignature_;
		ComPtr<ID3D12PipelineState> pipelineState_;
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		ComPtr<ID3D12RootSignature> screenRSig_;
		ComPtr<ID3D12PipelineState> screenPS_;

	};
}
