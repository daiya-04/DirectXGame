#include "SpotLight.h"

#include "DirectXCommon.h"
#include <cassert>
#include "ImGuiManager.h"

namespace DaiEngine {
	void SpotLight::Init() {

		CreateCBuffer();
		Map();
		Update();
	}

	void SpotLight::Update() {

		cMap_->color = color_;
		cMap_->position = position_;
		cMap_->intensity = intensity_;
		cMap_->direction = direction_;
		cMap_->distance = distance_;
		cMap_->decay = decay_;
		cMap_->cosAngle = cosAngle_;
		cMap_->cosFalloffStart = cosFalloffStart_;

	}

	void SpotLight::CreateCBuffer() {

		//リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapproperties{};
		uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
		//リソースの設定
		D3D12_RESOURCE_DESC ResourceDesc{};
		//バッファリソース。テクスチャの場合はまた別の設定をする
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(CBufferDataSpotLight); //リソースのサイズ。
		//バッファの場合はこれにする決まり
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc.Count = 1;
		//バッファの場合はこれらにする決まり
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		//実際に頂点リソースを作る
		ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
		HRESULT hr = device->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cBuffer_));
		assert(SUCCEEDED(hr));

	}

	void SpotLight::Map() {
		cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cMap_));
	}
}
