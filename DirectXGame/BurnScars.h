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
#include <functional>
#include <map>
#include <optional>

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

private:

	//座標
	Vector3 position_{};
	//倍率
	Vector2 scale_ = { 1.5f,1.5f };
	//回転
	float rotate_{};

	float threshold_ = 0.0f;

	bool isLife_ = false;

	int32_t lifeTime_ = 60 * 4;

public: //メンバ関数

	//初期化
	void Initialize(uint32_t textureHandle);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);

	void EffectStart(const Vector3& pos);
	void HieghtAdjustment(float hieght) { position_.y = hieght; }

	bool IsLife() const { return isLife_; }

private:

	enum class Phase {
		kHide,
		kVisible,
	};

	Phase phase_ = Phase::kHide;
	std::optional<Phase> phaseRequest_ = Phase::kHide;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kHide,[this]() {HideInit(); }},
		{Phase::kVisible,[this]() {VisibleInit(); }},
	};

	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kHide,[this]() {HideUpdate(); }},
		{Phase::kVisible,[this]() {VisibleUpdate(); }},
	};

private:

	void HideInit();
	void HideUpdate();

	void VisibleInit();
	void VisibleUpdate();

private:

	void TransferVertex();

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

};

