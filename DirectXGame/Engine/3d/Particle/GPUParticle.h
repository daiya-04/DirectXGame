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
#include <memory>


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
		kMaxNum,

		kParamNum,
	};

	enum class EmitterRootParam {
		kParticles,
		kEmitter,
		kPerFrame,
		kFreeListIndex,
		kFreeList,
		kMaxNum,
		kOverLifeTime,

		kParamNum,
	};

	enum class UpdateRootParam {
		kParticles,
		kPerFrame,
		kFreeListIndex,
		kFreeList,
		kMaxNum,
		kOverLifeTime,

		kParamNum,
	};

	

public:

	enum EmitShape : uint32_t {
		Sphere,
		Hemisphere,
		Box,
		Squere,
		Circle,

	};

	struct ParticleCS {
		Vector3 translation;
		Vector3 scale;
		Vector3 rotate;
		Vector3 velocity;
		float lifeTime;
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

	struct Emitter {
		Vector3 translate;
		float padding1;
		Vector3 size;

		float radius;
		float scale;
		float rotate;
		uint32_t count; //射出数
		float frequency; //射出間隔
		float frequencyTime; //射出間隔調整用時間
		uint32_t emit; //射出許可
		float padding2[2];
		Vector4 color;
		float lifeTime;
		float speed;
		uint32_t emitterType;
		float padding3;
	};

	struct OverLifeTime {
		uint32_t isConstantVelocity;
		Vector3 velocity;

		uint32_t isTransVelocity;		
		Vector3 startVelocity;
		Vector3 endVelocity;

		uint32_t isScale;
		float startScale;
		float endScale;

		uint32_t isColor;
		float padding1;
		Vector3 startColor;
		float padding2;
		Vector3 endColor;

		uint32_t isAlpha;
		float startAlpha;
		float midAlpha;
		float endAlpha;
		
		uint32_t isTransSpeed;
		float startSpeed;
		float endSpeed;
	};

	struct PerFrame {
		float time;
		float deltaTime;
	};

	struct MaxParticleNum {
		int32_t maxNum;
	};

private:

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;

	static ComPtr<ID3DBlob> signatureBlob_;
	static ComPtr<ID3DBlob> errorBlob_;

	static IDxcUtils* dxcUtils_;
	static IDxcCompiler3* dxcCompiler_;
	static IDxcIncludeHandler* includeHandler_;

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

	static void CreateInitCSPipeline();
	static void CreateEmitCSPipeline();
	static void CreateUpdateCSPipeline();


private:

	
	ComPtr<ID3D12Resource> particleBuff_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> uavHandle_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;

	ComPtr<ID3D12Resource> emitterBuff_;
	Emitter* emitterData_ = nullptr;

	ComPtr<ID3D12Resource> perFrameBuff_;
	PerFrame* perFrameData = nullptr;

	ComPtr<ID3D12Resource> maxParticleNumBuff_;
	MaxParticleNum* maxParticleNumData_ = nullptr;

	ComPtr<ID3D12Resource> freeListIndexBuff_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListIndexUavHandle_;

	ComPtr<ID3D12Resource> freeListBuff_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListUavHandle_;

	ComPtr<ID3D12Resource> OverLifeTimeBuff_;
	OverLifeTime* overLifeTimeData_ = nullptr;

	ComPtr<ID3D12Resource> vertexBuff_;
	D3D12_VERTEX_BUFFER_VIEW vbv_{};
	ComPtr<ID3D12Resource> indexBuff_;
	D3D12_INDEX_BUFFER_VIEW ibv_{};
	ComPtr<ID3D12Resource> perViewBuff_;
	PerView* perViewData_;

	int32_t uvHandle_ = 0;

	int32_t maxParticleNum_ = 0;

public:

	Emitter emitter_;

	OverLifeTime overLifeTime_;

	bool isLoop_ = true;

public:

	void Init(uint32_t textureHandle, int32_t particleNum);

	void Update();

	void Draw(const Camera& camera);

private:

	void CreateBuffer();

	void CreateUav();

	void ExecuteInitCS();

	void ExecuteEmitCS();

	void ExecuteUpdateCS();

};

