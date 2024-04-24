#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include "Quaternion.h"
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

private:

	float duration_;

	std::map<std::string, NodeAnimation> nodeAnimations_;

};

