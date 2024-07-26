#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Log.h"
#include <string>


class GPUParticle {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	enum class RootParameter {
		kParticleGPU,
		kPerView,
		kTexture,

		kParamNum,
	};

	enum class InitParticleRootParam {
		kParticles,
		kFreeListIndex,
		kFreeList,

		kParamNum,
	};

	enum class EmitterRootParam {
		kParticles,
		kEmitter,
		kPerFrame,
		kFreeListIndex,
		kFreeList,

		kParamNum,
	};

	enum class UpdateRootParam {
		kParticles,
		kPerFrame,
		kFreeListIndex,
		kFreeList,

		kParamNum,
	};

	struct ParticleCS {
		Vector3 translation;
		Vector3 scale;
		float lifeTime;
		Vector3 velocity;
		float currentTime;
		Vector4 color;
	};

	struct VertexData {
		Vector4 pos_;
		Vector2 uv_;
	};

	struct PerView {
		Matrix4x4 viewProjectionMat_;
		Matrix4x4 billboardMat_;
	};

	struct EmitterSphere {
		Vector3 translate;
		float size;
		float scale;
		uint32_t count; //射出数
		float frequency; //射出間隔
		float frequencyTime; //射出間隔調整用時間
		uint32_t emit; //射出許可
		Vector3 direction;
		float angle;
	};

	struct PerFrame {
		float time;
		float deltaTime;
	};

private:

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	static ComPtr<ID3D12RootSignature> initRootSignature_;
	static ComPtr<ID3D12PipelineState> initComputePS_;

	static ComPtr<ID3D12RootSignature> emitRootSignature_;
	static ComPtr<ID3D12PipelineState> emitComputePS_;

	static ComPtr<ID3D12RootSignature> updateRootSignature_;
	static ComPtr<ID3D12PipelineState> updateComputePS_;

public:

	//静的初期化
	static void StaticInit();

	static GPUParticle* Create(uint32_t textureHandle, int32_t particleNum);
	//描画前処理
	static void preDraw();
	//描画後処理
	static void postDraw();

private:

	//シェーダのコンパイル
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);
	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

private:

	
	ComPtr<ID3D12Resource> particleBuff_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> uavHandle_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;

	ComPtr<ID3D12Resource> emitterBuff_;
	EmitterSphere* emitterSphereData_ = nullptr;

	ComPtr<ID3D12Resource> perFrameBuff_;
	PerFrame* perFrameData = nullptr;

	ComPtr<ID3D12Resource> freeListIndexBuff_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListIndexUavHandle_;

	ComPtr<ID3D12Resource> freeListBuff_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListUavHandle_;

	ComPtr<ID3D12Resource> vertexBuff_;
	D3D12_VERTEX_BUFFER_VIEW vbv_{};
	ComPtr<ID3D12Resource> indexBuff_;
	D3D12_INDEX_BUFFER_VIEW ibv_{};
	ComPtr<ID3D12Resource> perViewBuff_;
	PerView* perViewData_;

	int32_t uvHandle_ = 0;

	int32_t maxParticleNum_ = 0;

public:

	void Init(uint32_t textureHandle, int32_t particleNum);

	void Update();

	void Draw(const Camera& camera);

private:

	void CreateBuffer();

	void CreateUav();

};

