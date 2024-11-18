#include "GPUParticle.h"

#include <cassert>
#include "TextureManager.h"
#include "DirectXCommon.h"
#include "PipelineManager.h"
#include "RootParameters.h"
#include "RandomEngine.h"

using namespace Microsoft::WRL;
using namespace GPUParticleParam;


GPUParticle* GPUParticle::Create(uint32_t textureHandle, int32_t particleNum) {

	GPUParticle* particle = new GPUParticle();

	particle->Init(textureHandle, particleNum);

	return particle;
}

void GPUParticle::preDraw() {

	PipelineManager::GetInstance()->preDraw("GPUParticle");

}

void GPUParticle::postDraw() {}

void GPUParticle::Init(uint32_t textureHandle, int32_t particleNum) {

	uvHandle_ = textureHandle;
	maxParticleNum_ = particleNum;

	commandList_ = DirectXCommon::GetInstance()->GetCommandList();
	device_ = DirectXCommon::GetInstance()->GetDevice();

	CreateBuffer();

	particleData_ = {};

	particleData_.isLoop_ = true;
	particleData_.emitter_.billboardType = BillboardType::Billboard;
	particleData_.textureName_ = TextureManager::GetInstance()->GetTextureName(uvHandle_);

	ExecuteInitCS();

}

void GPUParticle::Update() {

	perFrameData->time += (1.0f / 60.0f);

	if (particleData_.isLoop_) {
		
		perFrameData->seed = RandomEngine::GetInstance()->GetIntRandom(0, 10);
		particleData_.emitter_.frequencyTime += (1.0f / 60.0f);

		if (particleData_.emitter_.frequency <= particleData_.emitter_.frequencyTime) {
			particleData_.emitter_.frequencyTime -= particleData_.emitter_.frequency;
			particleData_.emitter_.emit = 1;
		}
		else {
			particleData_.emitter_.emit = 0;
		}
	}
	

	
	std::memcpy(overLifeTimeData_, &particleData_.overLifeTime_, sizeof(OverLifeTime));

	if (particleData_.emitter_.emit == 1) {
		std::memcpy(emitterData_, &particleData_.emitter_, sizeof(Emitter));

		ExecuteEmitCS();

	}
	

	
	
	ExecuteUpdateCS();

	if (!particleData_.isLoop_ && particleData_.emitter_.emit == 1) {
		particleData_.emitter_.emit = 0;
	}

}

void GPUParticle::Draw(const Camera& camera) {

	perViewData_->viewProjectionMat_ = camera.GetMatView() * camera.GetMatProj();
	perViewData_->billboardMat_ = camera.GetBillBoadMatrix();

	D3D12_RESOURCE_BARRIER preBarrier = {};
	preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	preBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	preBarrier.Transition.pResource = particleBuff_.Get();
	preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &preBarrier);

	//VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vbv_);
	commandList_->IASetIndexBuffer(&ibv_);
	
	//wvp用のCBufferの場所の設定
	//commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(static_cast<UINT>(RootParameter::kParticleGPU), srvHandle_.second);
	commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kPerView), perViewBuff_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, static_cast<UINT>(RootParameter::kTexture), uvHandle_);

	commandList_->DrawIndexedInstanced(6, maxParticleNum_, 0, 0, 0);

	D3D12_RESOURCE_BARRIER postBarrier = {};
	postBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	postBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	postBarrier.Transition.pResource = particleBuff_.Get();
	postBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	postBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	postBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &postBarrier);

}

void GPUParticle::SetParticleData(const ParticleData& particleData) {
	particleData_ = particleData;
	SetTextureHandle();
}

void GPUParticle::SetTextureHandle() {
	uvHandle_ = TextureManager::Load(particleData_.textureName_);
}

