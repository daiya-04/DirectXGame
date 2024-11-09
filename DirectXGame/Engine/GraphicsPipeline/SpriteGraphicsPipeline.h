#pragma once
#include "BaseGraphicsPipeline.h"

class SpriteGraphicsPipeline : public BaseGraphicsPipeline {
public:

	void Init() override;

	void preDraw() override;

};