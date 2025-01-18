#include "Box.h"

void Box::Init(std::vector<Object3d*> models)
{
	models_ = models;


	world_.Init();

	Character::SetColliderSize({ 1.0f,1.0f,1.0f });
}

void Box::Update()
{
	world_.UpdateMatrix();
	Character::ColliderUpdate();
}

void Box::Draw(const Camera& camera)
{
	for (Object3d* model : models_) {
		model->Draw(world_, camera);
	}
}
