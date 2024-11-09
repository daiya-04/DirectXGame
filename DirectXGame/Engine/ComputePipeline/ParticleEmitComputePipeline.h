#pragma once
#include "BaseComputePipeline.h"

class ParticleEmitComputePipeline : public BaseComputePipeline {
public:

	void Init() override;

	void preDispatch() override;

};

