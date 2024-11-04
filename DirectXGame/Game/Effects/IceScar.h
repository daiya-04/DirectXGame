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
#include "BaseScar.h"

#include <string>

class IceScar : public BaseScar {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private: //静的メンバ変数
	
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public: //静的メンバ関数

	//静的初期化
	static void StaticInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//モデルの生成
	static IceScar* Create(uint32_t textureHandle);
	//描画前処理
	static void preDraw();

private:
	//シェーダのコンパイル
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

public:

	void Init(uint32_t textureHandle) override;

	void Update() override;
	void EffectUpdate() override;

	void Draw(const Camera& camera) override;

	void EffectStart(const Vector3& pos) override;
	void HeightAdjustment(float height) override { BaseScar::HeightAdjustment(height); }

	bool IsEffect() const override { return BaseScar::IsEffect(); }


};

