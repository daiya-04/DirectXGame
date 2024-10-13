#include "BaseScar.h"

#include "TextureManager.h"
#include "DirectXCommon.h"

ID3D12Device* BaseScar::device_ = nullptr;
ID3D12GraphicsCommandList* BaseScar::commandList_ = nullptr;

void BaseScar::Init(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	maskTex_ = TextureManager::Load("noise0.png");

	vertexBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 4);

	vertexBufferView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	TransferVertex();

	indexBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6);

	indexBufferView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indices = nullptr;
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indices));

	indices[0] = 0;  indices[1] = 1;  indices[2] = 3;
	indices[3] = 1;  indices[4] = 3;  indices[5] = 2;

	worldMatBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Matrix4x4));
	Matrix4x4* MatData = nullptr;
	HRESULT hr = worldMatBuff_->Map(0, nullptr, reinterpret_cast<void**>(&MatData));
	*MatData = MakeIdentity44();

	scarDataBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(float));
	scarDataBuff_->Map(0, nullptr, reinterpret_cast<void**>(&scarData_));
}

void BaseScar::Update() {



}

void BaseScar::Draw(const Camera& camera) {

	scarData_->threshold_ = threshold_;
	scarData_->color_ = color_;

	Matrix4x4* MatData = nullptr;
	HRESULT hr = worldMatBuff_->Map(0, nullptr, reinterpret_cast<void**>(&MatData));
	*MatData = MakeAffineMatrix({ scale_.x,1.0f,scale_.y }, { 0.0f,rotate_,0.0f }, position_);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);

	//wvp用のCBufferの場所の設定
	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kWorldTransform, worldMatBuff_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kCamera, camera.GetGPUVirtualAddress());

	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, (UINT)RootParameter::kTexture, textureHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, (UINT)RootParameter::kMaskTex, maskTex_);

	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kScarData, scarDataBuff_->GetGPUVirtualAddress());

	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void BaseScar::EffectStart(const Vector3& pos) {

	position_ = pos;
	isEffect_ = true;
	lifeTimer_ = lifeTime_;
	threshold_ = 0.0f;

}

void BaseScar::TransferVertex() {

	//頂点データを設定する
	VertexData* vertexData = nullptr;
	vertexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//1枚目の三角形
	vertexData[0].pos_ = { -1.0f,0.0f,1.0f,1.0f };//左上
	vertexData[0].uv_ = { 0.0f,0.0f };
	vertexData[1].pos_ = { -1.0f,0.0f,-1.0f,1.0f };//左下
	vertexData[1].uv_ = { 0.0f,1.0f };
	vertexData[2].pos_ = { 1.0f,0.0f,-1.0f,1.0f };//右下
	vertexData[2].uv_ = { 1.0f,1.0f };
	vertexData[3].pos_ = { 1.0f,0.0f,1.0f,1.0f };//右上
	vertexData[3].uv_ = { 1.0f,0.0f };

}

Microsoft::WRL::ComPtr<ID3D12Resource> BaseScar::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
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
