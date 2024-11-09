#pragma once
#include "BaseGraphicsPipeline.h"

class SkyBoxGraphicsPipeline : public BaseGraphicsPipeline {
public:

	void Init() override;

	void preDraw() override;

};

