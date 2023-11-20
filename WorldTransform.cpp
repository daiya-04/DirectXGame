#include "WorldTransform.h"

void WorldTransform::UpdateMatrix() {

	matWorld_ = MakeIdentity44();

	/*Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);
	Matrix4x4 rotateMatrix = DirectionToDirection({ 1.0f,0.0f,0.0f }, rotation_);
	Matrix4x4 transformMatrix = MakeTranslateMatrix(translation_);

	matWorld_ = scaleMatrix * rotateMatrix * transformMatrix;*/

	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_) {
		matWorld_ = matWorld_ * parent_->matWorld_;
	}
	if (translationParent_) {

		Matrix4x4 inverseMat = MakeScaleMatrix(MakeScale(translationParent_->matWorld_)).Inverse();
		matWorld_ = matWorld_ * inverseMat;

		
		Matrix4x4 parentMat = translationParent_->matWorld_;

		matWorld_ = parentMat.Inverse() * matWorld_;
		
		//Vector3 pos = MakeTranslation(matWorld_) - MakeTranslation(translationParent_->matWorld_);
		
		//matWorld_ = matWorld_ * translationParent_->matWorld_;
	}

}