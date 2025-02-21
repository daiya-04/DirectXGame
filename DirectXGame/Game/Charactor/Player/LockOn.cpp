#include "LockOn.h"

#include "Boss.h"

void LockOn::Init() {

}

void LockOn::Update() {

}

Vector3 LockOn::GetTargetPos() const {

	if (target_) {
		return target_->GetCenterPos();
	}

	return Vector3();
}
