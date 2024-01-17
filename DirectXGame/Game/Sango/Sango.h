#pragma once

#include "Game/Character/Character.h"

class Sango : public Character
{
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;
#pragma region
	void SetPos(Vector3 Pos) { world_.translation_ = Pos; }
	void SetDirection(Vector3 Direction) { Direction_ = Direction;};
#pragma region
private:
	Vector3 Direction_ = {0.0f,0.0f,0.0f};
};