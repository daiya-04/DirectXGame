#pragma once

#include "ModelManager.h"
#include "WorldTransform.h"
#include "Viewpro.h"

class Character
{
public:
	
	virtual void Init(std::vector<uint32_t> models = {0}) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

private:

	std::vector<uint32_t> models_;
	WorldTransform world_;

};
