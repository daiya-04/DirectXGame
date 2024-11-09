#include "Line.h"
#include <cassert>
#include <Windows.h>
#include "DirectXCommon.h"
#include "Log.h"
#include "PipelineManager.h"

using namespace Microsoft::WRL;

UINT Line::drawNum_ = 0;
const uint32_t Line::kMaxDrawNum_ = 4096;
Line::DataForGPU* Line::datas_ = nullptr;
ComPtr<ID3D12Resource> Line::instancingResource_;
D3D12_CPU_DESCRIPTOR_HANDLE Line::srvHandleCPU_{};
D3D12_GPU_DESCRIPTOR_HANDLE Line::srvHandleGPU_{};

void Line::Init() {

	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	instancingResource_ = CreateBufferResource(device, sizeof(DataForGPU) * kMaxDrawNum_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = kMaxDrawNum_;
	srvDesc.Buffer.StructureByteStride = sizeof(DataForGPU);
	UINT handleSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandleCPU_ = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	srvHandleGPU_ = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	device->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, srvHandleCPU_);

	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&datas_));

}

void Line::preDraw() {
	PipelineManager::GetInstance()->preDraw("Line");
}

void Line::AllDraw() {

	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);

	commandList->DrawInstanced(2, drawNum_, 0, 0);
	drawNum_ = 0;
}

void Line::Draw(const Vector3& start, const Vector3& end, const Camera& camera, const Vector4& color) {
	//描画数が最大数以上だったら止める
	assert(drawNum_ < kMaxDrawNum_);
	//始点と終点が同じだったら止める。線にならん
	//assert(start != end);

	datas_[drawNum_].color_ = color;

	Vector3 scale{};
	scale.x = (end - start).Length();
	Vector3 to = (end - start).Normalize();
	Vector3 translate = start;

	Matrix4x4 SMat = MakeScaleMatrix(scale);
	Matrix4x4 RMat = DirectionToDirection({ 1.0f,0.0f,0.0f }, to);
	Matrix4x4 TMat = MakeTranslateMatrix(translate);

	Matrix4x4 SRTMat = SMat * RMat * TMat;

	datas_[drawNum_].wvp_ = SRTMat * (camera.GetMatView() * camera.GetMatProj());
	drawNum_++;

}

ComPtr<ID3D12Resource> Line::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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
