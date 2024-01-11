#pragma once
#include "Game/Character/Character.h"

class Player:public Character
{
public:
	void Init(std::vector<uint32_t> models = { 0 })override;
	void Update()override;
	void Draw()override;

private:

};