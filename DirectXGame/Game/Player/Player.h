#pragma once
#include "Game/Character/Character.h"

class Player:public Character
{
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw()override;

private:

};