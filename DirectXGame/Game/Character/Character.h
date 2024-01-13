#pragma once

#include "Object3d.h"
#include "ModelManager.h"
#include "WorldTransform.h"
#include "Hit.h"

class Character
{
public:
	
	virtual void Init(std::vector<Object3d*> models) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	AABB GetAABB() { return collider; }

protected:

	std::vector<Object3d*> models_;
	WorldTransform world_;

private:
	AABB collider;


};
