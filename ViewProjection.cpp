#include "ViewProjection.h"

void ViewProjection::Initialize() {

	UpdateMatrix();

}

void ViewProjection::UpdateMatrix() {

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void ViewProjection::UpdateViewMatrix() {
	matView_ = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_).Inverse();
}

void ViewProjection::UpdateProjectionMatrix() {
	matProjection_ = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}
