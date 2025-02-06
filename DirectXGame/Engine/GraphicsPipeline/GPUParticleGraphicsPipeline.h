#pragma once
///---------------------------------------------------------------------------------------------
//
// GPUParticleGraphicsPipeline
// GPUパーティクル用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------


#include "BaseGraphicsPipeline.h"

class GPUParticleGraphicsPipeline : public DaiEngine::BaseGraphicsPipeline {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 描画前
	/// </summary>
	void preDraw(DaiEngine::BlendMode blendMode = DaiEngine::BlendMode::kAdd) override;

};

