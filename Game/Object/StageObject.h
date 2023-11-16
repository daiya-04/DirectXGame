#pragma once

#include "../Math/Math.h"

class StageObject
{
public:

	enum class Element : size_t
	{
		kNone = 0,
		kPlayer,
		kBody,
		kBlock,

		kCount,
	};

	Vector3 position_;
	Vector3 direction_;
	Element element_;

public:

	void Initialize();

	void Update();

	void Draw();


};