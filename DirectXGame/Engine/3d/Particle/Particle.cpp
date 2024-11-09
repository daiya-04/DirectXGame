#include "Particle.h"
#include <cassert>
#include "TextureManager.h"
#include "DirectXCommon.h"
#include "PipelineManager.h"
#include "RootParameters.h"

using namespace Microsoft::WRL;
using namespace ParticleParam;

Particle* Particle::Create(uint32_t textureHandle, uint32_t particleNum) {

	Particle* particle = new Particle();

	particle->Initialize(textureHandle, particleNum);

	return particle;
}

void Particle::preDraw() {

	PipelineManager::GetInstance()->preDraw("Particle");

}

void Particle::postDraw() {}

Particle::ParticleData Particle::MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate){

	std::uniform_real_distribution<float> distPos(-5.0f, 5.0f);
	std::uniform_real_distribution<float> distVelocity(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 6.0f);
	ParticleData particle;

	//particle.worldTransform_.translation_ = { /*distPos(randomEngine),distPos(randomEngine) ,distPos(randomEngine)*/ };
	particle.worldTransform_.translation_ = translate;
	particle.velocity_ = { distVelocity(randomEngine), distVelocity(randomEngine) ,distVelocity(randomEngine) };
	particle.color_ = { distColor(randomEngine),distColor(randomEngine) ,distColor(randomEngine),1.0 };
	particle.lifeTime_ = 2.0f;
	//particle.lifeTime_ = distTime(randomEngine);
	particle.currentTime_ = 0.0f;


	return particle;
}

std::list<Particle::ParticleData> Particle::Emit(const Particle::Emitter& emitter, std::mt19937& randomEngine){
	std::list<Particle::ParticleData> particles;
	for (uint32_t count = 0; count < emitter.count_; count++) {
		particles.push_back(MakeNewParticle(randomEngine, emitter.translate_));
	}

	return particles;
}

ComPtr<ID3D12Resource> Particle::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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

void Particle::Initialize(uint32_t textureHandle, uint32_t particleNum) {

	uvHandle_ = textureHandle;
	particleMaxNum_ = particleNum;
	CreateMesh();
}

void Particle::Draw(std::list<ParticleData>& particleData,const Camera& camera,bool easeAlpha) {

	ParticleGPU* instancingData = nullptr;
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	particleNum_ = 0;
	for (std::list<ParticleData>::iterator itParticle = particleData.begin(); itParticle != particleData.end();) {
		if ((*itParticle).currentTime_ >= (*itParticle).lifeTime_) {
			itParticle = particleData.erase(itParticle);
			continue;
		}

		Matrix4x4 worldMatrix = MakeScaleMatrix((*itParticle).worldTransform_.scale_) * camera.GetBillBoadMatrix() * MakeTranslateMatrix((*itParticle).worldTransform_.translation_);
		float alpha = 1.0f;
		if (easeAlpha) {
			alpha = 1.0f - ((*itParticle).currentTime_ / (*itParticle).lifeTime_);
		}
		

		if (particleNum_ < particleMaxNum_) {
			instancingData[particleNum_].matWorld = worldMatrix;
			instancingData[particleNum_].color = (*itParticle).color_;
			instancingData[particleNum_].color.w = alpha;
			particleNum_++;
		}
		itParticle++;
	}

	Material* material = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material));
	material->color_ = color_;

	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	//VBVを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(RootParameter::kParticleGPU), particleSrvHandleGPU_);
	commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kCamera), camera.GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, static_cast<UINT>(RootParameter::kTexture), uvHandle_);

	commandList->DrawIndexedInstanced(6, static_cast<UINT>(particleNum_), 0, 0, 0);

}

void Particle::CreateMesh() {

	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 4);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	VertexData* vertices = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertices));

	//左下
	vertices[0].pos_ = { -1.0f,-1.0f,0.0f,1.0f };
	vertices[0].uv_ = { 0.0f,1.0f };
	//左上
	vertices[1].pos_ = { -1.0f,1.0f,0.0f,1.0f };
	vertices[1].uv_ = { 0.0f,0.0f };
	//右下
	vertices[2].pos_ = { 1.0f,-1.0f,0.0f,1.0f };
	vertices[2].uv_ = { 1.0f,1.0f };

	//左上
	//vertices[3] = vertices[1];
	//右上
	vertices[3].pos_ = { 1.0f,1.0f,0.0f,1.0f };
	vertices[3].uv_ = { 1.0f,0.0f };
	//右下
	//vertices[5] = vertices[2];

	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indices = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indices));

	indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
	indices[3] = 1;  indices[4] = 3;  indices[5] = 2;


	materialResource_ = CreateBufferResource(device, sizeof(Material));

	Material* material = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material));
	material->color_ = color_;

	instancingResource_ = CreateBufferResource(device, sizeof(ParticleGPU) * particleMaxNum_);

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = particleMaxNum_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleGPU);
	UINT handleSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	particleSrvHandleCPU_ = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	particleSrvHandleGPU_ = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	device->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, particleSrvHandleCPU_);

}
