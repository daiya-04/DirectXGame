#pragma once
#include "Framework.h"
#include <memory>

class MyGame : public DSFramework {
public:

	void Init()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

private:

	

};

