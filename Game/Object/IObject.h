#pragma once

#include "../Math/Math.h"
#include "../../ViewProjection.h"


class IObject
{
protected:

	ViewProjection* viewProjection_ = nullptr;

public:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

public:

	void SetViewProjection(ViewProjection* view) { viewProjection_ = view; }

};