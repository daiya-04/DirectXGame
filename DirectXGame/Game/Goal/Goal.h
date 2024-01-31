#pragma once
#include "ImGuiManager.h"

#include "Game/Character/Character.h"
class Goal :public Character {
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;
	void SetPos(Vector3 Position) {
		world_.translation_ = Position;
		world_.UpdateMatrix();
	}

private:



};