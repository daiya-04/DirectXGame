#pragma once
///---------------------------------------------------------------------------------------------
//
// ParticleGraphicsPipeline
// パーティクル用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------


#include "BaseGraphicsPipeline.h"

class ParticleGraphicsPipeline : public BaseGraphicsPipeline {
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
