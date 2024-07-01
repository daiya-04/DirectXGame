#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <vector>
#include "WorldTransform.h"
#include "Camera.h"
#include "ModelManager.h"
#include "Animation.h"
#include "SkinCluster.h"


class SkinningObject{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
private:

	enum class RootParameter {
		kMaterial,
		kWorldTransform,
		kCamera,
		kTexture,
		kDirectionLight,

		kParamNum,
	};

	enum class ComputeRootParam {
		kPalette,
		kInputVertex,
		kInfluence,
		kOutputVertex,
		kSkinningInfo,

		kParamNum,
	};

	struct SkinningInfoData {
		uint32_t numVertex_;
	};

private: //静的メンバ変数

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12RootSignature> computeRootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	static ComPtr<ID3D12PipelineState> computePipelineState_;

public: //静的メンバ関数

	//静的初期化
	static void StaticInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//モデルの生成
	static SkinningObject* Create(std::shared_ptr<Model> model);
	//描画前処理
	static void preDraw();
	//描画後処理
	static void postDraw();

	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

private:
	//シェーダのコンパイル
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

private: //メンバ変数

	std::shared_ptr<Model> model_;
	SkinCluster* skinCluster_;

	ComPtr<ID3D12Resource> skinningInfoBuff_;
	SkinningInfoData* skinningInfoData_;

public:

	WorldTransform worldTransform_;

public: //メンバ関数

	//初期化
	void Initialize(std::shared_ptr<Model> model);
	//更新
	//void Update();
	//描画
	void Draw(const Camera& camera);

	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

	void SetModelHandle(std::shared_ptr<Model> model) { model_ = model; }

	void SetSkinCluster(SkinCluster* skinCluster) { skinCluster_ = skinCluster; }

	std::shared_ptr<Model> GetModel() const { return model_; }

};

