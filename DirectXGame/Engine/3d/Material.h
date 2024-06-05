#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec4.h"
#include "Matrix44.h"

class Material {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
private:
	struct Data {
		Vector4 color_;
		int32_t enableLightnig_;
		float padding_[3];
		Matrix4x4 uvTransform_;
		float shininess_;
	};

public:

	void Init();

	void SetUVHandle(uint32_t uvHandle) { uvHandle_ = uvHandle; }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return cBuff_->GetGPUVirtualAddress(); }
	uint32_t GetUVHandle() const { return uvHandle_; }

private:

	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

private:

	ComPtr<ID3D12Resource> cBuff_;
	Data* data_ = nullptr;

	uint32_t uvHandle_ = 0;	

};

