#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "Material.h"

class Mesh {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	struct VertexData {
		Vector4 pos_;
		Vector2 uv_;
		Vector3 normal;
	};

public:

	void Init();

	void SetMaterial(const Material& material) { material_ = material; }

	const D3D12_VERTEX_BUFFER_VIEW* GetVBV() const { return &vertexBufferView_; }
	const D3D12_INDEX_BUFFER_VIEW* GetIVB() const { return &indexBufferView_; }

	Material GetMaterial() const { return material_; }


private:
	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

private:

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexBuff_;
	VertexData* vertexData_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	ComPtr<ID3D12Resource> indexBuff_;
	uint32_t* indexData_ = nullptr;

	Material material_;

public:

	std::vector<VertexData> vertices_;
	std::vector<uint32_t> indices_;
	//SkinCluster skinCluster_;

};