void GPUParticle::CreateBuffer() {

	if (vertexBuff_ != nullptr) { return; }

	vertexBuff_ = CreateBufferResource(device_, sizeof(VertexData) * 4);

	vbv_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = sizeof(VertexData) * 4;
	vbv_.StrideInBytes = sizeof(VertexData);

	VertexData* vertices = nullptr;
	vertexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertices));

	//左下
	vertices[0].pos_ = { -1.0f,-1.0f,0.0f,1.0f };
	vertices[0].uv_ = { 0.0f,1.0f };
	//左上
	vertices[1].pos_ = { -1.0f,1.0f,0.0f,1.0f };
	vertices[1].uv_ = { 0.0f,0.0f };
	//右下
	vertices[2].pos_ = { 1.0f,-1.0f,0.0f,1.0f };
	vertices[2].uv_ = { 1.0f,1.0f };

	//右上
	vertices[3].pos_ = { 1.0f,1.0f,0.0f,1.0f };
	vertices[3].uv_ = { 1.0f,0.0f };
	//右下
	//vertices[5] = vertices[2];

	indexBuff_ = CreateBufferResource(device_, sizeof(uint32_t) * 6);

	ibv_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibv_.SizeInBytes = sizeof(uint32_t) * 6;
	ibv_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indices = nullptr;
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indices));

	indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
	indices[3] = 1;  indices[4] = 3;  indices[5] = 2;

	perViewBuff_ = CreateBufferResource(device_, sizeof(PerView));

	perViewBuff_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));
	perViewData_->viewProjectionMat_ = MakeIdentity44();
	perViewData_->billboardMat_ = MakeIdentity44();

	emitterBuff_ = CreateBufferResource(device_, sizeof(Emitter));

	emitterBuff_->Map(0, nullptr, reinterpret_cast<void**>(&emitterData_));

	perFrameBuff_ = CreateBufferResource(device_, sizeof(PerFrame));
	
	perFrameBuff_->Map(0, nullptr, reinterpret_cast<void**>(&perFrameData));
	perFrameData->time = 0;
	perFrameData->deltaTime = 1.0f / 60.0f;

	maxParticleNumBuff_ = CreateBufferResource(device_, sizeof(MaxParticleNum));

	maxParticleNumBuff_->Map(0, nullptr, reinterpret_cast<void**>(&maxParticleNumData_));
	maxParticleNumData_->maxNum = maxParticleNum_;

	OverLifeTimeBuff_ = CreateBufferResource(device_, sizeof(OverLifeTime));

	OverLifeTimeBuff_->Map(0, nullptr, reinterpret_cast<void**>(&overLifeTimeData_));
	ZeroMemory(overLifeTimeData_, sizeof(OverLifeTime));

	CreateUav();

}

void GPUParticle::CreateUav() {

	//particleBuff_の生成
	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapproperties{};
	uploadHeapproperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeof(ParticleCS) * maxParticleNum_; //リソースのサイズ。
	//バッファの場合はこれにする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれらにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	HRESULT hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&particleBuff_));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticleNum_;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = sizeof(ParticleCS);
	UINT handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	uavHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	uavHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(particleBuff_.Get(), nullptr, &uavDesc, uavHandle_.first);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = maxParticleNum_;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleCS);
	handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	srvHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(particleBuff_.Get(), &srvDesc, srvHandle_.first);

	//freeListIndexBuff_の生成
	D3D12_HEAP_PROPERTIES freeListIndexHeapProp{};
	freeListIndexHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC freeListIndexResourceDesc{};
	freeListIndexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	freeListIndexResourceDesc.Width = sizeof(uint32_t);
	freeListIndexResourceDesc.Height = 1;
	freeListIndexResourceDesc.DepthOrArraySize = 1;
	freeListIndexResourceDesc.MipLevels = 1;
	freeListIndexResourceDesc.SampleDesc.Count = 1;
	freeListIndexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	freeListIndexResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	hr = device_->CreateCommittedResource(&freeListIndexHeapProp, D3D12_HEAP_FLAG_NONE, &freeListIndexResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListIndexBuff_));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC freeListIndexUavDesc{};
	freeListIndexUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	freeListIndexUavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	freeListIndexUavDesc.Buffer.FirstElement = 0;
	freeListIndexUavDesc.Buffer.NumElements = 1;
	freeListIndexUavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	freeListIndexUavDesc.Buffer.StructureByteStride = sizeof(uint32_t);
	handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	freeListIndexUavHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	freeListIndexUavHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(freeListIndexBuff_.Get(), nullptr, &freeListIndexUavDesc, freeListIndexUavHandle_.first);

	//freeListBuff_の生成
	D3D12_HEAP_PROPERTIES freeListHeapProp{};
	freeListHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC freeListResourceDesc{};
	freeListResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	freeListResourceDesc.Width = sizeof(uint32_t) * maxParticleNum_;
	freeListResourceDesc.Height = 1;
	freeListResourceDesc.DepthOrArraySize = 1;
	freeListResourceDesc.MipLevels = 1;
	freeListResourceDesc.SampleDesc.Count = 1;
	freeListResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	freeListResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	hr = device_->CreateCommittedResource(&freeListHeapProp, D3D12_HEAP_FLAG_NONE, &freeListResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListBuff_));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC freeListUavDesc{};
	freeListUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	freeListUavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	freeListUavDesc.Buffer.FirstElement = 0;
	freeListUavDesc.Buffer.NumElements = maxParticleNum_;
	freeListUavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	freeListUavDesc.Buffer.StructureByteStride = sizeof(uint32_t);
	handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	freeListUavHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	freeListUavHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(freeListBuff_.Get(), nullptr, &freeListUavDesc, freeListUavHandle_.first);
}

