#include "WorldTransform.h"

void WorldTransform::UpdateMatrix() {

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