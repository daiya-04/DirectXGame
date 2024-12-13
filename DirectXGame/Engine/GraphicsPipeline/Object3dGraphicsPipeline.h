#pragma once
///---------------------------------------------------------------------------------------------
//
// Object3dGraphicsPipeline
// 3Dオブジェクト用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------


#include "BaseGraphicsPipeline.h"

class Object3dGraphicsPipeline : public BaseGraphicsPipeline {
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

