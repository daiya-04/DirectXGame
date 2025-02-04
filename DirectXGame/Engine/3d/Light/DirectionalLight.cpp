#include "DirectionalLight.h"

#include "DirectXCommon.h"
#include <cassert>
#include "ImGuiManager.h"

namespace DaiEngine {
	DirectionalLight* DirectionalLight::GetInstance() {
		static DirectionalLight instance;

		return &instance;
	}

	void DirectionalLight::Init() {

		CreateCBuffer();
		Map();
	}

	void DirectionalLight::Update() {
		DebugGUI();

		Map();
	}

	void DirectionalLight::CreateCBuffer() {

		//リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapproperties{};
		uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
		//リソースの設定
		D3D12_RESOURCE_DESC ResourceDesc{};
		//バッファリソース。テクスチャの場合はまた別の設定をする
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(CBufferDataDirectionalLight); //リソースのサイズ。
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

	void DirectionalLight::Map() {

		cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cMap_));
		cMap_->color = color_;
		cMap_->direction = direction_.Normalize();
		cMap_->intensity = intensity_;

	}

	void DirectionalLight::DebugGUI() {
#ifdef _DEBUG

		ImGui::Begin("Light");

		if (ImGui::TreeNode("DirectionLight")) {

			ImGui::ColorEdit4("DL color", &color_.x);
			ImGui::SliderFloat3("DL direction", &direction_.x, -1.0f, 1.0f);
			ImGui::SliderFloat("DL intensity", &intensity_, 0.0f, 1.0f);

			ImGui::TreePop();
		}

		ImGui::End();

#endif // _DEBUG
	}
}
