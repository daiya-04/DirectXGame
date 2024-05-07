#include "PostEffect.h"

#include <d3dx12.h>
#include <DirectXTex.h>
#include <format>
#include <cassert>
#include <d3dcompiler.h>
#include "Log.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;

//const float PostEffect::clearColor_[4] = { 0.1f,0.25f,0.5f,0.0f };
const float PostEffect::clearColor_[4] = { 0.0f,0.0f,0.0f,0.0f };

PostEffect* PostEffect::GetInstance() {
	static PostEffect instance;

	return &instance;
}

PostEffect::PostEffect() {
	HRESULT hr;

	CreateGraphicsPipelineState();

	//テクスチャリソースの設定
	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texDesc.Width = WinApp::kClientWidth;
	texDesc.Height = WinApp::kClientHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	//利用するヒープの設定
	D3D12_HEAP_PROPERTIES srvHeapProperties{};
	//heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作成
	srvHeapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	srvHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	srvHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_CLEAR_VALUE clearColorValue{};
	clearColorValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	memcpy(clearColorValue.Color, clearColor_, sizeof(float) * 4);

	//テクスチャバッファの生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&srvHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearColorValue,
		IID_PPV_ARGS(&texBuff_)
	);
	assert(SUCCEEDED(hr));


	//画素数
	const UINT pixelCount = UINT(WinApp::kClientWidth * WinApp::kClientHeight);
	//画像1行分のデータサイズ
	const UINT rowPitch = sizeof(UINT) * WinApp::kClientWidth;
	//画像全体のデータサイズ
	const UINT depthPitch = rowPitch * WinApp::kClientHeight;
	//画像イメージ
	UINT* img = new UINT[pixelCount];
	for (int i = 0; i < pixelCount; i++) { img[i] = 0xffffffff; }

	hr = texBuff_->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
	assert(SUCCEEDED(hr));
	delete[] img;

	textureSrvHandleCPU_ = GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), DirectXCommon::GetInstance()->GetSrvHeapCount());
	textureSrvHandleGPU_ = GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), DirectXCommon::GetInstance()->GetSrvHeapCount());

	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(texBuff_.Get(), &srvDesc, textureSrvHandleCPU_);

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	rtvHandleCPU_ = GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetRtvHeap(), DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), DirectXCommon::GetInstance()->GetRtvHeapCount());
	DirectXCommon::GetInstance()->IncrementRtvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateRenderTargetView(texBuff_.Get(), &rtvDesc, rtvHandleCPU_);

	//深度バッファリソースの設定
	D3D12_RESOURCE_DESC depResourceDesc{};
	depResourceDesc.Width = WinApp::kClientWidth; //Textureの幅
	depResourceDesc.Height = WinApp::kClientHeight; //Textureの高さ
	depResourceDesc.MipLevels = 1; //mipmapの数
	depResourceDesc.DepthOrArraySize = 1; //奥行き or 配列Textureの配列数
	depResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DepthStencilとして利用可能なフォーマット
	depResourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定
	depResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2次元
	depResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES dsvHeapProperties{};
	dsvHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; //1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //フォーマット。Resourceと合わせる

	hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&dsvHeapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし。
		&depResourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値を書き込む状態にしておく
		&depthClearValue, //Clear最適地
		IID_PPV_ARGS(&depthBuff_) //作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	//DVSの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //2dTexture

	dsvHandleCPU_ = GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetDsvHeap(), DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV), DirectXCommon::GetInstance()->GetDsvHeapCount());
	DirectXCommon::GetInstance()->IncrementDsvHeapCount();

	//DSVHeapの先頭にDSVを作る
	DirectXCommon::GetInstance()->GetDevice()->CreateDepthStencilView(depthBuff_.Get(), &dsvDesc, dsvHandleCPU_);
}

