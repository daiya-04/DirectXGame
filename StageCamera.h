#pragma once
#include "ViewProjection.h"
#include "./Math.h"
#include "Input.h"

class StageCamera
{
private:

	ViewProjection viewProjection_;
	Vector3 stageCenter_ = { 4.0f,-8.0f,4.0f };

public:

	void Initialize();

	void Update();

	ViewProjection& GetViewProjection() { return viewProjection_; }

};

