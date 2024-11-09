#pragma once
#include "BaseComputePipeline.h"

class SkinningComputePipeline : public BaseComputePipeline {
public:

	void Init() override;

	void preDispatch() override;

};

