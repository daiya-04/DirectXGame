#include "Camera.h"
#include "DirectXCommon.h"
#include <cassert>

void Camera::Init() {

	CreateCBuffer();
	Map();
	UpdateMatrix();

}

void Camera::CreateCBuffer() {

	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapproperties{};
	uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeof(CBufferDataCamera); //リソースのサイズ。
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

void Camera::Map() {

	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cMap_));
	cMap_->matView = matView_;
	cMap_->matProjection = matProjection_;
	Matrix4x4 matWorld = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_);
	cMap_->cameraPos = { matWorld.m[3][0],matWorld.m[3][1] ,matWorld.m[3][2] };

}

void Camera::UpdateMatrix() {

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix() {
	matView_ = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_).Inverse();
	Map();
}

void Camera::UpdateProjectionMatrix() {
	matProjection_ = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}