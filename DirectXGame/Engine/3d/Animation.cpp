#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>

Animation Animation::LoadAnimationFile(const std::string& filename) {

	Animation animation;
	Assimp::Importer importer;
	std::string directoryPath = "Resources/model/" + filename + "/";
	std::string filePath = directoryPath + filename + ".gltf";
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);
	aiAnimation* animationAssimp = scene->mAnimations[0];
	//時間の単位を秒に変換
	//animationAssimp->mTicksPerSecond : 周波数
	//animationAssimp->mDuration : mTicksPerSecondで指定された周波数にをける長さ
	animation.duration_ = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	//assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報をとってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; channelIndex++) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations_[nodeAnimationAssimp->mNodeName.C_Str()];
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
			aiVectorKey& position = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time_ = float(position.mTime / animationAssimp->mTicksPerSecond); //秒に変換
			keyframe.value_ = { -position.mValue.x,position.mValue.y,position.mValue.z }; //右手->左手
			nodeAnimation.translate_.Keyframes_.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
			aiQuatKey& rotate = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time_ = float(rotate.mTime / animationAssimp->mTicksPerSecond); //秒に変換
			keyframe.value_ = { rotate.mValue.x, -rotate.mValue.y, -rotate.mValue.z, rotate.mValue.w };
			nodeAnimation.rotate_.Keyframes_.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
			aiVectorKey& scale = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time_ = float(scale.mTime / animationAssimp->mTicksPerSecond); //秒に変換
			keyframe.value_ = { scale.mValue.x, scale.mValue.y, scale.mValue.z };
			nodeAnimation.scale_.Keyframes_.push_back(keyframe);
		}

	}

	return animation;
}

void Animation::Play(const Model::Node& rootNode, bool isLoop) {

	isLoop_ = isLoop;
	isPlaying_ = true;
	CountingAnimationTime();
	
	NodeAnimation& rootNodeAnimation = nodeAnimations_[rootNode.name_];
	Vector3 translate = CalcValue(rootNodeAnimation.translate_.Keyframes_, animationTime_);
	Quaternion rotate = CalcValue(rootNodeAnimation.rotate_.Keyframes_, animationTime_);
	Vector3 scale = CalcValue(rootNodeAnimation.scale_.Keyframes_, animationTime_);

	localMatrix_ = MakeTranslateMatrix(translate) * rotate.MakeRotateMatrix() * MakeScaleMatrix(scale);
}

void Animation::Play(Skeleton& skeleton,bool isLoop) {

	isLoop_ = isLoop;
	isPlaying_ = true;
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
		animationTime_ += 1.0f / 60.0f;
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
