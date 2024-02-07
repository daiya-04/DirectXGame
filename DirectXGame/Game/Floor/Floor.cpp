#include "Floor.h"

void Floor::Init(std::vector<Object3d*> models)
{
	models_ = models;

	world_.Init();

	Character::SetColliderSize({ 5.0f,0.0f,5.0f });
}

void Floor::Update()
{
	world_.UpdateMatrix();
	Character::ColliderUpdate();
}

void Floor::Draw(const Camera& camera)
{
	for (Object3d * model : models_) {
		model->Draw(world_,camera);
	}
}
