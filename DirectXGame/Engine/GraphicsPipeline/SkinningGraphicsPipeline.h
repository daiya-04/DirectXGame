#pragma once
///---------------------------------------------------------------------------------------------
//
// SkinningGraphicsPipeline
// スキニングオブジェクト用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------


#include "BaseGraphicsPipeline.h"

class SkinningGraphicsPipeline : public DaiEngine::BaseGraphicsPipeline {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 描画前
	/// </summary>
	void preDraw(DaiEngine::BlendMode blendMode = DaiEngine::BlendMode::kAlpha) override;

};

