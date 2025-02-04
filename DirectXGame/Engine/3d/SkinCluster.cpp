#include "SkinCluster.h"

using namespace Microsoft::WRL;

namespace DaiEngine {
	void SkinCluster::Create(const Skeleton& skeleton, const std::shared_ptr<Model>& model) {


		DirectXCommon* dxCommon = DirectXCommon::GetInstance();
		ID3D12Device* device = dxCommon->GetDevice();
		UINT srvDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//palette用のResource確保
		paletteBuff_ = CreateBufferResource(device, sizeof(WellForGPU) * skeleton.joints_.size());
		WellForGPU* mappedPalette = nullptr;
		paletteBuff_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
		mappedPalette_ = { mappedPalette,skeleton.joints_.size() }; //spanを使ってアクセスするようにする
		paletteSrvHandle_.first = dxCommon->GetCPUDescriptorHandle(dxCommon->GetSrvHeap(), srvDescSize, dxCommon->GetSrvHeapCount());
		paletteSrvHandle_.second = dxCommon->GetGPUDescriptorHandle(dxCommon->GetSrvHeap(), srvDescSize, dxCommon->GetSrvHeapCount());
		dxCommon->IncrementSrvHeapCount();

		//palette用のsrvを作成。StructuredBufferでアクセスできるようにする。
		D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
		paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		paletteSrvDesc.Buffer.FirstElement = 0;
		paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints_.size());
		paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
		device->CreateShaderResourceView(paletteBuff_.Get(), &paletteSrvDesc, paletteSrvHandle_.first);

		//influence用のResourceを確保。頂点ごとにinfluence情報を追加できるようにする
		influenceBuff_ = CreateBufferResource(device, sizeof(VertexInfluence) * model->meshes_[0].vertices_.size());
		VertexInfluence* mappedInfluence = nullptr;
		influenceBuff_->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * model->meshes_[0].vertices_.size()); //0埋め。weightを0にしておく
		mappedInfluence_ = { mappedInfluence,model->meshes_[0].vertices_.size() };

		//influence用のVBVを作成
		influenceBufferView_.BufferLocation = influenceBuff_->GetGPUVirtualAddress();
		influenceBufferView_.SizeInBytes = UINT(sizeof(VertexInfluence) * model->meshes_[0].vertices_.size());
		influenceBufferView_.StrideInBytes = sizeof(VertexInfluence);

		influenceSrvHandle_.first = dxCommon->GetCPUDescriptorHandle(dxCommon->GetSrvHeap(), srvDescSize, dxCommon->GetSrvHeapCount());
		influenceSrvHandle_.second = dxCommon->GetGPUDescriptorHandle(dxCommon->GetSrvHeap(), srvDescSize, dxCommon->GetSrvHeapCount());
		dxCommon->IncrementSrvHeapCount();

		D3D12_SHADER_RESOURCE_VIEW_DESC influenceSrvDesc{};
		influenceSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		influenceSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		influenceSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		influenceSrvDesc.Buffer.FirstElement = 0;
		influenceSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		influenceSrvDesc.Buffer.NumElements = UINT(model->meshes_[0].vertices_.size());
		influenceSrvDesc.Buffer.StructureByteStride = sizeof(VertexInfluence);
		device->CreateShaderResourceView(influenceBuff_.Get(), &influenceSrvDesc, influenceSrvHandle_.first);

		//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
		inverseBindPoseMatrices_.resize(skeleton.joints_.size());
		std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), MakeIdentity44);

		//ModelのSkinCluster情報を解析
		for (const auto& jointWeight : model->skinClusterData_) {
			//JointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
			auto it = skeleton.jointMap_.find(jointWeight.first);
			if (it == skeleton.jointMap_.end()) { continue; } //存在しないので、次

			//(*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
			inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix_;
			for (const auto& vertexWeight : jointWeight.second.vertexWeights_) {
				//該当のvertexIndexのinfluence情報を参照しておく
				auto& currentInfluence = mappedInfluence_[vertexWeight.vertexIndex_];
				for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
					//空いているところに入れる
					if (currentInfluence.weights_[index] == 0.0f) {
						currentInfluence.weights_[index] = vertexWeight.weight_;
						currentInfluence.jointIndices_[index] = (*it).second;
						break;
					}
				}
			}
		}
	}

	void SkinCluster::Update(const Skeleton& skeleton) {
		for (size_t jointIndex = 0; jointIndex < skeleton.joints_.size(); ++jointIndex) {
			assert(jointIndex < inverseBindPoseMatrices_.size());
			mappedPalette_[jointIndex].skeletonSpaceMat_ = inverseBindPoseMatrices_[jointIndex] * skeleton.joints_[jointIndex].skeletonSpaceMat_;
			mappedPalette_[jointIndex].skeletonSpaceInverseTransposeMat_ = (mappedPalette_[jointIndex].skeletonSpaceMat_.Inverse()).Transpose();
		}
	}


	ComPtr<ID3D12Resource> SkinCluster::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
		//リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapproperties{};
		uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
		//リソースの設定
		D3D12_RESOURCE_DESC ResourceDesc{};
		//バッファリソース。テクスチャの場合はまた別の設定をする
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeInBytes; //リソースのサイズ。
		//バッファの場合はこれにする決まり
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc.Count = 1;
		//バッファの場合はこれらにする決まり
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		//実際に頂点リソースを作る
		ComPtr<ID3D12Resource> Resource = nullptr;
		HRESULT hr = device->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
		assert(SUCCEEDED(hr));

		return Resource;
	}
}
