#pragma once
#include "BaseGraphicsPipeline.h"

class SkinningGraphicsPipeline : public BaseGraphicsPipeline {
public:

	void Init() override;

	void preDraw() override;

};

