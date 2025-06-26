#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Log.h"
#include "PipelineManager.h"
#include <string>
#include <memory>

namespace DaiEngine {
	class GPUParticle {
	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:

		enum EmitShape : uint32_t {
			NonShape,
			Sphere,
			Hemisphere,
			Box,
			Square,
			Circle,

		};

		enum BillboardType : uint32_t {
			Billboard,
			Horizontalillboard,
			None,
		};

		struct ParticleCS {
			Vector3 translation;
			Vector3 scale;
			Vector3 rotate;
			Vector3 velocity;
			float lifeTime;
			float currentTime;
			Vector4 color;
			uint32_t billboardType;
			Vector3 roringSpeed;
			Vector3 noiseOffset;
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
			Vector3 translate; //位置座標
			float radius; //半径(球体、半球、円)

			Vector3 size; //サイズ（ボックス、四角）
			float scale; //スケール

			float rotate; //回転
			uint32_t count; //射出数
			float frequency; //射出間隔
			float frequencyTime; //射出間隔調整用時間

			Vector4 color; //色

			uint32_t emit; //射出許可
			float lifeTime; //寿命
			float speed; //速さ
			uint32_t emitterType; //エミッターの種類

			uint32_t billboardType; //ビルボードタイプ
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

			float gravity;

			uint32_t isRoring;
			Vector3 minRoringSpeed;
			float padding3;
			Vector3 maxRoringSpeed;

			uint32_t isNoise;
			Vector3 density;
			float strength;
			uint32_t isRandom;

		};

		struct ParticleData {
			Emitter emitter_;
			OverLifeTime overLifeTime_;
			bool isLoop_ = true;
			std::string textureName_;
			bool isModel_ = false;
			std::string modelName_;
		};

		struct PerFrame {
			float time;
			float deltaTime;
		};

		struct MaxParticleNum {
			int32_t maxNum;
		};

	public:

		static GPUParticle* Create(uint32_t textureHandle, int32_t particleNum);
		//描画前処理
		static void preDraw();
		//描画後処理
		static void postDraw();

	private:

		//リソースの生成
		static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	private:

		ID3D12GraphicsCommandList* commandList_ = nullptr;
		ID3D12Device* device_ = nullptr;

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

		std::shared_ptr<Model> model_;

	public:

		ParticleData particleData_;

	public:

		void Init(uint32_t textureHandle, int32_t particleNum);

		void Update();

		void Draw(const Camera& camera, BlendMode blendMode = BlendMode::kAdd);

		void SetParticleData(const ParticleData& particleData);

		void SetTextureHandle();

		void SetModel();
		void ModelReset() { model_ = nullptr; }

		void Emit() { particleData_.emitter_.emit = 1; }

	private:

		void CreateBuffer();

		void CreateUav();

		void ExecuteInitCS();

		void ExecuteEmitCS();

		void ExecuteUpdateCS();

		void DataInit();

	};

}
