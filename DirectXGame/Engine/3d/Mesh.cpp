#include "Mesh.h"
#include "DirectXCommon.h"
#include <iostream>

using namespace Microsoft::WRL;

void Mesh::Init() {
	//頂点リソース作成
	vertexBuff_ = CreateBufferResource(sizeof(VertexData) * vertices_.size());
	//頂点バッファビューの作成
	vertexBufferView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();  //リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size()); //使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);  //1頂点当たりのサイズ

	vertexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	//indexリソースを作る
	indexBuff_ = CreateBufferResource(sizeof(uint32_t) * indices_.size());
	//indexバッファビューを作成する
	indexBufferView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//indexリソースにデータを書き込む
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, indices_.data(), sizeof(uint32_t) * indices_.size());

	material_.Init();

}

void Mesh::SkinnedInit() {

	D3D12_SHADER_RESOURCE_VIEW_DESC vertexSrvDesc{};
	vertexSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	vertexSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	vertexSrvDesc.Buffer.FirstElement = 0;
	vertexSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	vertexSrvDesc.Buffer.NumElements = (UINT)vertices_.size();
	vertexSrvDesc.Buffer.StructureByteStride = sizeof(VertexData);
	UINT handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	vertexSrvHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	vertexSrvHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(vertexBuff_.Get(), &vertexSrvDesc, vertexSrvHandle_.first);


	UavInit();

}

void Mesh::UavInit() {

	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapproperties{};
	uploadHeapproperties.Type = D3D12_HEAP_TYPE_DEFAULT;//UploadHeapを使う
	//リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeof(VertexData) * vertices_.size(); //リソースのサイズ。
	//バッファの場合はこれにする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれらにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	HRESULT hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&uavResource_));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = (UINT)vertices_.size();
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = sizeof(VertexData);
	UINT handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	vertexUavHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	vertexUavHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(uavResource_.Get(), nullptr, &uavDesc, vertexUavHandle_.first);

	skinnedVBV_.BufferLocation = uavResource_->GetGPUVirtualAddress();
	skinnedVBV_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	skinnedVBV_.StrideInBytes = sizeof(VertexData);

}

ComPtr<ID3D12Resource> Mesh::CreateBufferResource(size_t sizeInBytes) {
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
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	assert(device != nullptr);
	HRESULT hr = device->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}
