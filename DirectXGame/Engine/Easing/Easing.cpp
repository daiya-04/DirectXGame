#include "Easing.h"
#include <cmath>
#include <numbers>

float Easing::easeInSine(float x) {
	return 1.0f - std::cosf((x * (float)std::numbers::pi) / 2.0f);
}

float Easing::easeOutSine(float x) {
	return std::sinf((x * (float)std::numbers::pi) / 2.0f);
}

float Easing::easeInOutSine(float x) {
	return -(cosf((float)std::numbers::pi * x) - 1.0f) / 2.0f;
}

float Easing::easeInQuad(float x) {
	return std::powf(x, 2.0f);
}

float Easing::easeOutQuad(float x) {
	return 1.0f - std::powf(1.0f - x, 2.0f);
}

float Easing::easeInOutQuad(float x) {
	return x < 0.5f ? 2.0f * std::powf(x, 2.0f) : 1 - std::powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;
}

float Easing::easeInCubic(float x) {
	return std::powf(x, 3.0f);
}

float Easing::easeOutCubic(float x) {
	return 1.0f - std::powf(1.0f - x, 3.0f);
}

float Easing::easeInOutCubic(float x) {
	return x < 0.5f ? 4.0f * std::powf(x, 3.0f) : 1.0f - std::powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

float Easing::easeInQuart(float x) {
	return std::powf(x, 4.0f);
}

float Easing::easeOutQuart(float x) {
	return 1.0f - std::pow(1.0f - x, 4.0f);
}

float Easing::easeInOutQuart(float x) {
	return x < 0.5f ? 8.0f * std::powf(x, 4.0f) : 1.0f - std::powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;
}

float Easing::easeInQuint(float x) {
	return std::powf(x, 5.0f);
}

float Easing::easeOutQuint(float x) {
	return 1.0f - std::powf(1.0f - x, 5.0f);
}

float Easing::easeInOutQuint(float x) {
	return x < 0.5f ? 16.0f * std::powf(x, 5.0f) : 1.0f - std::powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;
}

float Easing::easeInExpo(float x) {
	return x == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * x - 10.0f);
}

float Easing::easeOutExpo(float x) {
	return x == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * x);
}

float Easing::easeInOutExpo(float x) {
	return x == 0.0f 
		? 0.0f 
		: x == 1.0f 
		? 1.0f 
		: x < 0.5f 
		? std::powf(2.0f, 20.0f * x - 10.0f) / 2.0f 
		: (2.0f - std::powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;
}

float Easing::easeInCirc(float x) {
	return 1.0f - std::sqrtf(1.0f - std::pow(x, 2.0f));
}

float Easing::easeOutCirc(float x) {
	return std::sqrtf(1.0f - std::powf(x - 1.0f, 2.0f));
}

float Easing::easeInOutCirc(float x) {
	return x < 0.5f
		? (1.0f - std::sqrtf(1.0f - std::powf(2.0f * x, 2.0f))) / 2.0f
		: (std::sqrtf(1.0f - std::powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

float Easing::easeInBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return c3 * std::powf(x, 3.0f) - c1 * std::powf(x, 2.0f);
}

float Easing::easeOutBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * std::powf(x - 1.0f, 3.0f) + c1 * std::powf(x - 1.0f, 2.0f);
}

float Easing::easeInOutBack(float x) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5f
		? (std::powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
		: (std::powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) + c2) + 2.0f) / 2.0f;
}

float Easing::easeInElastic(float x) {
	const float c4 = (2.0f * (float)std::numbers::pi) / 3.0f;

	return x == 0
		? 0.0f
		: x == 1.0f
		? 1.0f
		: -std::powf(2.0f, 10.0f * x - 10.0f) * std::sinf((x * 10.0f - 10.75f) * c4);
}

float Easing::easeOutElastic(float x) {
	const float c4= (2.0f * (float)std::numbers::pi) / 3.0f;

	return x == 0.0f
		? 0.0f
		: x == 1.0f
		? 1.0f
		: std::powf(2.0f, -10.0f * x) * std::sinf((x * 10.0f - 0.75f) * c4) + 1;
}

float Easing::easeInOutElastic(float x) {
	const float c5 = (2.0f * (float)std::numbers::pi) / 4.5f;

	return x == 0
		? 0.0f
		: x == 1.0f
		? 1.0f
		: x < 0.5f
		? -(std::powf(2.0f, 20.0f * x - 10.0f) * std::sinf((20.0f * x - 11.125f) * c5)) / 2.0f
		: (std::powf(2.0f, -20.0f * x + 10.0f) * std::sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
}

float Easing::easeInBounce(float x) {
	return 1.0f - easeOutBounce(1.0f - x);
}

float Easing::easeOutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	return x < 1.0f / d1
		? n1 * std::powf(x, 2.0f)
		: x < 2.0f / d1
		? n1 * (x -= 1.5f / d1) * x + 0.75f
		: x < 2.5f / d1
		? n1 * (x -= 2.25f / d1) * x + 0.9375f
		: n1 * (x -= 2.625f / d1) * x + 0.984375f;
}

float Easing::easeInOutBounce(float x) {
	return x < 0.5f
		? (1.0f - easeOutBounce(1.0f - 2.0f * x)) / 2.0f
		: (1.0f + easeOutBounce(2.0f * x - 1.0f)) / 2.0f;
}