#include "AnimationManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


AnimationManager* AnimationManager::GetInstance() {
	static AnimationManager instance;

	return &instance;
}

Animation AnimationManager::Load(const std::string& filename) {
	return AnimationManager::GetInstance()->LoadInterval(filename);
}

Animation AnimationManager::LoadInterval(const std::string& filename) {

	for (const auto& animation : animations_) {
		if (animation.filename_ == filename) {
			return animation;
		}
	}

	Animation animation;
	animation.filename_ = filename;
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
		Animation::NodeAnimation& nodeAnimation = animation.nodeAnimations_[nodeAnimationAssimp->mNodeName.C_Str()];
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
			aiVectorKey& position = nodeAnimationAssimp->mPositionKeys[keyIndex];
			Animation::KeyframeVector3 keyframe;
			keyframe.time_ = float(position.mTime / animationAssimp->mTicksPerSecond); //秒に変換
			keyframe.value_ = { -position.mValue.x,position.mValue.y,position.mValue.z }; //右手->左手
			nodeAnimation.translate_.Keyframes_.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
			aiQuatKey& rotate = nodeAnimationAssimp->mRotationKeys[keyIndex];
			Animation::KeyframeQuaternion keyframe;
			keyframe.time_ = float(rotate.mTime / animationAssimp->mTicksPerSecond); //秒に変換
			keyframe.value_ = { rotate.mValue.x, -rotate.mValue.y, -rotate.mValue.z, rotate.mValue.w };
			nodeAnimation.rotate_.Keyframes_.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
			aiVectorKey& scale = nodeAnimationAssimp->mScalingKeys[keyIndex];
			Animation::KeyframeVector3 keyframe;
			keyframe.time_ = float(scale.mTime / animationAssimp->mTicksPerSecond); //秒に変換
			keyframe.value_ = { scale.mValue.x, scale.mValue.y, scale.mValue.z };
			nodeAnimation.scale_.Keyframes_.push_back(keyframe);
		}
	}

	animations_.emplace_back(animation);

	return animations_.back();

}
