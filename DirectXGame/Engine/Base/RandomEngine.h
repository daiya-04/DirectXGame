#pragma once
#include <random>
class RandomEngine
{
public:
	static RandomEngine* GetInstance() {
		static RandomEngine instance;
		return &instance;
	};
	//void Initialize();
	std::mt19937& GetRandomEngine() { return randomEngine_; };
	static float GetFloatRandom(float min, float max) {
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(GetInstance()->GetRandomEngine());
	};
	static int GetIntRandom(int min, int max) {
		std::uniform_int_distribution<int> distributinon(min, max);
		return distributinon(GetInstance()->GetRandomEngine());
	}
private:
	RandomEngine() {
		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());
		randomEngine_ = randomEngine;
	};
	~RandomEngine() = default;
	RandomEngine(const RandomEngine&) = delete;
	RandomEngine operator=(const RandomEngine&) = delete;

	std::mt19937 randomEngine_;
};