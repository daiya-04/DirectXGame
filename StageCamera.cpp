#include "StageCamera.h"
#include <cmath>
#include <numbers>
#include "Easing.h"
#include "ImGuiManager.h"

void StageCamera::Initialize()
{
	viewProjection_.rotation_ = { 0.0f,0.0f,0.0f };
	start_ = 0.0f;
	end_ = 0.0f;
	rotateParam_ = 1.0f;
	lengthParam_ = 0.0f;
}

void StageCamera::Update(float stageCenter)
{

	stageCenter_ = { stageCenter,-stageCenter * 2.0f,stageCenter };

	if (rotateParam_ >= 1.0f) {
		if (Input::GetInstance()->TriggerKey(DIK_LEFT)) {
			start_ = viewProjection_.rotation_.y;
			end_ = viewProjection_.rotation_.y + 90.0f * (std::numbers::pi_v<float> / 180.0f);
			rotateParam_ = 0.0f;
			lengthParam_ = 0.0f;
		}
		if (Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
			start_ = viewProjection_.rotation_.y;
			end_ = viewProjection_.rotation_.y + (-90.0f * (std::numbers::pi_v<float> / 180.0f));
			rotateParam_ = 0.0f;
			lengthParam_ = 0.0f;
		}
	}

	viewProjection_.rotation_.x = 0.5f;

	rotateParam_ += 1.0f / 10.0f;

	if (rotateParam_ >= 1.0f) {
		rotateParam_ = 1.0f;
	}

	float T = Easing::easeInSine(rotateParam_);

	//viewProjection_.rotation_.y = Lerp(T,viewProjection_.rotation_.y,end_);
	viewProjection_.rotation_.y = Lerp(T, start_, end_);

	viewProjection_.rotation_.y = std::fmod(viewProjection_.rotation_.y, 2.0f * std::numbers::pi_v<float>);

	Vector3 offset = { 0.0f,2.0f,-30.0f };
	Vector3 interPos = offset.Normalize() * (offset.Length() * 0.8f);

	Matrix4x4 rotateMat = MakeRotateXMatrix(viewProjection_.rotation_.x)
		                * MakeRotateYMatrix(viewProjection_.rotation_.y)
		                * MakeRotateZMatrix(viewProjection_.rotation_.z);

	
	if (rotateParam_ < 0.5f) {
		lengthParam_ += (1.0f / 10.0f) / 2.0f;
		T = Easing::easeInSine(lengthParam_);
		offset = Lerp(T, offset, interPos);
		if (T <= 1.0f) { lengthParam_ = 0.0f; }
	}
	if (rotateParam_ >= 0.5f) {
		lengthParam_ += (1.0f / 10.0f) / 2.0f;
		T = Easing::easeInSine(lengthParam_);
		offset = Lerp(T, interPos, offset);
	}

	if (lengthParam_ >= 1.0f) {
		lengthParam_ = 1.0f;
	}
	

	offset= TransformNormal(offset, rotateMat);

	viewProjection_.translation_ = stageCenter_ + offset;

	viewProjection_.UpdateMatrix();

#ifdef _DEBUG

	ImGui::Begin("stageCamera");

	ImGui::DragFloat3("Rotate", &viewProjection_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Translate", &viewProjection_.translation_.x, 0.1f);

	ImGui::End();

#endif // _DEBUG


}
