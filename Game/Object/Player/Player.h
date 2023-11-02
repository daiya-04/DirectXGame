#pragma once

#include "../IObject.h"

class Player : public IObject
{
private:



public:

	void Initialize() override;

	void Update() override;

	void Draw() override;

};