void PostEffect::Init() {

	//頂点バッファ生成
	vertexBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 4);
	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	TransferVertex();

	indexBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6);

	indexBufferView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indices = nullptr;
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indices));

	indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
	indices[3] = 1;  indices[4] = 3;  indices[5] = 2;

	materialBuff_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(MaterialData));
	materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color_ = Vector4({ 1.0f,1.0f,1.0f,1.0f });

	grayScaleBuffer_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(GrayScale));
	grayScaleBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&grayScaleData_));
	grayScaleData_->isGrayScale_ = false;
	grayScaleData_->param_ = 0.0f;

}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList) {
	/*Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 640.0f,360.0f,0.0f });
	Matrix4x4 viewMatrix = MakeIdentity44();
	Matrix4x4 wvpMatrix = worldMatrix * viewMatrix * projectionMatrix_;
	Matrix4x4* wvpData = nullptr;
	*wvpData_ = wvpMatrix;*/

	if (grayScaleData_->isGrayScale_) {
		grayScaleData_->param_ += 0.01f;
		grayScaleData_->param_ = min(grayScaleData_->param_, 1.0f);
	}

	cmdList->SetGraphicsRootSignature(rootSignature_.Get());

	cmdList->SetPipelineState(piplineState_.Get());  //PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。設置物を設定すると考えておけばいい
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cmdList->IASetIndexBuffer(&indexBufferView_);

	cmdList->SetGraphicsRootConstantBufferView(0, materialBuff_->GetGPUVirtualAddress());
	//TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList, 2, textureHandle_);
	cmdList->SetGraphicsRootDescriptorTable(1, textureSrvHandleGPU_);
	cmdList->SetGraphicsRootConstantBufferView(2, grayScaleBuffer_->GetGPUVirtualAddress());

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* cmdList) {

	//リソースバリアの変更(シェーダーリソース描画可能)
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリリース。
	barrier.Transition.pResource = texBuff_.Get();
	//遷移前（現在）のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	cmdList->ResourceBarrier(1, &barrier);

	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvHandleCPU_, false, &dsvHandleCPU_);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kClientWidth;
	viewport.Height = WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	cmdList->RSSetViewports(1, &viewport);  //Viewportを設定

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kClientHeight;

	cmdList->RSSetScissorRects(1, &scissorRect);  //Scissorを設定

	//全画面クリア
	cmdList->ClearRenderTargetView(rtvHandleCPU_, clearColor_, 0, nullptr);
	//深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvHandleCPU_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInstance()->GetSrvHeap() };
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* cmdList) {

	D3D12_RESOURCE_BARRIER barrier{};
	//バリアを張る対象のリリース。現在のバックバッファに対して行う
	barrier.Transition.pResource = texBuff_.Get();
	//画面に描く処理はすべて終わり、画面に移すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	cmdList->ResourceBarrier(1, &barrier);

}

void PostEffect::TransferVertex() {

	vertexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].pos_ = { -1.0f,-1.0f,0.0f,1.0f };
	vertexData_[0].uv_ = { 0.0f,1.0f };
	//左上
	vertexData_[1].pos_ = { -1.0f,1.0f,0.0f,1.0f };
	vertexData_[1].uv_ = { 0.0f,0.0f };
	//右下
	vertexData_[2].pos_ = { 1.0f,-1.0f,0.0f,1.0f };
	vertexData_[2].uv_ = { 1.0f,1.0f };
	//右上
	vertexData_[3].pos_ = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[3].uv_ = { 1.0f,0.0f };

}

void PostEffect::CreateGraphicsPipelineState() {

	//dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
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

	//D3D12_DESCRIPTOR_RANGE descriptorRangeForBloom[1] = {};
	//descriptorRange[0].BaseShaderRegister = 1; //0から始まる
	//descriptorRange[0].NumDescriptors = 1; //数は1つ
	//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; //0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;  //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;  //ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;  //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う

	//staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //バイリニアフィルタ
	//staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; //0~1の範囲外をリピート
	//staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	//staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	//staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;  //比較しない
	//staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;  //ありったけのMipmapを使う
	//staticSamplers[1].ShaderRegister = 1;  //レジスタ番号0を使う
	//staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   //PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;   //レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   //PixelShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;   //レジスタ番号1とバインド

	//rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	//rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForBloom; //Tableの中身の配列を指定
	//rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForBloom); //Tableで利用する数

	descriptionRootSignature.pParameters = rootParameters;   //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);  //配列の長さ

	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	//ここをいじるといろいろなブレンドモードを設定できる
	//ノーマルブレンド
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	
	//α値のブレンド設定で基本的に使わないからいじらない
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	ComPtr<IDxcBlob> verterShaderBlob = CompileShader(L"Resources/shaders/PostEffect.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(verterShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/PostEffect.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//POSを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get(); //RootSignature
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

	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&piplineState_));
	assert(SUCCEEDED(hr));

}

ComPtr<ID3D12Resource> PostEffect::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
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
	ComPtr<ID3D12Resource> Resource;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}

ComPtr<IDxcBlob> PostEffect::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer) {

	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, Path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaserSourceBuffer{};
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
	ComPtr<IDxcBlob> shaderBlob;
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

D3D12_CPU_DESCRIPTOR_HANDLE PostEffect::GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE PostEffect::GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}
