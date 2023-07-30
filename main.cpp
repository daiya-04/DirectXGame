#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <string>
#include <format>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "Matrix44.h"
#include "Vec3.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <vector>
#include <numbers>
#include <cmath>
#include <fstream>
#include <sstream>
#include <wrl.h>


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")


using namespace Microsoft::WRL;


struct Vector4 {
	float x, y, z, w;
};

struct Vector2 {
	float x, y;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct Material {
	Vector4 color;
	int32_t enableLightnig;
	float padding[3];
	Matrix4x4 uvtransform;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct ModelData {
	std::vector<VertexData> vertices;
};

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リリースリークチェック
		ComPtr<IDXGIDebug> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};


void Log(const std::string& message);
//string->wstring
std::wstring ConvertString(const std::string& str);
//wstring->string
std::string ConvertString(const std::wstring& str);

IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

DirectX::ScratchImage LoadTexture(const std::string& filePath);

ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

[[nodiscard]]
ComPtr<ID3D12Resource> UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImage, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commadList);

ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height);

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int) {
	D3DResourceLeakChecker leakCheck;

	WinApp* win = nullptr;
	
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"Engine");
	

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG


	

	//DXGIファクトリーの生成
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプターでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		//ソフトウェアアダプタの場合は見なかったことにする
		useAdapter = nullptr;
	}
	assert(useAdapter != nullptr);

	ComPtr<ID3D12Device> device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(device != nullptr);
	Log("Complete create D3D12Device!!\n");//初期化完了のログを出す

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時の止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif // DEBUG


	//コマンドキューを生成
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドアロケーターを生成する
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//困んでアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//スワップチェーンを生成する
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WinApp::kClientWidth;      //画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = WinApp::kClientHeight;    //画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   //色の形式
	swapChainDesc.SampleDesc.Count = 1;   //マルチサンプリングしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;   //ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  //モニタに移したら、中身を破壊
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), win->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));

	const uint32_t descriptorSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//ディスクリプタヒープの生成
	//RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//SRV用のヒープでディスクリプタの数は128。SRVはShader内にで触るものなので、ShaderVisibleはtrue
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	//SwapChainからResourceを引っ張ってくる
	ComPtr<ID3D12Resource> swapChainResource[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResource[0]));
	//うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResource[1]));
	assert(SUCCEEDED(hr));

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;       //出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;  //2dテクスチャとして書き込む
	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらでしてあげる必要がある
	rtvHandles[0] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorRTV, 0);
	device->CreateRenderTargetView(swapChainResource[0].Get(), &rtvDesc, rtvHandles[0]);
	//2つ目のディスクリプタハンドルを得る(自力で)
	rtvHandles[1] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorRTV, 1);
	//2つ目を作る
	device->CreateRenderTargetView(swapChainResource[1].Get(), &rtvDesc, rtvHandles[1]);

	//初期値0でFenceを作る
	ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	//dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
	
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; //0から始まる
	descriptorRange[0].NumDescriptors = 1; //数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;  //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;  //ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;  //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   //PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;   //レジスタ番号0とバインド
	
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;   //VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;   //レジスタ番号0を使う
	
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  //PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;  //レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;   //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);  //配列の長さ

	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	IDxcBlob* verterShaderBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(verterShaderBlob != nullptr);

	IDxcBlob* pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//POSを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;  //InputLayout
	graphicsPipelineStateDesc.VS = { verterShaderBlob->GetBufferPointer(),verterShaderBlob->GetBufferSize() };//VerterShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };//pixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //blendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	const uint32_t kSubdivision = 16;
	const float kLonEvery = float(std::numbers::pi) * 2.0f / float(kSubdivision);
	const float kLatEvery = float(std::numbers::pi) / float(kSubdivision);
	
	//頂点リソースを作る
	ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * kSubdivision * kSubdivision * 6);
	
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * kSubdivision * kSubdivision * 6;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	uint32_t latIndex;
	uint32_t lonIndex;

	for (latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(std::numbers::pi) / 2.0f + kLatEvery * latIndex; //θ
		for (lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery; //φ
			float u = float(lonIndex) / float(kSubdivision);
			float v = 1.0f - float(latIndex) / float(kSubdivision);

			//左下
			vertexData[start].position = { std::cosf(lat) * std::cosf(lon),std::sinf(lat),std::cosf(lat) * std::sinf(lon),1.0f };
			vertexData[start].texcoord = { u,v };
			//左上
			vertexData[start + 1].position = { std::cosf(lat + kLatEvery) * std::cosf(lon),std::sinf(lat + kLatEvery),std::cosf(lat + kLatEvery) * std::sinf(lon),1.0f };
			vertexData[start + 1].texcoord = { u ,v - 1.0f / float(kSubdivision) };
			//右下
			vertexData[start + 2].position = { std::cosf(lat) * std::cosf(lon + kLonEvery),std::sinf(lat),std::cosf(lat) * std::sinf(lon + kLonEvery),1.0f };
			vertexData[start + 2].texcoord = { u + 1.0f / float(kSubdivision),v };
			//右下
			vertexData[start + 3].position = { std::cosf(lat) * std::cosf(lon + kLonEvery),std::sinf(lat),std::cosf(lat) * std::sinf(lon + kLonEvery),1.0f };
			vertexData[start + 3].texcoord = { u + 1.0f / float(kSubdivision),v };
			//左上
			vertexData[start + 4].position = { std::cosf(lat + kLatEvery) * std::cosf(lon),std::sinf(lat + kLatEvery),std::cosf(lat + kLatEvery) * std::sinf(lon),1.0f };
			vertexData[start + 4].texcoord = { u,v - 1.0f / float(kSubdivision) };
			//右上
			vertexData[start + 5].position = { std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),std::sinf(lat + kLatEvery),std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery),1.0f };
			vertexData[start + 5].texcoord = { u + 1.0f / float(kSubdivision),v - 1.0f / float(kSubdivision) };

		}
	}

	for (uint32_t index = 0; index < (kSubdivision * kSubdivision * 6); ++index) {

		vertexData[index].normal.x = vertexData[index].position.x;
		vertexData[index].normal.y = vertexData[index].position.y;
		vertexData[index].normal.z = vertexData[index].position.z;
	}
	

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ComPtr<ID3D12Resource> materialResource = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLightnig = true;
	materialData->uvtransform = MakeIdentity44();
	

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	wvpData->WVP = MakeIdentity44();
	wvpData->World = MakeIdentity44();


	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kClientWidth;
	viewport.Height = WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kClientHeight;

	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	Transform uvTransformSphere = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(win->GetHwnd());
	ImGui_ImplDX12_Init(device.Get(),
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap.Get(),
		GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSRV, 0),
		GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSRV, 0));

	DirectX::ScratchImage mipImages[2];
	ComPtr<ID3D12Resource> textureResource[2];
	ComPtr<ID3D12Resource> intermediateResource[2];

	//Textureを読んで転送する
	mipImages[0] = LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages[0].GetMetadata();
	textureResource[0] = CreateTextureResource(device, metadata);
	intermediateResource[0] = UploadTextureData(textureResource[0], mipImages[0], device, commandList);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc[2] = {};

	//metadataを基にSRVの設定
	srvDesc[0].Format = metadata.format;
	srvDesc[0].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc[0].ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc[0].Texture2D.MipLevels = UINT(metadata.mipLevels);

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[2];
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[2];

	//SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU[0] = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSRV, 1);
	textureSrvHandleGPU[0] = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSRV, 1);
	//SRVの生成
	device->CreateShaderResourceView(textureResource[0].Get(), &srvDesc[0], textureSrvHandleCPU[0]);

	//2枚目のTextureを読んで転送する
	mipImages[1] = LoadTexture("Resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages[1].GetMetadata();
	textureResource[1] = CreateTextureResource(device, metadata2);
	intermediateResource[1] = UploadTextureData(textureResource[1], mipImages[1], device, commandList);

	//metaDataを基にSRVの設定
	srvDesc[1].Format = metadata2.format;
	srvDesc[1].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc[1].ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc[1].Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU[1] = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSRV, 2);
	textureSrvHandleGPU[1] = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSRV, 2);
	//SRVの生成
	device->CreateShaderResourceView(textureResource[1].Get(), &srvDesc[1], textureSrvHandleCPU[1]);

	//DepthStencilTextureをウィンドウのサイズで作成
	ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(device, WinApp::kClientWidth, WinApp::kClientHeight);

	//DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので,ShaderVisibleはfalse
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//DVSの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //2dTexture
	//DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//Sprite用の頂点リソースを作る
	ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//頂点データを設定する
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//1枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexDataSprite[3] = vertexDataSprite[1];//左上
	vertexDataSprite[4].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexDataSprite[4].texcoord = { 1.0f,0.0f };
	vertexDataSprite[5] = vertexDataSprite[2];//右下
	

	//Sprite用の頂点インデックスのリソースを作成
	ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

	//インデックスバッファビューを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	//インデックスリソースにデータを書き込む
	uint32_t* indexDateSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDateSprite));

	indexDateSprite[0] = 0;
	indexDateSprite[1] = 1;
	indexDateSprite[2] = 2;
	
	indexDateSprite[3] = 1;
	indexDateSprite[4] = 4;
	indexDateSprite[5] = 2;


	//Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4　1つ分のサイズを用意する
	ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//単位行列を書き込んでおく
	transformationMatrixDataSprite->WVP = MakeIdentity44();
	

	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//Sprite用のマテリアルリソースを作る
	ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	//データを書き込む
	Material* materialDataSprite = nullptr;
	//書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataSprite->enableLightnig = false;
	materialDataSprite->uvtransform = MakeIdentity44();

	Transform uvTransformSprite = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	//DirectionalLighting用のリソースを作る
	ComPtr<ID3D12Resource> directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	//データを書き込む
	DirectionalLight* directionalLightData = nullptr;
	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;


	//1個目の三角形
	//Shaderをコンパイルする
	IDxcBlob* VSBlobTriangle = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(VSBlobTriangle != nullptr);

	IDxcBlob* PSBlobTriangle = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(PSBlobTriangle != nullptr);

	//POSを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPSDescTriangle{};
	graphicsPSDescTriangle.pRootSignature = rootSignature.Get(); //RootSignature
	graphicsPSDescTriangle.InputLayout = inputLayoutDesc;  //InputLayout
	graphicsPSDescTriangle.VS = { VSBlobTriangle->GetBufferPointer(),VSBlobTriangle->GetBufferSize() };//VerterShader
	graphicsPSDescTriangle.PS = { PSBlobTriangle->GetBufferPointer(),PSBlobTriangle->GetBufferSize() };//pixelShader
	graphicsPSDescTriangle.BlendState = blendDesc; //blendState
	graphicsPSDescTriangle.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPSDescTriangle.NumRenderTargets = 1;
	graphicsPSDescTriangle.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPSDescTriangle.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPSDescTriangle.SampleDesc.Count = 1;
	graphicsPSDescTriangle.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPSDescTriangle.DepthStencilState = depthStencilDesc;
	graphicsPSDescTriangle.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ComPtr<ID3D12PipelineState> graphicsPSTriangle = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPSDescTriangle, IID_PPV_ARGS(&graphicsPSTriangle));
	assert(SUCCEEDED(hr));

	ComPtr<ID3D12Resource> vertexResourceTriangle = CreateBufferResource(device, sizeof(VertexData) * 3);

	D3D12_VERTEX_BUFFER_VIEW vbvTriangle = {};
	//リソースの先頭のアドレスから使う
	vbvTriangle.BufferLocation = vertexResourceTriangle->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vbvTriangle.SizeInBytes = sizeof(VertexData) * 3;
	//1頂点あたりのサイズ
	vbvTriangle.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexDataTriangle = nullptr;
	//書き込むためのアドレスを取得
	vertexResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataTriangle));

	//左下
	vertexDataTriangle[0].position = { -1.0f,-1.0f,0.0f,1.0f };
	vertexDataTriangle[0].texcoord = { 0.0f,1.0f };
	//上
	vertexDataTriangle[1].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexDataTriangle[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexDataTriangle[2].position = { 1.0f,-1.0f,0.0f,1.0f };
	vertexDataTriangle[2].texcoord = { 1.0f,1.0f };


	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ComPtr<ID3D12Resource> materialResourceTriangle = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialDataTriangle = nullptr;
	//書き込むためのアドレスを取得
	materialResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&materialDataTriangle));
	materialDataTriangle->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataTriangle->enableLightnig = false;
	materialDataTriangle->uvtransform = MakeIdentity44();


	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> wvpResourceTriangle = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* wvpDataTriangle = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataTriangle));
	//単位行列を書き込んでおく
	wvpDataTriangle->WVP = MakeIdentity44();
	wvpDataTriangle->World = MakeIdentity44();


	//2個目の三角形
	//Shaderをコンパイルする
	IDxcBlob* VSBlobTriangle2 = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(VSBlobTriangle2 != nullptr);

	IDxcBlob* PSBlobTriangle2 = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(PSBlobTriangle2 != nullptr);

	//POSを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPSDescTriangle2{};
	graphicsPSDescTriangle2.pRootSignature = rootSignature.Get(); //RootSignature
	graphicsPSDescTriangle2.InputLayout = inputLayoutDesc;  //InputLayout
	graphicsPSDescTriangle2.VS = { VSBlobTriangle2->GetBufferPointer(),VSBlobTriangle2->GetBufferSize() };//VerterShader
	graphicsPSDescTriangle2.PS = { PSBlobTriangle2->GetBufferPointer(),PSBlobTriangle2->GetBufferSize() };//pixelShader
	graphicsPSDescTriangle2.BlendState = blendDesc; //blendState
	graphicsPSDescTriangle2.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPSDescTriangle2.NumRenderTargets = 1;
	graphicsPSDescTriangle2.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPSDescTriangle2.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPSDescTriangle2.SampleDesc.Count = 1;
	graphicsPSDescTriangle2.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPSDescTriangle2.DepthStencilState = depthStencilDesc;
	graphicsPSDescTriangle2.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ComPtr<ID3D12PipelineState> graphicsPSTriangle2 = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPSDescTriangle2, IID_PPV_ARGS(&graphicsPSTriangle2));
	assert(SUCCEEDED(hr));

	ComPtr<ID3D12Resource> vertexResourceTriangle2 = CreateBufferResource(device, sizeof(VertexData) * 3);

	D3D12_VERTEX_BUFFER_VIEW vbvTriangle2 = {};
	//リソースの先頭のアドレスから使う
	vbvTriangle2.BufferLocation = vertexResourceTriangle2->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vbvTriangle2.SizeInBytes = sizeof(VertexData) * 3;
	//1頂点あたりのサイズ
	vbvTriangle2.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexDataTriangle2 = nullptr;
	//書き込むためのアドレスを取得
	vertexResourceTriangle2->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataTriangle2));

	//左下
	vertexDataTriangle2[0].position = { -1.0f,-1.0f,0.0f,1.0f };
	vertexDataTriangle2[0].texcoord = { 0.0f,1.0f };
	//上
	vertexDataTriangle2[1].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexDataTriangle2[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexDataTriangle2[2].position = { 1.0f,-1.0f,0.0f,1.0f };
	vertexDataTriangle2[2].texcoord = { 1.0f,1.0f };


	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ComPtr<ID3D12Resource> materialResourceTriangle2 = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialDataTriangle2 = nullptr;
	//書き込むためのアドレスを取得
	materialResourceTriangle2->Map(0, nullptr, reinterpret_cast<void**>(&materialDataTriangle2));
	materialDataTriangle2->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataTriangle2->enableLightnig = false;
	materialDataTriangle2->uvtransform = MakeIdentity44();


	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> wvpResourceTriangle2 = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* wvpDataTriangle2 = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceTriangle2->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataTriangle2));
	//単位行列を書き込んでおく
	wvpDataTriangle2->WVP = MakeIdentity44();
	wvpDataTriangle2->World = MakeIdentity44();

	Transform transformTriangle[] = {
		{{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},
		{{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}
	};

	Transform uvTransformTriangle[] = {
		{{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},
		{{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}
	};

	//立方体
	//Shaderをコンパイルする
	IDxcBlob* VSBlobCube = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(VSBlobCube != nullptr);

	IDxcBlob* PSBlobCube = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(PSBlobCube != nullptr);

	//POSを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPSDescCube{};
	graphicsPSDescCube.pRootSignature = rootSignature.Get(); //RootSignature
	graphicsPSDescCube.InputLayout = inputLayoutDesc;  //InputLayout
	graphicsPSDescCube.VS = { VSBlobCube->GetBufferPointer(),VSBlobCube->GetBufferSize() };//VerterShader
	graphicsPSDescCube.PS = { PSBlobCube->GetBufferPointer(),PSBlobCube->GetBufferSize() };//pixelShader
	graphicsPSDescCube.BlendState = blendDesc; //blendState
	graphicsPSDescCube.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPSDescCube.NumRenderTargets = 1;
	graphicsPSDescCube.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPSDescCube.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPSDescCube.SampleDesc.Count = 1;
	graphicsPSDescCube.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPSDescCube.DepthStencilState = depthStencilDesc;
	graphicsPSDescCube.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ComPtr<ID3D12PipelineState> graphicsPSCube = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPSDescCube, IID_PPV_ARGS(&graphicsPSCube));
	assert(SUCCEEDED(hr));

	ComPtr<ID3D12Resource> vertexResourceCube = CreateBufferResource(device, sizeof(VertexData) * 36);

	D3D12_VERTEX_BUFFER_VIEW vbvCube = {};
	//リソースの先頭のアドレスから使う
	vbvCube.BufferLocation = vertexResourceCube->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vbvCube.SizeInBytes = sizeof(VertexData) * 36;
	//1頂点あたりのサイズ
	vbvCube.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexDataCube = nullptr;
	//書き込むためのアドレスを取得
	vertexResourceCube->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataCube));

	
	vertexDataCube[0].position = { -1.0f,1.0f,-1.0f,1.0f };    //左上前
	vertexDataCube[0].texcoord = { 0.0f,0.0f };				   
	vertexDataCube[1].position = { 1.0f,1.0f,-1.0f,1.0f };     //右上前
	vertexDataCube[1].texcoord = { 1.0f,0.0f };				   
	vertexDataCube[2].position = { -1.0f,-1.0f,-1.0f,1.0f };   //左下前
	vertexDataCube[2].texcoord = { 0.0f,1.0f };				   
	vertexDataCube[3] = vertexDataCube[1];                     //右上前
	vertexDataCube[4].position = { 1.0f,-1.0f,-1.0f,1.0f };    //右下前
	vertexDataCube[4].texcoord = { 1.0f,1.0f };				   
	vertexDataCube[5] = vertexDataCube[2];                     //左下前
	vertexDataCube[6].position = { -1.0f,1.0f,1.0f,1.0f };     //左上奥
	vertexDataCube[6].texcoord = {0.0f,0.0f};				   
	vertexDataCube[7].position = vertexDataCube[0].position;   //左上前
	vertexDataCube[7].texcoord = { 1.0f,0.0f };				   
	vertexDataCube[8].position = { -1.0f,-1.0f,1.0f,1.0f };    //左下奥
	vertexDataCube[8].texcoord = { 0.0f,1.0f };				   
	vertexDataCube[9] = vertexDataCube[7];                     //左上前
	vertexDataCube[10].position = vertexDataCube[2].position;  //左下前
	vertexDataCube[10].texcoord = { 1.0f,1.0f };			   
	vertexDataCube[11] = vertexDataCube[8];                    //左下奥
	vertexDataCube[12].position = { 1.0f,1.0f,1.0f,1.0f };     //右上奥
	vertexDataCube[12].texcoord = { 0.0f,0.0f };			   
	vertexDataCube[13].position = vertexDataCube[6].position;  //左上奥
	vertexDataCube[13].texcoord = { 1.0f,0.0f };			   
	vertexDataCube[14].position = { 1.0f,-1.0f,1.0f,1.0f };    //右下奥
	vertexDataCube[14].texcoord = { 0.0f,1.0f };			   
	vertexDataCube[15] = vertexDataCube[13];                   //左上奥
	vertexDataCube[16].position = vertexDataCube[8].position;  //左下奥
	vertexDataCube[16].texcoord = { 1.0f,1.0f };			   
	vertexDataCube[17] = vertexDataCube[14];                   //右下奥
	vertexDataCube[18].position = vertexDataCube[1].position;  //右上前
	vertexDataCube[18].texcoord = { 0.0f,0.0f };			   
	vertexDataCube[19].position = vertexDataCube[12].position; //右上奥
	vertexDataCube[19].texcoord = { 1.0f,0.0f };
	vertexDataCube[20].position = vertexDataCube[4].position;  //右下前
	vertexDataCube[20].texcoord = { 0.0f,1.0f };
	vertexDataCube[21] = vertexDataCube[19];                   //右上奥
	vertexDataCube[22].position = vertexDataCube[14].position; //右下奥
	vertexDataCube[22].texcoord = { 1.0f,1.0f };
	vertexDataCube[23] = vertexDataCube[20];                   //右下前
	vertexDataCube[24] = vertexDataCube[0];                    //左上前
	vertexDataCube[25] = vertexDataCube[13];                   //左上奥
	vertexDataCube[26].position = vertexDataCube[18].position; //右上前
	vertexDataCube[26].texcoord = { 0.0f,1.0f };
	vertexDataCube[27] = vertexDataCube[25];                   //左上前
	vertexDataCube[28].position = vertexDataCube[19].position; //右上奥
	vertexDataCube[28].texcoord = { 1.0f,1.0f };
	vertexDataCube[29] = vertexDataCube[26];                   //右上前
	vertexDataCube[30].position = vertexDataCube[8].position;  //左下奥
	vertexDataCube[30].texcoord = { 0.0f,0.0f };
	vertexDataCube[31].position = vertexDataCube[2].position;  //左下前
	vertexDataCube[31].texcoord = { 1.0f,0.0f };
	vertexDataCube[32].position = vertexDataCube[14].position; //右下奥
	vertexDataCube[32].texcoord = { 0.0f,1.0f };
	vertexDataCube[33] = vertexDataCube[31];                   //左下前
	vertexDataCube[34].position = vertexDataCube[4].position;  //右下前
	vertexDataCube[34].texcoord = { 1.0f,1.0f };
	vertexDataCube[35] = vertexDataCube[32];                   //右下奥



	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ComPtr<ID3D12Resource> materialResourceCube = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialDataCube = nullptr;
	//書き込むためのアドレスを取得
	materialResourceCube->Map(0, nullptr, reinterpret_cast<void**>(&materialDataCube));
	materialDataCube->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataCube->enableLightnig = false;
	materialDataCube->uvtransform = MakeIdentity44();


	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> wvpResourceCube = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* wvpDataCube = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceCube->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataCube));
	//単位行列を書き込んでおく
	wvpDataCube->WVP = MakeIdentity44();
	wvpDataCube->World = MakeIdentity44();

	Transform transformCube = { {1.0f,1.0f,1.0f},{},{} };

	Transform uvTransformCube = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	////モデル読み込み
	//ModelData modelData = LoadObjFile("Resources","axis.obj");
	////頂点リソースを作る
	//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());
	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();  //リソースの先頭のアドレスから使う
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); //使用するリソースのサイズは頂点のサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);  //1頂点当たりのサイズ

	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));  //書き込むためのアドレスを取得
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());  //頂点データをリソースにコピー

	bool useMonsterBall = false;
	bool createObject[5] = {};

	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Window");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		static int item = 0;
		ImGui::Combo("Model", &item, "Triangle\0Sprite\0Sphere\0Cube\0");
		if (ImGui::Button("Create")) {
			if (item == 0) {
				if ((createObject[0] == false && createObject[1] == false) || (createObject[0] == false && createObject[1] == true)) {
					createObject[0] = true;
				}else if (createObject[0] == true) {
					createObject[1] = true;
				}
			}
			if (item == 1) {
				createObject[2] = true;
			}
			if (item == 2) {
				createObject[3] = true;
			}
			if (item == 3) {
				createObject[4] = true;
			}
		}

		//Triangle
		if (createObject[0] == true) {
			if (ImGui::CollapsingHeader("Triangle1")) {
				ImGui::ColorEdit4("Triangle1Color", &materialDataTriangle->color.x);
				ImGui::DragFloat3("Triangle1Translate", &transformTriangle[0].translate.x, 0.01f);
				ImGui::DragFloat3("Triangle1Rotate", &transformTriangle[0].rotate.x, 0.01f);
				ImGui::DragFloat3("Triangle1Scale", &transformTriangle[0].scale.x, 0.01f);
				ImGui::DragFloat2("Triangle1UVTranslate", &uvTransformTriangle[0].translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("Triangle1UVScale", &uvTransformTriangle[0].scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("Triangle1UVRotate", &uvTransformTriangle[0].rotate.z);
				if (ImGui::Button("Delete1")) {
					createObject[0] = false;
					transformTriangle[0] = {{1.0f,1.0f,1.0f},{},{}};
					uvTransformTriangle[0] = { {1.0f,1.0f,1.0f},{},{} };
					materialDataTriangle->color = { 1.0f,1.0f,1.0f,1.0f };
				}
			}
			
		}
		if (createObject[1] == true) {
			if (ImGui::CollapsingHeader("Triangle2")) {
				ImGui::ColorEdit4("Triangle2Color", &materialDataTriangle2->color.x);
				ImGui::DragFloat3("Triangle2Translate", &transformTriangle[1].translate.x, 0.01f);
				ImGui::DragFloat3("Triangle2Rotate", &transformTriangle[1].rotate.x, 0.01f);
				ImGui::DragFloat3("Triangle2Scale", &transformTriangle[1].scale.x, 0.01f);
				ImGui::DragFloat2("Triangle2UVTranslate", &uvTransformTriangle[1].translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("Triangle2UVScale", &uvTransformTriangle[1].scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("Triangle2UVRotate", &uvTransformTriangle[1].rotate.z);
				if (ImGui::Button("Delete2")) {
					createObject[1] = false;
					transformTriangle[1] = { {1.0f,1.0f,1.0f},{},{} };
					uvTransformTriangle[1] = { {1.0f,1.0f,1.0f},{},{} };
					materialDataTriangle2->color = { 1.0f,1.0f,1.0f,1.0f };
				}
			}
			
		}
		//Sprite
		if (createObject[2] == true) {
			if (ImGui::CollapsingHeader("Sprite")) {
				ImGui::ColorEdit4("SpriteColor", &materialDataSprite->color.x);
				ImGui::DragFloat3("SpriteTranslate", &transformSprite.translate.x, 1.0f);
				ImGui::DragFloat3("SpriteRotate", &transformSprite.rotate.x, 0.01f);
				ImGui::DragFloat3("SpriteScale", &transformSprite.scale.x, 0.01f);
				ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
				if (ImGui::Button("Delete3")) {
					createObject[2] = false;
					transformSprite = { {1.0f,1.0f,1.0f},{},{} };
					uvTransformSprite = { {1.0f,1.0f,1.0f},{},{} };
					materialDataSprite->color = { 1.0f,1.0f,1.0f,1.0f };
				}
			}
		}
		//Sphere
		if (createObject[3] == true) {
			if (ImGui::CollapsingHeader("Sphere")) {
				ImGui::ColorEdit4("SphereColor", &materialData->color.x);
				ImGui::DragFloat3("SphereTranslate", &transform.translate.x, 0.01f);
				ImGui::DragFloat3("SphereRotate", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("SphereScale", &transform.scale.x, 0.01f);
				ImGui::DragFloat2("SphereUVTranslate", &uvTransformSphere.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("SphereUVScale", &uvTransformSphere.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("SphereUVRotate", &uvTransformSphere.rotate.z);
				ImGui::Checkbox("useMonsterBall.intensity", &useMonsterBall);
				if (ImGui::CollapsingHeader("Light")) {
					ImGui::ColorEdit3("directionalLightData.color", &directionalLightData->color.x);
					ImGui::SliderFloat3("directionalLightData.direction", &directionalLightData->direction.x, -1.0f, 1.0f);
					ImGui::DragFloat("directionalLightData", &directionalLightData->intensity, 0.01f);
				}
				if (ImGui::Button("Delete4")) {
					createObject[3] = false;
					transform = { {1.0f,1.0f,1.0f},{},{} };
					uvTransformSphere = { {1.0f,1.0f,1.0f},{},{} };
					materialData->color = { 1.0f,1.0f,1.0f,1.0f };
				}
			}
		}
		//Cube
		if (createObject[4] == true) {
			if (ImGui::CollapsingHeader("Cube")) {
				ImGui::ColorEdit4("CubeColor", &materialDataCube->color.x);
				ImGui::DragFloat3("CubeTranslate", &transformCube.translate.x, 0.01f);
				ImGui::DragFloat3("CubeRotate", &transformCube.rotate.x, 0.01f);
				ImGui::DragFloat3("CubeScale", &transformCube.scale.x, 0.01f);
				ImGui::DragFloat2("CubeUVTranslate", &uvTransformCube.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("CubeUVScale", &uvTransformCube.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("CubeUVRotate", &uvTransformCube.rotate.z);
				if (ImGui::Button("Delete5")) {
					createObject[4] = false;
					transformCube = { {1.0f,1.0f,1.0f},{},{} };
					uvTransformCube = { {1.0f,1.0f,1.0f},{},{} };
					materialDataCube->color = { 1.0f,1.0f,1.0f,1.0f };
				}
			}
		}

		ImGui::End();




		//ゲームの処理


		directionalLightData->direction = directionalLightData->direction.Normalize();

		//transform.rotate.y += 0.01f;
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = cameraMatrix.Inverse();
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
		wvpData->WVP = worldViewProjectionMatrix;

		Matrix4x4 worldMatrixTriangle = MakeAffineMatrix(transformTriangle[0].scale, transformTriangle[0].rotate, transformTriangle[0].translate);
		Matrix4x4 worldViewProjectionMatrixTriangle = worldMatrixTriangle * viewMatrix * projectionMatrix;
		wvpDataTriangle->WVP = worldViewProjectionMatrixTriangle;

		worldMatrixTriangle = MakeAffineMatrix(transformTriangle[1].scale, transformTriangle[1].rotate, transformTriangle[1].translate);
		worldViewProjectionMatrixTriangle = worldMatrixTriangle * viewMatrix * projectionMatrix;
		wvpDataTriangle2->WVP = worldViewProjectionMatrixTriangle;

		Matrix4x4 worldMatrixCube = MakeAffineMatrix(transformCube.scale, transformCube.rotate, transformCube.translate);
		Matrix4x4 worldViewProjectionMatrixCube = worldMatrixCube * viewMatrix * projectionMatrix;
		wvpDataCube->WVP = worldViewProjectionMatrixCube;

		Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		Matrix4x4 viewMatrixSprite = MakeIdentity44();
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite * viewMatrixSprite * projectionMatrixSprite;
		transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;


		Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
		uvTransformMatrix = uvTransformMatrix * MakeRotateZMatrix(uvTransformSprite.rotate.z);
		uvTransformMatrix = uvTransformMatrix * MakeTranslateMatrix(uvTransformSprite.translate);
		materialDataSprite->uvtransform = uvTransformMatrix;

		uvTransformMatrix = MakeScaleMatrix(uvTransformSphere.scale);
		uvTransformMatrix = uvTransformMatrix * MakeRotateZMatrix(uvTransformSphere.rotate.z);
		uvTransformMatrix = uvTransformMatrix * MakeTranslateMatrix(uvTransformSphere.translate);
		materialData->uvtransform = uvTransformMatrix;

		uvTransformMatrix = MakeScaleMatrix(uvTransformTriangle[0].scale);
		uvTransformMatrix = uvTransformMatrix * MakeRotateZMatrix(uvTransformTriangle[0].rotate.z);
		uvTransformMatrix = uvTransformMatrix * MakeTranslateMatrix(uvTransformTriangle[0].translate);
		materialDataTriangle->uvtransform = uvTransformMatrix;

		uvTransformMatrix = MakeScaleMatrix(uvTransformTriangle[1].scale);
		uvTransformMatrix = uvTransformMatrix * MakeRotateZMatrix(uvTransformTriangle[1].rotate.z);
		uvTransformMatrix = uvTransformMatrix * MakeTranslateMatrix(uvTransformTriangle[1].translate);
		materialDataTriangle2->uvtransform = uvTransformMatrix;

		uvTransformMatrix = MakeScaleMatrix(uvTransformCube.scale);
		uvTransformMatrix = uvTransformMatrix * MakeRotateZMatrix(uvTransformCube.rotate.z);
		uvTransformMatrix = uvTransformMatrix * MakeTranslateMatrix(uvTransformCube.translate);
		materialDataCube->uvtransform = uvTransformMatrix;

		//ImGuiの内部コマンドを生成する
		ImGui::Render();

		//これから書き込むバックバッファのインデックスを取得
		UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
		//TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrier{};
		//今回のバリアはTransition
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//Noneにしておく
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//バリアを張る対象のリリース。現在のバックバッファに対して行う
		barrier.Transition.pResource = swapChainResource[backBufferIndex].Get();
		//遷移前（現在）のResourceState
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//遷移後のResourceState
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		//TransitionBarrierを張る
		commandList->ResourceBarrier(1, &barrier);

		//描画先のRTVを設定する
		commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
		//指定した色で画面全体をクリアする
		float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };  //RGBAの順
		commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
		//描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(1, descriptorHeaps);
		//描画先のRTVとDSVを設定する
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(dsvDescriptorHeap, descriptorDSV, 0);
		commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
		//指定した深度で画面全体をクリアする
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		commandList->RSSetViewports(1, &viewport);  //Viewportを設定
		commandList->RSSetScissorRects(1, &scissorRect);  //Scissorを設定
		//RootSignatureを設定。PSOに設定しているけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		commandList->SetPipelineState(graphicsPipelineState.Get());  //PSOを設定
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
		//形状を設定。PSOに設定しているものとはまた別。設置物を設定すると考えておけばいい
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		//wvp用のCBufferの場所の設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU[1] : textureSrvHandleGPU[0]);
		commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

		//描画！（DrawCall/ドローコール）。3頂点で1つのインスタンス。
		if (createObject[3] == true) {
			commandList->DrawInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0);
		}

		commandList->SetPipelineState(graphicsPSTriangle.Get());  //PSOを設定
		commandList->IASetVertexBuffers(0, 1, &vbvTriangle); //VBVを設定
		//形状を設定。PSOに設定しているものとはまた別。設置物を設定すると考えておけばいい
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResourceTriangle->GetGPUVirtualAddress());
		//wvp用のCBufferの場所の設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResourceTriangle->GetGPUVirtualAddress());
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[0]);
		if (createObject[0] == true) {
			commandList->DrawInstanced(3, 1, 0, 0);
		}

		commandList->SetPipelineState(graphicsPSTriangle2.Get());  //PSOを設定
		commandList->IASetVertexBuffers(0, 1, &vbvTriangle2); //VBVを設定
		//形状を設定。PSOに設定しているものとはまた別。設置物を設定すると考えておけばいい
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResourceTriangle2->GetGPUVirtualAddress());
		//wvp用のCBufferの場所の設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResourceTriangle2->GetGPUVirtualAddress());
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[0]);
		if (createObject[1] == true) {
			commandList->DrawInstanced(3, 1, 0, 0);
		}

		commandList->SetPipelineState(graphicsPSCube.Get());  //PSOを設定
		commandList->IASetVertexBuffers(0, 1, &vbvCube); //VBVを設定
		//形状を設定。PSOに設定しているものとはまた別。設置物を設定すると考えておけばいい
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResourceCube->GetGPUVirtualAddress());
		//wvp用のCBufferの場所の設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResourceCube->GetGPUVirtualAddress());
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[0]);
		if (createObject[4] == true) {
			commandList->DrawInstanced(36, 1, 0, 0);
		}
		


		//Spriteの描画。変更が必要なものだけ変更する
		commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);  //VBVを設定
		commandList->IASetIndexBuffer(&indexBufferViewSprite);  //IBVを設定
		//TransformationMatrixCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[0]);
		commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
		//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
		//commandList->DrawInstanced(6, 1, 0, 0);
		if (createObject[2] == true) {
			commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
		}



		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());



		//画面に描く処理はすべて終わり、画面に移すので、状態を遷移
		//今回はRenderTargetからPresentにする
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		//TransitionBarrierを張る
		commandList->ResourceBarrier(1, &barrier);

		//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
		hr = commandList->Close();
		assert(SUCCEEDED(hr));


		//GPUにコマンドリストの実行を行わせる
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, commandLists);
		//GPUとOSに画面の交換を行うよう通知する
		swapChain->Present(1, 0);
		//Fenceの値の更新
		fenceValue++;
		//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue->Signal(fence.Get(), fenceValue);
		//Fenceの値が指定したSignal値にたどり着いているか確認する
		//GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence->GetCompletedValue() < fenceValue) {
			//指定したSignalにたどりついてないので、たどり着くまで待つようにイベントを設定する
			fence->SetEventOnCompletion(fenceValue, fenceEvent);
			//イベントを待つ
			WaitForSingleObject(fenceEvent, INFINITE);
		}

		//次のフレーム用のコマンドリストを準備
		hr = commandAllocator->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList->Reset(commandAllocator.Get(), nullptr);
		assert(SUCCEEDED(hr));

	}

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX12_Shutdown();
	ImGui::DestroyContext();

	//解放処理
	CloseHandle(fenceEvent);
	signatureBlob->Release();

	if (errorBlob) {
		errorBlob->Release();
	}
	pixelShaderBlob->Release();
	verterShaderBlob->Release();
	VSBlobTriangle->Release();
	PSBlobTriangle->Release();
	
	win->TerminateGameWindow();

	return 0;
}

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandleer) {
	
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, Path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaserSourceBuffer;
	shaserSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaserSourceBuffer.Size = shaderSource->GetBufferSize();
	shaserSourceBuffer.Encoding = DXC_CP_UTF8;   //UTF8の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(),  //コンパイル対象のhlslファイル名
		L"-E",L"main",  //エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,  //shaderProfileの設定
		L"-Zi",L"-Qembed_debug",  //デバッグ用の情報を埋め込む
		L"-Od", //最適化を外しておく
		L"-Zpr",  //メモリレイアウトは行優先
	};
	//実際にshaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaserSourceBuffer,   //読み込んだファイル
		arguments,             //コンパイルオプション
		_countof(arguments),   //コンパイルオプションの数
		includeHandleer,       //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)  //コンパイルの結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメ絶対
		assert(false);
	}

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, Path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;

}

ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapproperties{};
	uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes; //リソースのサイズ。今回はVector4を3頂点分
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

ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	//ディスクリプタヒープの生成
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));

	return descriptorHeap;
}

DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミニマップ付きのデータを返す
	return mipImages;
}

ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata) {
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
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, //データ転送される設定
		nullptr, //Clear最適地。使わないのでnullptr
		IID_PPV_ARGS(&resource) //作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;

}


ComPtr<ID3D12Resource> UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, ComPtr<ID3D12Device> device,ComPtr<ID3D12GraphicsCommandList> commadList) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	//読み込んだデータからDirectX12用のSubresouceの配列を作成する
	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);
	//intermediateResourceに必要なサイズを計算する
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresource.size()));
	//計算したサイズでintermediateResourceを作る
	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	//データ転送をコマンドに積む
	UpdateSubresources(commadList.Get(), texture.Get (), intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());
	
	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commadList->ResourceBarrier(1, &barrier);
	return intermediateResource;

}

ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height) {

	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width; //Textureの幅
	resourceDesc.Height = height; //Textureの高さ
	resourceDesc.MipLevels = 1; //mipmapの数
	resourceDesc.DepthOrArraySize = 1; //奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; //1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //フォーマット。Resourceと合わせる

	//Resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし。
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値を書き込む状態にしておく
		&depthClearValue, //Clear最適地
		IID_PPV_ARGS(&resource) //作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	ModelData modelData; //構築するModelData
	std::vector<Vector4> positions;  //位置
	std::vector<Vector3> normals;  //法線
	std::vector<Vector2> texcoords;  //テクスチャ座標
	std::string line;  //ファイルから呼んだ1行を格納するもの

	std::ifstream file(directoryPath + "/" + filename); //ファイルと開く
	assert(file.is_open());  //とりあえず開けなかったら止める

	while (std::getline(file, line)) {

		std::string identifier;
		std::istringstream s(line);
		s >> identifier;  //先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			
			position.w = 1.0f;
			positions.push_back(position);
		}else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			
			normals.push_back(normal);
		}else if (identifier == "f") {
			//VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点への要素へのindexは「位置/UV/法線」で格納されているので、分散してindexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');  //区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのindexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//position.x *= -1.0f;
				//normal.x *= -1.0f;
				VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);
				//triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、周り順を逆にする
			//modelData.vertices.push_back(triangle[2]);
			//modelData.vertices.push_back(triangle[1]);
			//modelData.vertices.push_back(triangle[0]);
		}
	}

	return modelData;
}