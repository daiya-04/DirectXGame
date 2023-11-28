#include "LockOn.h"

#include "TextureManager.h"
#include "Input.h"
#include "Enemy.h"
#include <cmath>

void LockOn::Initialize() {

	uint32_t reticle = TextureManager::Load("Reticle.png");

	lockOnMark_.reset(new Sprite(reticle, { 640.0f,360.0f }, { 32.0f,32.0f }, 0.0f, { 0.5f,0.5f }));
	lockOnMark_->Initialize();

	

}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {

	if (target_) {
		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_LEFT_SHOULDER)) {

			target_ = nullptr;
		}else if (OutSideDecision(enemies, viewProjection)) {
			target_ = nullptr;
		}

		if (Input::GetInstance()->LeftTrigger()) {
			ChangeTarget(enemies, viewProjection);
		}

	}else {

		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_Y)) {
			if (isAuto_) {
				isAuto_ = false;
			}
			else {
				isAuto_ = true;
			}
		}

		if (!isAuto_) {
			if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				Search(enemies, viewProjection);
			}
		}
		else {
			Search(enemies, viewProjection);
		}
		

	}

	if (target_) {

		Vector3 worldPos = target_->GetWorldPos();

		Matrix4x4 viewportMat = MakeViewportMatrix(0.0f, 0.0f, (float)WinApp::kClientWidth, (float)WinApp::kClientHeight);

		Matrix4x4 vpvMat = viewProjection.matView_ * viewProjection.matProjection_ * viewportMat;

		Vector3 screenPos = Transform(worldPos, vpvMat);

		Vector2 screenPosV2(screenPos.x, screenPos.y);

		lockOnMark_->SetPosition(screenPosV2);
	}

}

void LockOn::Draw() {

	if (target_) {
		lockOnMark_->Draw();
	}
	

}

void LockOn::Search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection){
	
	std::list<std::pair<float, const Enemy*>> targets;

	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		Vector3	worldPos = enemy->GetWorldPos();

		Vector3 positionView = Transform(worldPos, viewProjection.matView_);

		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {

			//float arcTangent = std::atan2(std::sqrtf(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

			float angle = std::acos(positionView.z / std::sqrtf(positionView.x * positionView.x + positionView.z * positionView.z));

			if (std::fabs(angle) <= angleRange_) {
				targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
			}

		}

		target_ = nullptr;
		if (!targets.empty()) {
			targets.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
			target_ = targets.front().second;
		}

	}

}

void LockOn::ChangeTarget(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection){
	std::list<std::pair<float, const Enemy*>> targets;

	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		Vector3	worldPos = enemy->GetWorldPos();

		Vector3 positionView = Transform(worldPos, viewProjection.matView_);

		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {

			//float arcTangent = std::atan2(std::sqrtf(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

			float angle = std::acos(positionView.z / std::sqrtf(positionView.x * positionView.x + positionView.z * positionView.z));

			if (std::fabs(angle) <= angleRange_) {
				targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
			}
		}
	}

	if (!targets.empty()) {
		targets.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
		for (std::list<std::pair<float, const Enemy*>>::iterator itTarget = targets.begin(); itTarget != targets.end();) {
			if ((*itTarget).second == target_) {
				itTarget++;
				continue;
			}else {
				target_ = (*itTarget).second;
			}
			
		}
	}

}

bool LockOn::OutSideDecision(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection){

	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		Vector3	worldPos = enemy->GetWorldPos();

		Vector3 positionView = Transform(worldPos, viewProjection.matView_);

		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
			return false;
		}
	}

	return true;
}

Vector3 LockOn::GetTargetPos() const {
	if (target_) {
		return target_->GetWorldPos();
	}

	return Vector3();
}

