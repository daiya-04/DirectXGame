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
#include "ViewProjection.h"

class Object3d{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

private: //静的メンバ変数

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public: //静的メンバ関数

	//静的初期化
	static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//モデルの生成
	static Object3d* Create(uint32_t modelHandle);
	//描画前処理
	static void preDraw();
	//描画後処理
	static void postDraw();

	
private:

	//シェーダのコンパイル
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);
	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	
private: //メンバ変数

	
	ComPtr<ID3D12Resource> wvpResource_;
	ComPtr<ID3D12Resource> directionalLightResource_;

	uint32_t modelHandle_ = 0;

public: //メンバ関数

	//初期化
	void Initialize(uint32_t modelHandle);
	//描画
	void Draw(const WorldTransform& worldTransform,const ViewProjection& viewProjwction);
	
	void SetModelHandle(uint32_t modelHandle) { modelHandle_ = modelHandle; }

private:
	

};
