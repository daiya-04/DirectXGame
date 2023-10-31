#include "IObject.h"

Input* IObject::input_ = nullptr;
ViewProjection* IObject::viewProjection_ = nullptr;

void IObject::StaticInitialize(Input* input, ViewProjection* vp)
{
	assert(input);
	input_ = input;
	viewProjection_ = vp;
}

void IObject::Initialize()
{
	transformBase_.rotation_ = { 0.0f,0.0f,0.0f };
	transformBase_.translation_ = { 0.0f,0.0f,0.0f };
	transforms_.clear();
	models_.clear();
}

void IObject::Update()
{
	UpdateMatrixs();
}

void IObject::Draw()
{
	DrawAllModel();
}

void IObject::DrawAllModel()
{
	for (size_t i = 0; i < transforms_.size(); i++)
	{
		models_[i]->Draw(transforms_[i], *viewProjection_);
	}
}

void IObject::UpdateMatrixs()
{
	// 行列の更新
	transformBase_.UpdateMatrix();

	for (WorldTransform& wt : transforms_) {
		wt.UpdateMatrix();
	}
}
