#include "Signpost.h"

void Signpost::Init(std::vector<Object3d*> models)
{
	world_.Init();
	models_ = models;
}

void Signpost::Update()
{

#ifdef _DEBUG
	ImGui::Begin("Sign");
	ImGui::InputFloat3("posStert",&posStert.x);
	ImGui::InputFloat3("posEnd",&posEnd.x);
	ImGui::End();
#endif
	posStert.y += 2.7f;

	Vector3 sub = posEnd - posStert;

	sub.z = 0;
	sub = sub.Normalize();

	Vector3 cross = Cross({1.0f,0.0f,0.0f},sub).Normalize();
	float dot = Dot({1.0f,0.0f,0.0f},sub);

	forTargetQua = MakwRotateAxisAngleQuaternion(cross,std::acos(dot));

	Vector3 offset = OffsetCalc();
	world_.translation_ = posStert + offset;

	world_.UpdateMatrixQua(forTargetQua.MakeRotateMatrix());
}

void Signpost::Draw(const Camera& camera)
{
	models_[0]->Draw(world_, camera);
}

Vector3 Signpost::OffsetCalc()
{
	Vector3 offset = {10.0,0.0f,0.0f};
	Matrix4x4 rotateMatrix = forTargetQua.MakeRotateMatrix();
	offset = TransformNormal(offset,rotateMatrix);
	return offset;
}
