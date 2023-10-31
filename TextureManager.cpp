#include "TextureManager.h"
#include "DirectXCommon.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cassert>

using namespace DirectX;
using namespace Microsoft::WRL;

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;

	return &instance;
}

uint32_t TextureManager::Load(const std::string& fileName) {
	return TextureManager::GetInstance()->LoadInternal(fileName);
}

uint32_t TextureManager::LoadUv(const std::string& fileName, const std::string& filePath) {
	return TextureManager::GetInstance()->LoadUvInternal(fileName, filePath);
}

void TextureManager::Initialize() {

	device_ = DirectXCommon::GetInstance()->GetDevice();

	srvDescriptorHandleSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void TextureManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle) {
	assert(textureHandle < kNumTextures);

	commandList->SetGraphicsRootDescriptorTable(rootParamIndex, textures_[textureHandle].textureSrvHandleGPU_);
}

ComPtr<ID3D12Resource> TextureManager::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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

uint32_t TextureManager::LoadInternal(const std::string& fileName){

	assert(useTextureNum_ < kNumTextures);
	uint32_t handle = useTextureNum_;

	//読み込み済みのテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {return texture.name == fileName; });
	
	if (it != textures_.end()) {
		//読み込み済みのテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}

	Texture& texture = textures_.at(useTextureNum_);
	texture.name = fileName;

	//ディレクトリパスとファイル名を連結してフルパスを得る
	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullPath = currentRelative ? fileName : directoryPath_ + fileName;

	//ユニコード文字列に変換
	wchar_t wFilePath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath, _countof(wFilePath));

	HRESULT hr;

	TexMetadata metadata{};
	ScratchImage image{};

	//WICテクスチャのロード
	hr = LoadFromWICFile(wFilePath, WIC_FLAGS_NONE, &metadata, image);
	assert(SUCCEEDED(hr));

	ScratchImage mipImages{};
	//みっぷマップ生成
	hr= GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipImages);
	if (SUCCEEDED(hr)) {
		image = std::move(mipImages);
		metadata = image.GetMetadata();
	}

	metadata.format = MakeSRGB(metadata.format);

	//metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き　or　配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	//利用するHeapの設定。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作成

	//Resourceの生成
	hr = device_->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, //データ転送される設定
		nullptr, //Clear最適地。使わないのでnullptr
		IID_PPV_ARGS(&texture.resource) //作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	//読み込んだデータからDirectX12用のSubresouceの配列を作成する
	PrepareUpload(device_, image.GetImages(), image.GetImageCount(), image.GetMetadata(), subresource);
	//intermediateResourceに必要なサイズを計算する
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.resource.Get(), 0, UINT(subresource.size()));
	//計算したサイズでintermediateResourceを作る
	intermediateResource = CreateBufferResource(device_, intermediateSize);
	//データ転送をコマンドに積む
	UpdateSubresources(DirectXCommon::GetInstance()->GetCommandList(), texture.resource.Get(), intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.resource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	DirectXCommon::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

	//シェーダーリソースビュー作成
	texture.textureSrvHandleCPU_ = GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), srvDescriptorHandleSize_, DirectXCommon::GetInstance()->GetSrvHeapCount());
	texture.textureSrvHandleGPU_ = GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), srvDescriptorHandleSize_, DirectXCommon::GetInstance()->GetSrvHeapCount());

	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	//metadataを基にSRVの設定
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	device_->CreateShaderResourceView(texture.resource.Get(), &srvDesc, texture.textureSrvHandleCPU_);

	useTextureNum_++;
	return handle;
}

uint32_t TextureManager::LoadUvInternal(const std::string& fileName, const std::string& filePath) {
	assert(useTextureNum_ < kNumTextures);
	uint32_t handle = useTextureNum_;

	//読み込み済みのテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {return texture.name == fileName; });

	if (it != textures_.end()) {
		//読み込み済みのテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}

	Texture& texture = textures_.at(useTextureNum_);
	texture.name = fileName;

	//ディレクトリパスとファイル名を連結してフルパスを得る
	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullPath = currentRelative ? fileName : filePath;

	//ユニコード文字列に変換
	wchar_t wFilePath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath, _countof(wFilePath));

	HRESULT hr;

	TexMetadata metadata{};
	ScratchImage image{};

	//WICテクスチャのロード
	hr = LoadFromWICFile(wFilePath, WIC_FLAGS_NONE, &metadata, image);
	assert(SUCCEEDED(hr));

	ScratchImage mipImages{};
	//みっぷマップ生成
	hr = GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipImages);
	if (SUCCEEDED(hr)) {
		image = std::move(mipImages);
		metadata = image.GetMetadata();
	}

	metadata.format = MakeSRGB(metadata.format);

	//metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き　or　配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	//利用するHeapの設定。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作成

	//Resourceの生成
	hr = device_->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, //データ転送される設定
		nullptr, //Clear最適地。使わないのでnullptr
		IID_PPV_ARGS(&texture.resource) //作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	//読み込んだデータからDirectX12用のSubresouceの配列を作成する
	PrepareUpload(device_, image.GetImages(), image.GetImageCount(), image.GetMetadata(), subresource);
	//intermediateResourceに必要なサイズを計算する
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.resource.Get(), 0, UINT(subresource.size()));
	//計算したサイズでintermediateResourceを作る
	intermediateResource = CreateBufferResource(device_, intermediateSize);
	//データ転送をコマンドに積む
	UpdateSubresources(DirectXCommon::GetInstance()->GetCommandList(), texture.resource.Get(), intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.resource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	DirectXCommon::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

	//シェーダーリソースビュー作成
	texture.textureSrvHandleCPU_ = GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), srvDescriptorHandleSize_, DirectXCommon::GetInstance()->GetSrvHeapCount());
	texture.textureSrvHandleGPU_ = GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), srvDescriptorHandleSize_, DirectXCommon::GetInstance()->GetSrvHeapCount());

	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	//metadataを基にSRVの設定
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	device_->CreateShaderResourceView(texture.resource.Get(), &srvDesc, texture.textureSrvHandleCPU_);

	useTextureNum_++;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}