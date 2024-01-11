#pragma once

#include "ModelManager.h"
#include "WorldTransform.h"

class Character
{
public:
	
	virtual void Init(uint32_t model = 0) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

private:

	uint32_t model_;
	WorldTransform world_;

};
