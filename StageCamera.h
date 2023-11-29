#pragma once
#include "ViewProjection.h"
#include "./Math.h"
#include "Input.h"


class StageCamera
{
private:

	ViewProjection viewProjection_;
	Vector3 stageCenter_ = { 4.0f,-8.0f,4.0f };

	float rotateParam_ = 1.0f;
	float lengthParam_ = 0.0f;

	float end_ = 0.0f;
	float start_ = 0.0f;

public:

	void Initialize();

	void Update();

	ViewProjection& GetViewProjection() { return viewProjection_; }

};

