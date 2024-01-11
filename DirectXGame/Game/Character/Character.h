#pragma once

#include "ModelManager.h"
#include "WorldTransform.h"

class Character
{
public:
	
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

private:

};
