#include "SkyBox.h"
#include <cassert>

#include "TextureManager.h"
#include "DirectXCommon.h"
#include "PipelineManager.h"
#include "RootParameters.h"

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

namespace DaiEngine {
	SkyBox* SkyBox::Create(uint32_t textureHandle) {

		SkyBox* skyBox = new SkyBox();
		skyBox->Init(textureHandle);

		return skyBox;
	}

	ComPtr<ID3D12Resource> SkyBox::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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

	void SkyBox::Init(uint32_t textureHandle) {

		textureHandle_ = textureHandle;
		worldTransform_.Init();
		CreateMesh();

	}

	void SkyBox::Draw(const Camera& camera) {

		worldTransform_.matWorld_ = MakeAffineMatrix({ scale_,scale_ ,scale_ }, {}, {});
		worldTransform_.Map();

		ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

		PipelineManager::GetInstance()->preDraw("SkyBox");

		//VBVを設定
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
		commandList->IASetIndexBuffer(&indexBufferView_);

		commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(SkyBoxParam::RootParameter::kWorldTransform), worldTransform_.GetGPUVirtualAddress());

		commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(SkyBoxParam::RootParameter::kCamera), camera.GetGPUVirtualAddress());

		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, static_cast<UINT>(SkyBoxParam::RootParameter::kTexture), textureHandle_);

		commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
	}

	void SkyBox::CreateMesh() {

		ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

		vertexResource_ = CreateBufferResource(device, sizeof(Vector4) * 8);

		vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = sizeof(Vector4) * 8;
		vertexBufferView_.StrideInBytes = sizeof(Vector4);

		Vector4* vertexData = nullptr;
		vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

		vertexData[0] = { -1.0f,1.0f,1.0f,1.0f };
		vertexData[1] = { -1.0f,-1.0f,1.0f,1.0f };
		vertexData[2] = { -1.0f,-1.0f,-1.0f,1.0f };
		vertexData[3] = { -1.0f,1.0f,-1.0f,1.0f };

		vertexData[4] = { 1.0f,1.0f,1.0f,1.0f };
		vertexData[5] = { 1.0f,-1.0f,1.0f,1.0f };
		vertexData[6] = { 1.0f,-1.0f,-1.0f,1.0f };
		vertexData[7] = { 1.0f,1.0f,-1.0f,1.0f };

		indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 36);

		indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = sizeof(uint32_t) * 36;
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

		uint32_t* indexData = nullptr;
		indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

		//前面
		indexData[0] = 0; indexData[1] = 4; indexData[2] = 1;
		indexData[3] = 4; indexData[4] = 5; indexData[5] = 1;

		//左面
		indexData[6] = 3; indexData[7] = 0; indexData[8] = 2;
		indexData[9] = 0; indexData[10] = 1; indexData[11] = 2;

		//後面
		indexData[12] = 7; indexData[13] = 3; indexData[14] = 6;
		indexData[15] = 3; indexData[16] = 2; indexData[17] = 6;

		//右面
		indexData[18] = 4; indexData[19] = 7; indexData[20] = 5;
		indexData[21] = 7; indexData[22] = 6; indexData[23] = 5;

		//上面
		indexData[24] = 0; indexData[25] = 3; indexData[26] = 4;
		indexData[27] = 3; indexData[28] = 7; indexData[29] = 4;

		//下面
		indexData[30] = 2; indexData[31] = 1; indexData[32] = 6;
		indexData[33] = 1; indexData[34] = 5; indexData[35] = 6;

	}

}
