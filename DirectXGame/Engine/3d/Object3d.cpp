#include "Object3d.h"
#include <cassert>
#include <Windows.h>
#include "TextureManager.h"
#include "Log.h"
#include "DirectionalLight.h"
#include "DirectXCommon.h"
#include "RootParameters.h"

using namespace Microsoft::WRL;
using namespace Object3dParam;

namespace DaiEngine {
	PointLight* Object3d::pointLight_ = nullptr;
	SpotLight* Object3d::spotLight_ = nullptr;

	Object3d* Object3d::Create(std::shared_ptr<Model> model) {

		Object3d* obj = new Object3d();
		obj->Initialize(model);

		return obj;

	}

	void Object3d::preDraw() {
		PipelineManager::GetInstance()->preDraw("Object3d");
	}

	void Object3d::postDraw() {



	}

	void Object3d::Initialize(std::shared_ptr<Model> model) {
		model_ = model;
		worldTransform_.Init();
		visible_ = true;

		objectDataBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ObjectData));
		objectDataBuff_->Map(0, nullptr, reinterpret_cast<void**>(&objectData_));
		objectData_->alpha_ = 1.0f;
		objectData_->envReflectIntensity_ = 0.01f;
		objectData_->fresnelColor_ = { 0.8f, 0.9f, 1.0f };
		objectData_->subsurfaceColor_ = { 0.6f, 0.8f, 1.0f };

	}

	void Object3d::Draw(const Camera& camera, BlendMode blendMode) {

		if (!visible_) { return; }

		PipelineManager::GetInstance()->preDraw("Object3d", blendMode);

		worldTransform_.Map();

		ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

		for (auto& mesh : model_->meshes_) {

			commandList->IASetVertexBuffers(0, 1, mesh.GetVBV());
			commandList->IASetIndexBuffer(mesh.GetIVB());

			const auto& material = mesh.GetMaterial();
			commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());
			//wvp用のCBufferの場所の設定
			commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform_.GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kCamera), camera.GetGPUVirtualAddress());

			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, static_cast<UINT>(RootParameter::kTexture), material.GetUVHandle());

			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, static_cast<UINT>(RootParameter::kEnvironmentTex), TextureManager::Load("skyBox.dds"));

			commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kDirectionLight), DirectionalLight::GetInstance()->GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kPointLight), pointLight_->GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kSpotLight), spotLight_->GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kObjectData), objectDataBuff_->GetGPUVirtualAddress());

			commandList->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);

		}

	}

	Vector3 Object3d::GetWorldPos() const {
		Vector3 worldPos;

		worldPos.x = worldTransform_.matWorld_.m[3][0];
		worldPos.y = worldTransform_.matWorld_.m[3][1];
		worldPos.z = worldTransform_.matWorld_.m[3][2];

		return worldPos;
	}

	ComPtr<ID3D12Resource> Object3d::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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
