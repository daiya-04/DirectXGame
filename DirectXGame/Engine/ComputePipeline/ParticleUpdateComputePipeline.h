#pragma once
///---------------------------------------------------------------------------------------------
//
// ParticleUpdateComputePipeline
// パーティクル更新用のパイプラインステートの生成
//
///---------------------------------------------------------------------------------------------


#include "BaseComputePipeline.h"

class ParticleUpdateComputePipeline : public DaiEngine::BaseComputePipeline {
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

