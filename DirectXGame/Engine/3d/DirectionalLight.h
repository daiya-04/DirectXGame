#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec3.h"
#include "Vec4.h"

struct CBufferDataDirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

class DirectionalLight {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	static DirectionalLight* GetInstance();

	void Init();
	void Update();
	void DebugGUI();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return cBuffer_->GetGPUVirtualAddress();
	}

private:
	void CreateCBuffer();
	void Map();

public:
	Vector4 color_ = { 0.419f,0.411f,0.788f,1.0f };
	Vector3 direction_ = { 0.0f,-1.0f,0.0f };
	float intensity_ = 1.0f;

private:

	ComPtr<ID3D12Resource> cBuffer_;
	CBufferDataDirectionalLight* cMap_;

private:

	DirectionalLight() = default;
	~DirectionalLight() = default;
	DirectionalLight(const DirectionalLight&) = delete;
	DirectionalLight& operator=(const DirectionalLight&) = delete;

};

