#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "Camera.h"
#include "WorldTransform.h"

class SkyBox {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //静的メンバ関数

	//モデルの生成
	static SkyBox* Create(uint32_t textureHandle);

private:
	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

private:

	uint32_t textureHandle_ = 0;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexResource_;
	
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	WorldTransform worldTransform_;

	float scale_ = 1000.0f;

public:

	void Init(uint32_t textureHandle);

	void Draw(const Camera& camera);

private:

	void CreateMesh();

};

