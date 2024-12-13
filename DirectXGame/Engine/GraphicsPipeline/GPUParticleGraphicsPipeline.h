#pragma once
///---------------------------------------------------------------------------------------------
//
// GPUParticleGraphicsPipeline
// GPUパーティクル用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------


#include "BaseGraphicsPipeline.h"

class GPUParticleGraphicsPipeline : public BaseGraphicsPipeline {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 描画前
	/// </summary>
	void preDraw() override;
	/// <summary>
	/// スクリーンブレンド用パイプラインステート生成
	/// </summary>
	void screenPSCreate();

};