void GPUParticle::ExecuteInitCS() {

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInstance()->GetSrvHeap() };
	commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_RESOURCE_BARRIER preBarrier = {};
	preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	preBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	preBarrier.Transition.pResource = particleBuff_.Get();
	preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &preBarrier);

	PipelineManager::GetInstance()->preDispatch("ParticleInit");

	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(InitRootParam::kParticles), uavHandle_.second);
	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(InitRootParam::kFreeListIndex), freeListIndexUavHandle_.second);
	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(InitRootParam::kFreeList), freeListUavHandle_.second);
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(InitRootParam::kMaxNum), maxParticleNumBuff_->GetGPUVirtualAddress());

	commandList_->Dispatch(static_cast<UINT>(maxParticleNum_ + 1023) / 1024, 1, 1);

	D3D12_RESOURCE_BARRIER postBarrier = {};
	postBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	postBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	postBarrier.Transition.pResource = particleBuff_.Get();
	postBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	postBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	postBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &postBarrier);

}

void GPUParticle::ExecuteEmitCS() {

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInstance()->GetSrvHeap() };
	commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_RESOURCE_BARRIER preBarrier = {};
	preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	preBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	preBarrier.Transition.pResource = particleBuff_.Get();
	preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &preBarrier);

	PipelineManager::GetInstance()->preDispatch("ParticleEmit");

	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(EmitterRootParam::kParticles), uavHandle_.second);
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(EmitterRootParam::kEmitter), emitterBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(EmitterRootParam::kPerFrame), perFrameBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(EmitterRootParam::kFreeListIndex), freeListIndexUavHandle_.second);
	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(EmitterRootParam::kFreeList), freeListUavHandle_.second);
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(EmitterRootParam::kMaxNum), maxParticleNumBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(EmitterRootParam::kOverLifeTime), OverLifeTimeBuff_->GetGPUVirtualAddress());

	commandList_->Dispatch(1, 1, 1);

	D3D12_RESOURCE_BARRIER connectBarrier{};
	connectBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	connectBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	connectBarrier.UAV.pResource = particleBuff_.Get();
	commandList_->ResourceBarrier(1, &connectBarrier);

}

void GPUParticle::ExecuteUpdateCS() {

	if (particleData_.emitter_.emit == 0) {
		//描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInstance()->GetSrvHeap() };
		commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		D3D12_RESOURCE_BARRIER preBarrier = {};
		preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		preBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		preBarrier.Transition.pResource = particleBuff_.Get();
		preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList_->ResourceBarrier(1, &preBarrier);
	}

	PipelineManager::GetInstance()->preDispatch("ParticleUpdate");

	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(UpdateRootParam::kParticles), uavHandle_.second);
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(UpdateRootParam::kPerFrame), perFrameBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(UpdateRootParam::kFreeListIndex), freeListIndexUavHandle_.second);
	commandList_->SetComputeRootDescriptorTable(static_cast<UINT>(UpdateRootParam::kFreeList), freeListUavHandle_.second);
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(UpdateRootParam::kMaxNum), maxParticleNumBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootConstantBufferView(static_cast<UINT>(UpdateRootParam::kOverLifeTime), OverLifeTimeBuff_->GetGPUVirtualAddress());

	commandList_->Dispatch(static_cast<UINT>(maxParticleNum_ + 1023) / 1024, 1, 1);

	D3D12_RESOURCE_BARRIER postBarrier = {};
	postBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	postBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	postBarrier.Transition.pResource = particleBuff_.Get();
	postBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	postBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	postBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &postBarrier);


}

ComPtr<ID3D12Resource> GPUParticle::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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