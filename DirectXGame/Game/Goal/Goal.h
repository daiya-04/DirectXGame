#pragma once
#include "ImGuiManager.h"

#include "Game/Character/Character.h"
class Goal :public Character {
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;

private:



};