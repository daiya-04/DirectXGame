#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec3.h"
#include "Vec4.h"
#include <numbers>

struct CBufferDataSpotLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	Vector3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
	float padding[2];
};

class SpotLight{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	void Init();
	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		return cBuffer_->GetGPUVirtualAddress();
	}

private:

	void CreateCBuffer();
	void Map();

public:

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_ = {};
	float intensity_ = 1.0f;
	Vector3 direction_ = {1.0f,0.0f,0.0f};
	float distance_ = 7.0f;
	float decay_ = 1.0f;
	float cosAngle_ = std::cosf(std::numbers::pi_v<float> / 3.0f);
	float cosFalloffStart_ = 1.0f;

private:

	ComPtr<ID3D12Resource> cBuffer_;
	CBufferDataSpotLight* cMap_ = nullptr;


};

