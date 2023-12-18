#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include <d3d12.h>
#include <wrl.h>

struct CBufferDataWorldTransform {
	Matrix4x4 matWorld;
};

class WorldTransform{
public:

	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	
	Vector3 rotation_ = {};

	Vector3 translation_ = {};

	Matrix4x4 matWorld_ = MakeIdentity44();

	const WorldTransform* parent_ = nullptr;

public:

	void Init();
	void UpdateMatrix();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return cBuffer_->GetGPUVirtualAddress();
	}

private:

	void Map();
	void CreateCBuffer();

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> cBuffer_;
	CBufferDataWorldTransform* cMap_ = nullptr;

};

