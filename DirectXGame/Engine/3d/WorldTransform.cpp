#include "WorldTransform.h"
#include "DirectXCommon.h"
#include <cassert>

namespace DaiEngine {
	void WorldTransform::Init() {
		CreateCBuffer();
		Map();
		UpdateMatrix();
	}

	void WorldTransform::CreateCBuffer() {

		//リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapproperties{};
		uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
		//リソースの設定
		D3D12_RESOURCE_DESC ResourceDesc{};
		//バッファリソース。テクスチャの場合はまた別の設定をする
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(CBufferDataWorldTransform); //リソースのサイズ。
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

	void WorldTransform::Map() {
		cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&cMap_));
		cMap_->matWorld = matWorld_;
		cMap_->WorldInverseTranspose = WorldInverseTransposeMat_;
	}

	void WorldTransform::UpdateMatrix() {

		matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

		if (parent_) {
			matWorld_ = matWorld_ * parent_->matWorld_;
		}

		WorldInverseTransposeMat_ = (matWorld_.Inverse()).Transpose();

		cMap_->matWorld = matWorld_;
		cMap_->WorldInverseTranspose = WorldInverseTransposeMat_;
	}

	void WorldTransform::UpdateMatrixRotate(const Matrix4x4& rotateMat) {

		Matrix4x4 S = MakeScaleMatrix(scale_);
		Matrix4x4 T = MakeTranslateMatrix(translation_);

		matWorld_ = S * rotateMat * T;

		if (parent_) {
			matWorld_ = matWorld_ * parent_->matWorld_;
		}

		Matrix4x4 inverMat = matWorld_.Inverse();
		Matrix4x4 e = matWorld_ * inverMat;
		WorldInverseTransposeMat_ = (matWorld_.Inverse()).Transpose();

		cMap_->matWorld = matWorld_;
		cMap_->WorldInverseTranspose = WorldInverseTransposeMat_;

	}
}
