#include "Material.h"
#include "DirectXCommon.h"

using namespace Microsoft::WRL;

namespace DaiEngine {
	void Material::Init() {
		//リソース作成
		cBuff_ = CreateBufferResource(sizeof(Data));

		cBuff_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		//データの書き込み
		data_->color_ = Vector4({ 1.0f,1.0f,1.0f,1.0f });
		data_->enableLightnig_ = true;
		data_->uvTransform_ = MakeIdentity44();
		data_->shininess_ = 10.0f;

	}

	ComPtr<ID3D12Resource> Material::CreateBufferResource(size_t sizeInBytes) {
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
		HRESULT hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
		assert(SUCCEEDED(hr));

		return Resource;
	}

}
