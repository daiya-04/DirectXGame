#pragma once

#include "Game/Character/Character.h"

static int sangoId = 0;

class Sango : public Character
{
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;

#pragma region
	void SetPos(Vector3 Pos) {
		world_.translation_ = Pos; world_
			.UpdateMatrix();
	}
	void SetDirection(Vector3 Direction) { Direction_ = Direction;};
	void HitPlayer() { IsAlreadyHit = true; }
	void NotHitPlayer() { IsAlreadyHit = false; }
#pragma region setter

#pragma region
	bool GetIsAlreadyHit()const { return IsAlreadyHit; }
	int GetSangoId()const { return Id; }
	Vector3 GetDirection()const { return Direction_; }
#pragma region getter

private:
	Vector3 Direction_ = {0.0f,0.0f,0.0f};
	
	bool IsAlreadyHit = false;
	int Id;
};