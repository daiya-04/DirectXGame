#pragma once

#include "../Math/Math.h"

class IObject
{
protected:




public:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};