#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include "Quaternion.h"
#include <string>
#include <vector>
#include <map>
#include "Animation.h"

class AnimationManager {


public:

	static AnimationManager* GetInstance();

	static Animation Load(const std::string& filename);


private:

	Animation LoadInterval(const std::string& filename);
	
private:

	std::vector<Animation> animations_;

private:

	AnimationManager() = default;
	~AnimationManager() = default;
	AnimationManager(const AnimationManager&) = default;
	AnimationManager& operator=(const AnimationManager&) = default;

};

