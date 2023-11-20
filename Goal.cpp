#include "Goal.h"
#include <cassert>
#include "GlobalVariables.h"

void Goal::Initialize(Object3d* model) {
	assert(model);
	model_ = model;

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Goal";
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	
	worldTransform_.translation_.y += worldTransform_.scale_.y;

	globalVariables->AddItem(groupName, "Goal Translation", worldTransform_.translation_);

	worldTransform_.UpdateMatrix();
}

void Goal::Update() {
	ApplyGlobalVariables();

	worldTransform_.UpdateMatrix();
}

void Goal::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection);

}

void Goal::ApplyGlobalVariables(){

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Goal";

	worldTransform_.translation_ = globalVariables->GetVec3Value(groupName, "Goal Translation");
}

Vector3 Goal::GetWorldPos() const {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
