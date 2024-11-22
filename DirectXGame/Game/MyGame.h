#pragma once
#include "Framework.h"
#include <memory>
#include "PostEffect.h"

class MyGame : public DSFramework {
public:

	void Init()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

};

