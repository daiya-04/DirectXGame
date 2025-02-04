#pragma once
///---------------------------------------------------------------------------------------------
//
// LineGraphicsPipeline
// ライン用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------

#include "BaseGraphicsPipeline.h"


class LineGraphicsPipeline : public DaiEngine::BaseGraphicsPipeline {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 描画前
	/// </summary>
	void preDraw() override;

};

