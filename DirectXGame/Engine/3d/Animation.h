#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include "Quaternion.h"
#include "ModelManager.h"
#include <vector>
#include <map>
#include <string>


class Animation {
private:

	template <typename Value>
	struct Keyframe {
		float time_;
		Value value_;
	};

	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	template <typename Value>
	struct AnimationCurve {
		std::vector<Keyframe<Value>> Keyframes_;
	};

	struct NodeAnimation {
		AnimationCurve<Vector3> translate_;
		AnimationCurve<Quaternion> rotate_;
		AnimationCurve<Vector3> scale_;
	};

public:

	static Animation LoadAnimationFile(const std::string& filename);

	//再生
	void Play(const Model::Node& rootNode);

	Matrix4x4 GetLocalMatrix() const { return localMatrix_; }

private:

	Vector3 CalcValue(const std::vector<KeyframeVector3>& keyframe, float time);
	Quaternion CalcValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

private:

	float duration_ = 0.0f;
	float animationTime_ = 0.0f;

	std::map<std::string, NodeAnimation> nodeAnimations_;

	Matrix4x4 localMatrix_;

};

