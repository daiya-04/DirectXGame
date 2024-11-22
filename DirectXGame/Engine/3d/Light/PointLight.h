#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec3.h"
#include "Vec4.h"

struct CBufferDataPointLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
	float padding[2];
};

class PointLight{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	void Init();
	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return cBuffer_->GetGPUVirtualAddress();
	}

private:

	void CreateCBuffer();
	void Map();

public:

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_ = {};
	float intensity_ = 0.0f;
	float radius_ = 5.0f;
	float decay_ = 1.0f;

private:

	ComPtr<ID3D12Resource> cBuffer_;
	CBufferDataPointLight* cMap_ = nullptr;

};

