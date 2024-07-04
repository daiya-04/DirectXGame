#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "Camera.h"
#include <string>

class BurnScars {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	enum class RootParameter {
		kWorldTransform,
		kCamera,
		kTexture,
		kMaskTex,
		kThreshold,

		kParamNum,
	};

	struct VertexData {
		Vector4 pos_;
		Vector2 uv_;
	};


private: //静的メンバ変数

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public: //静的メンバ関数

	//静的初期化
	static void StaticInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//モデルの生成
	static BurnScars* Create(uint32_t textureHandle);
	//描画前処理
	static void preDraw();

private:
	//シェーダのコンパイル
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

private:

	ComPtr<ID3D12Resource> worldMatBuff_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexBuff_;
	ComPtr<ID3D12Resource> indexBuff_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	ComPtr<ID3D12Resource> thresholdBuff_;
	float* thresholdData_ = nullptr;

	uint32_t textureHandle_ = 0;
	uint32_t maskTex_ = 0;

public:

	//座標
	Vector3 position_{};
	//倍率
	Vector2 scale_ = { 1.0f,1.0f };
	//回転
	float rotate_{};

	float threshold_ = 0.5f;

public: //メンバ関数

	//初期化
	void Initialize(uint32_t textureHandle);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);

private:

	void TransferVertex();

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

};

