#pragma once
#include "BaseGraphicsPipeline.h"

class Object3dGraphicsPipeline : public BaseGraphicsPipeline {
public:

	void Init() override;

	void preDraw() override;

};

