#include "Sprite.h"
#include <cassert>
#include <format>
#include <Windows.h>
#include "WinApp.h"
#include "TextureManager.h"
#include "Log.h"
#include "DirectXCommon.h"
#include "PipelineManager.h"
#include "RootParameters.h"

using namespace Microsoft::WRL;
using namespace SpriteParam;

Sprite* Sprite::Create(uint32_t textureHandle, Vector2 postion, float scale) {

	Sprite* sprite = new Sprite(textureHandle, postion, scale);
	sprite->Initialize();

	return sprite;
}

void Sprite::preDraw(){

	PipelineManager::GetInstance()->preDraw("Sprite");

}

void Sprite::postDraw() {

}

Sprite::Sprite(uint32_t textureHandle, Vector2 position, float scale, Vector2 anchorpoint, Vector4 color, float rotate) {

	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
	position_ = position;
	size_ = { (float)resourceDesc_.Width * scale,(float)resourceDesc_.Height * scale};
	rotate_ = rotate;
	anchorpoint_ = anchorpoint;
	color_ = color;
	
	texSize_ = { (float)resourceDesc_.Width,(float)resourceDesc_.Height };

}

void Sprite::Initialize() {

	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	//Sprite用の頂点リソースを作る
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 6);

	//頂点バッファビューを作成する
	
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	TransferVertex();

	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);

	indexBufferView.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexMap = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));

	indexMap[0] = 1;  indexMap[1] = 0;  indexMap[2] = 2;
	indexMap[3] = 0;  indexMap[4] = 3;  indexMap[5] = 2;

	//マテリアル用のリソースを作る
	matrialResource_ = CreateBufferResource(device, sizeof(MaterialData));
	//マテリアルにデータを書き込む
	MaterialData* materialData = nullptr;
	//書き込むためのアドレスを取得
	matrialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color_ = color_;

	wvpResource_ = CreateBufferResource(device, sizeof(Matrix4x4));
	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	*wvpData = MakeIdentity44();

}

void Sprite::Draw(){

	
	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,rotate_ }, { position_.x,position_.y,0.0f });
	Matrix4x4 viewMatrix = MakeIdentity44();
	Matrix4x4 prijectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::kClientWidth), static_cast<float>(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 wvpMatrix = worldMatrix * viewMatrix * prijectionMatrix;
	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	*wvpData = wvpMatrix;
	
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

	commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), matrialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWVPMat), wvpResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, static_cast<UINT>(RootParameter::kTexture), textureHandle_);

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::SetPosition(const Vector2& position) {

	position_ = position;

}

void Sprite::SetSize(const Vector2& size) {

	size_ = size;

	TransferVertex();
}

void Sprite::SetScale(float scale) {

	scale_ = Vector2(scale, scale);

	TransferVertex();
}

void Sprite::SetScale(Vector2 scale) {

	scale_ = scale;

	TransferVertex();
}

void Sprite::SetRotate(float rotate) {

	rotate_ = rotate;

}

void Sprite::SetAnchorpoint(const Vector2& anchorpoint) {

	anchorpoint_ = anchorpoint;

	TransferVertex();
}

void Sprite::SetColor(const Vector4& color) {

	MaterialData* materialData = nullptr;
	//書き込むためのアドレスを取得
	matrialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color_ = color;

}

void Sprite::SetTextureArea(const Vector2& texBase, const Vector2& texSize) {
	texBase_ = texBase;
	texSize_ = texSize;

	TransferVertex();
}

void Sprite::TransferVertex(){

	float left = (0.0f - anchorpoint_.x) * size_.x * scale_.x;
	float right = (1.0f - anchorpoint_.x) * size_.x * scale_.x;
	float top = (0.0f - anchorpoint_.y) * size_.y * scale_.y;
	float bottom = (1.0f - anchorpoint_.y) * size_.y * scale_.y;

	float uvLeft = texBase_.x / (float)resourceDesc_.Width;
	float uvRight = (texBase_.x + texSize_.x) / (float)resourceDesc_.Width;
	float uvTop = texBase_.y / (float)resourceDesc_.Height;
	float uvBottom = (texBase_.y + texSize_.y) / (float)resourceDesc_.Height;

	//頂点データを設定する
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//1枚目の三角形
	vertexData[0].pos_ = { left,top,0.0f,1.0f };//左上
	vertexData[0].uv_ = { uvLeft,uvTop };
	vertexData[1].pos_ = { left,bottom,0.0f,1.0f };//左下
	vertexData[1].uv_ = { uvLeft,uvBottom };
	vertexData[2].pos_ = { right,bottom,0.0f,1.0f };//右下
	vertexData[2].uv_ = { uvRight,uvBottom };
	//2枚目の三角形
	//vertexData[3] = vertexData[1];//左上
	vertexData[3].pos_ = { right,top,0.0f,1.0f };//右上
	vertexData[3].uv_ = { uvRight,uvTop };
	//vertexData[5] = vertexData[2];//右下
	


}

ComPtr<ID3D12Resource> Sprite::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapproperties{};
	uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes; //リソースのサイズ。今回はVector4を3頂点分
	//バッファの場合はこれにする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれらにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	ComPtr<ID3D12Resource> Resource;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}
