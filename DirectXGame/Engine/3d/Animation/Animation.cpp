#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>

void Animation::Play(const Model::Node& rootNode) {

	//isLoop_ = isLoop;
	//isPlaying_ = true;
	CountingAnimationTime();
	
	NodeAnimation& rootNodeAnimation = nodeAnimations_[rootNode.name_];
	Vector3 translate = CalcValue(rootNodeAnimation.translate_.Keyframes_, animationTime_);
	Quaternion rotate = CalcValue(rootNodeAnimation.rotate_.Keyframes_, animationTime_);
	Vector3 scale = CalcValue(rootNodeAnimation.scale_.Keyframes_, animationTime_);

	localMatrix_ = MakeTranslateMatrix(translate) * rotate.MakeRotateMatrix() * MakeScaleMatrix(scale);
}

void Animation::Play(Skeleton& skeleton) {

	/*isLoop_ = isLoop;
	isPlaying_ = true;*/
	CountingAnimationTime();

	for (Skeleton::Joint& joint : skeleton.joints_) {
		if (auto it = nodeAnimations_.find(joint.name_); it != nodeAnimations_.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform_.translate_ = CalcValue(rootNodeAnimation.translate_.Keyframes_, animationTime_);
			joint.transform_.rotate_ = CalcValue(rootNodeAnimation.rotate_.Keyframes_, animationTime_);
			joint.transform_.scale_ = CalcValue(rootNodeAnimation.scale_.Keyframes_, animationTime_);
		}
	}
}

Vector3 Animation::CalcValue(const std::vector<KeyframeVector3>& keyframe, float time) {
	assert(!keyframe.empty()); //キーが入っているか
	if (keyframe.size() == 1 || time <= keyframe[0].time_) {
		return keyframe[0].value_;
	}

	for (size_t index = 0; index < keyframe.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframe[index].time_ <= time && time <= keyframe[nextIndex].time_) {
			float t = (time - keyframe[index].time_) / (keyframe[nextIndex].time_ - keyframe[index].time_);
			return Lerp(t, keyframe[index].value_, keyframe[nextIndex].value_);
		}
	}

	return (*keyframe.begin()).value_;
}

Quaternion Animation::CalcValue(const std::vector<KeyframeQuaternion>& keyframe, float time) {
	assert(!keyframe.empty()); //キーが入っているか
	if (keyframe.size() == 1 || time <= keyframe[0].time_) {
		return keyframe[0].value_;
	}
	for (size_t index = 0; index < keyframe.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframe[index].time_ <= time && time <= keyframe[nextIndex].time_) {
			float t = (time - keyframe[index].time_) / (keyframe[nextIndex].time_ - keyframe[index].time_);
			return Slerp(keyframe[index].value_, keyframe[nextIndex].value_, t);
		}
	}

	return (*keyframe.begin()).value_;
}

void Animation::CountingAnimationTime() {
	if (isPlaying_) {
		animationTime_ += animationSpeed_;
	}
	if (animationTime_ >= duration_) {
		if (isLoop_) {
			animationTime_ = std::fmod(animationTime_, duration_);
		}else {
			animationTime_ = duration_;
			isPlaying_ = false;
		}
	}

}
