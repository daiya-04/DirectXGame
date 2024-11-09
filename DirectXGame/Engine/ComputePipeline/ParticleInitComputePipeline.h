#pragma once
#include "BaseComputePipeline.h"

class ParticleInitComputePipeline : public BaseComputePipeline {
public:

	void Init() override;

	void preDispatch() override;

};

