#pragma once
#include "BaseGraphicsPipeline.h"

class ParticleGraphicsPipeline : public BaseGraphicsPipeline {
public:

	void Init() override;

	void preDraw() override;

};

