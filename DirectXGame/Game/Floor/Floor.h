#pragma once
#include "Game/Character/Character.h"

class Floor:public Character
{
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;

	void SetPos(Vector3 Pos) { world_.translation_ = Pos; world_.UpdateMatrix(); }
	void SetScale(Vector3 Scale) {
		world_.scale_ = Scale;
		world_.UpdateMatrix();
		Character::SetColliderSize({ colliderSize.x * Scale.x,colliderSize.y * Scale.y,colliderSize.z * Scale.z });
	}
private:

};
