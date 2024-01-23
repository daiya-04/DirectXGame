#pragma once
#include "Game/Character/Character.h"

class Box :public Character
{
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;

#pragma region
	void SetPos(Vector3 Pos) { world_.translation_ = Pos; world_.UpdateMatrix(); }
	void SetScale(Vector3 Scale) {
		world_.scale_ = Scale;
		world_.UpdateMatrix();
		Character::SetColliderSize({ colliderSize.x * Scale.x,colliderSize.y * Scale.y,colliderSize.z * Scale.z });
	}
#pragma endregion setter

private:

};