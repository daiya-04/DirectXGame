#pragma once
#include "BaseComputePipeline.h"

class ParticleUpdateComputePipeline : public BaseComputePipeline {
public:

	void Init() override;

	void preDispatch() override;

};

