#pragma once
#include <wrl.h>
#include <vector>
#include <array>
#include <span>
#include <utility>
#include "Matrix44.h"
#include "DirectXCommon.h"
#include "ModelManager.h"

namespace DaiEngine {
	class SkinCluster {
	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	private:

		static const uint32_t kNumMaxInfluence = 4;
		struct VertexInfluence {
			std::array<float, kNumMaxInfluence> weights_;
			std::array<uint32_t, kNumMaxInfluence> jointIndices_;
		};

		struct WellForGPU {
			Matrix4x4 skeletonSpaceMat_; //位置
			Matrix4x4 skeletonSpaceInverseTransposeMat_; //法線用
		};

	public:

		void Create(const Skeleton& skeleton, const std::shared_ptr<Model>& model);

		void Update(const Skeleton& skeleton);

	private:

		static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	public:

		std::vector<Matrix4x4> inverseBindPoseMatrices_;
		ComPtr<ID3D12Resource> influenceBuff_;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView_{};
		std::span<VertexInfluence> mappedInfluence_;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> influenceSrvHandle_;

		ComPtr<ID3D12Resource> paletteBuff_;
		std::span<WellForGPU> mappedPalette_;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle_;

	};


}
