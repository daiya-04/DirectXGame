#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <string>
#include <array>



class Sprite{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	struct VertexData {
		Vector4 pos_;
		Vector2 uv_;
	};

	struct MaterialData {
		Vector4 color_;
	};

private:  //静的メンバ変数

	//共通部分の変数
	static ID3D12Device* device_ ;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	static Matrix4x4 projectionMatrix_;

public: //静的メンバ関数

	//共通部分の関数

	static void StaticInitialize(ID3D12Device* device,int windowWidth,int windowHeight);

	static void preDraw(ID3D12GraphicsCommandList* commandList);

	static void postDraw();

	static void Finalize();

	//static uint32_t LoadTexture(const std::string& fileName);

	/// <summary>
	/// シェーダーのコンパイル
	/// </summary>
	/// <param name="filePath">CompilerするShaderファイルへのパス</param>
	/// <param name="profile">Compilerに使用するProfile</param>
	/// <param name="dxcUtils"></param>
	/// <param name="dxcCompiler"></param>
	/// <param name="includeHandleer"></param>
	/// <returns></returns>
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

private:

	//ログ
	static void Log(const std::string& message);

	static std::wstring ConvertString(const std::string& str);

	static std::string ConvertString(const std::wstring& str);

private: //メンバ変数



	ComPtr<ID3D12Resource> matrialResource_;
	ComPtr<ID3D12Resource> wvpResource_;
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//座標
	Vector2 position_{};
	//サイズ
	Vector2 size_ = { 100.0f,100.0f };
	//回転
	float rotate_{};
	//アンカーポイント
	Vector2 anchorpoint_{};
	//色(RGBA)
	Vector4 color_{ 1.0f,1.0f,1.0f,1.0f };
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

public: //メンバ関数
	
	Sprite();
	Sprite(uint32_t textureHandle,Vector2 position, Vector2 size, float rotate = 0.0f, Vector2 anchorpoint = { 0.0f,0.0f }, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

	void Initialize();

	void Draw();

	//座標の設定
	void SetPosition(const Vector2& position);
	//サイズの設定
	void SetSize(const Vector2& size);
	//回転の設定
	void SetRotate(float rotate);
	//アンカーポイントの設定
	void SetAnchorpoint(const Vector2& anchorpoint);
	//色の設定
	void SetColor(const Vector4& color);
	//座標の取得
	const Vector2& GetPosition() const { return position_; }
	//サイズの取得
	const Vector2& GetSize() const { return size_; }
	//回転の取得
	float GetRotate() const { return rotate_; }
	//アンカーポイントの取得
	const Vector2& GetAnchorpoint() const { return anchorpoint_; }
	//色の取得
	const Vector4& GetColor() const { return color_; }
	

private:

	void TransferVertex();

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

};
