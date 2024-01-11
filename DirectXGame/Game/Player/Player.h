#pragma once
#include "Game/Character/Character.h"

class Player:public Character
{
public:
	void Init(uint32_t model = 0)override;
	void Update()override;
	void Draw()override;

private:

};