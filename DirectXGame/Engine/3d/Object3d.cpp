#include "Object3d.h"
#include <cassert>
#include <Windows.h>
#include "TextureManager.h"
#include "Log.h"
#include "DirectionalLight.h"
#include "DirectXCommon.h"
#include "PipelineManager.h"
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

	}

	void Object3d::Update() {

		//worldTransform_.UpdateMatrix();
		//worldTransform_.matWorld_ = model_->rootNode_.localMatrix_ * worldTransform_.matWorld_;

		/*if (isAnimation_) {
			animation_.Play(model_->rootNode_);
			worldTransform_.matWorld_ = animation_.GetLocalMatrix() * worldTransform_.matWorld_;
		}*/

	}

	void Object3d::Draw(const Camera& camera) {

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

}
