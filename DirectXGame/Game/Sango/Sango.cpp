#include "Sango.h"

void Sango::Init(std::vector<Object3d*> models)
{
	Id = sangoId++;
	models_ = models;

	world_.Init();

	Character::SetColliderSize({ 3.0f,3.0f,5.0f });
}

void Sango::Update()
{
	world_.UpdateMatrix();
	Character::ColliderUpdate();
}

void Sango::Draw(const Camera& camera)
{
	for (Object3d* model : models_) {
		model->Draw(world_,camera);
	}
}
