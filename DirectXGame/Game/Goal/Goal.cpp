#include "Goal.h"

void Goal::Init(std::vector<Object3d*> models)
{
	models_ = models;

	world_.Init();

	Character::SetColliderSize({ 1.0f,1.0f,1.0f });
}

void Goal::Update()
{
	world_.UpdateMatrix();
	Character::ColliderUpdate();
}

void Goal::Draw(const Camera& camera)
{
	for (Object3d* model : models_) {
		model->Draw(world_, camera);
	}
}
