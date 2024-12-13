#pragma once
///---------------------------------------------------------------------------------------------
//
// ParticleEmitComputePipeline
// パーティクル発生用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------

#include "BaseComputePipeline.h"

class ParticleEmitComputePipeline : public BaseComputePipeline {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// ディスパッチ起動前
	/// </summary>
	void preDispatch() override;

};

