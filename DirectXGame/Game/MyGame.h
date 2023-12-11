#pragma once
#include "Framework.h"


class MyGame : public DSFramework {
public:

	void Init()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

};

