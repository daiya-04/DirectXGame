#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include "Quaternion.h"
#include "ModelManager.h"
#include <vector>
#include <map>
#include <string>


namespace DaiEngine {

	class Animation {
	private:
		friend class AnimationManager;
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

		//再生
		void Play(const Model::Node& rootNode);
		void Play(Skeleton& skeleton);
		//再生開始
		void Start(bool isLoop = true) {
			if (isPlaying_) { return; }
			isPlaying_ = true;
			isLoop_ = isLoop;
			TimeReset();
		}
		//再生終了
		void End() { isPlaying_ = false; }

		void SetAnimationSpeed(float speed) { animationSpeed_ = speed; }
		void TimeReset() { animationTime_ = 0.0f; }

		Matrix4x4 GetLocalMatrix() const { return localMatrix_; }

		float GetDuration() const { return duration_; }
		float GetAnimationTime() const { return animationTime_; }
		//再生中か
		bool IsPlaying() const { return isPlaying_; }

	private:

		Vector3 CalcValue(const std::vector<KeyframeVector3>& keyframe, float time);
		Quaternion CalcValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

		void CountingAnimationTime();

	private:

		float duration_ = 0.0f;
		float animationTime_ = 0.0f;
		float animationSpeed_ = 1.0f / 60.0f;

		bool isLoop_ = false;
		bool isPlaying_ = false;

		std::map<std::string, NodeAnimation> nodeAnimations_;

		Matrix4x4 localMatrix_;

		std::string filename_;

	};
}

