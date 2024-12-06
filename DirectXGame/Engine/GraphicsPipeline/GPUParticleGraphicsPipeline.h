#pragma once
#include "BaseGraphicsPipeline.h"

class GPUParticleGraphicsPipeline : public BaseGraphicsPipeline {
public:

	void Init() override;

	void preDraw() override;

	void screenPSCreate();

};

