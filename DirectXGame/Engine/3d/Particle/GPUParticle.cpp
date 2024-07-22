#include "GPUParticle.h"

#include <cassert>
#include <format>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"
#include "DirectXCommon.h"

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;


ID3D12Device* GPUParticle::device_ = nullptr;
ID3D12GraphicsCommandList* GPUParticle::commandList_ = nullptr;
ComPtr<ID3D12RootSignature> GPUParticle::rootSignature_;
ComPtr<ID3D12PipelineState> GPUParticle::graphicsPipelineState_;

ComPtr<ID3D12RootSignature> GPUParticle::initRootSignature_;
ComPtr<ID3D12PipelineState> GPUParticle::initComputePS_;

ComPtr<ID3D12RootSignature> GPUParticle::emitRootSignature_;
ComPtr<ID3D12PipelineState> GPUParticle::emitComputePS_;

ComPtr<ID3D12RootSignature> GPUParticle::updateRootSignature_;
ComPtr<ID3D12PipelineState> GPUParticle::updateComputePS_;


void GPUParticle::StaticInit() {

	device_ = DirectXCommon::GetInstance()->GetDevice();
	commandList_ = DirectXCommon::GetInstance()->GetCommandList();

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
	descriptorRange[0].BaseShaderRegister = 1; //0から始まる
	descriptorRange[0].NumDescriptors = 1; //数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0; //0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1; //数は1つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

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
	D3D12_ROOT_PARAMETER rootParameters[(size_t)RootParameter::kParamNum] = {};

	rootParameters[(size_t)RootParameter::kParticleGPU].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[(size_t)RootParameter::kParticleGPU].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	rootParameters[(size_t)RootParameter::kParticleGPU].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; //Tableの中身の配列を指定
	rootParameters[(size_t)RootParameter::kParticleGPU].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); //Tableで利用する数

	rootParameters[(size_t)RootParameter::kPerView].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[(size_t)RootParameter::kPerView].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[(size_t)RootParameter::kPerView].Descriptor.ShaderRegister = 0;   //レジスタ番号とバインド

	rootParameters[(size_t)RootParameter::kTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[(size_t)RootParameter::kTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //PixelShaderで使う
	rootParameters[(size_t)RootParameter::kTexture].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[(size_t)RootParameter::kTexture].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

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
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
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
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;*/
	//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//減算
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;*/
	//乗算
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;*/
	//スクリーン
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;*/
	//
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
	ComPtr<IDxcBlob> verterShaderBlob = CompileShader(L"Resources/shaders/GPUParticle.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(verterShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/GPUParticle.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	ComPtr<IDxcBlob> initComputeShaderBlob = CompileShader(L"Resources/shaders/InitGPUParticle.CS.hlsl", L"cs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(initComputeShaderBlob != nullptr);

	ComPtr<IDxcBlob> emitComputeShaderBlob = CompileShader(L"Resources/shaders/EmitParticle.CS.hlsl", L"cs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(emitComputeShaderBlob != nullptr);

	ComPtr<IDxcBlob> updateComputeShaderBlob = CompileShader(L"Resources/shaders/UpdateParticle.CS.hlsl", L"cs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(updateComputeShaderBlob != nullptr);

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

	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));


	//init用のcomputeShader設定
	D3D12_DESCRIPTOR_RANGE DescRangeForParticles[1] = {};
	DescRangeForParticles[0].BaseShaderRegister = 0; //0から始まる
	DescRangeForParticles[0].NumDescriptors = 1; //数は1つ
	DescRangeForParticles[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; //SRVを使う
	DescRangeForParticles[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE DescRangeForFreeListIndex[1] = {};
	DescRangeForFreeListIndex[0].BaseShaderRegister = 1; //0から始まる
	DescRangeForFreeListIndex[0].NumDescriptors = 1; //数は1つ
	DescRangeForFreeListIndex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; //SRVを使う
	DescRangeForFreeListIndex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE DescRangeForFreeList[1] = {};
	DescRangeForFreeList[0].BaseShaderRegister = 2; //0から始まる
	DescRangeForFreeList[0].NumDescriptors = 1; //数は1つ
	DescRangeForFreeList[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; //SRVを使う
	DescRangeForFreeList[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_ROOT_SIGNATURE_DESC initParticleRootSignatureDesc{};
	initParticleRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER initParticleRootParams[(size_t)InitParticleRootParam::kParamNum] = {};

	initParticleRootParams[(size_t)InitParticleRootParam::kParticles].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	initParticleRootParams[(size_t)InitParticleRootParam::kParticles].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	initParticleRootParams[(size_t)InitParticleRootParam::kParticles].DescriptorTable.pDescriptorRanges = DescRangeForParticles; //Tableの中身の配列を指定
	initParticleRootParams[(size_t)InitParticleRootParam::kParticles].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForParticles); //Tableで利用する数

	initParticleRootParams[(size_t)InitParticleRootParam::kFreeListIndex].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	initParticleRootParams[(size_t)InitParticleRootParam::kFreeListIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	initParticleRootParams[(size_t)InitParticleRootParam::kFreeListIndex].DescriptorTable.pDescriptorRanges = DescRangeForFreeListIndex; //Tableの中身の配列を指定
	initParticleRootParams[(size_t)InitParticleRootParam::kFreeListIndex].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeListIndex); //Tableで利用する数

	initParticleRootParams[(size_t)InitParticleRootParam::kFreeList].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	initParticleRootParams[(size_t)InitParticleRootParam::kFreeList].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	initParticleRootParams[(size_t)InitParticleRootParam::kFreeList].DescriptorTable.pDescriptorRanges = DescRangeForFreeList; //Tableの中身の配列を指定
	initParticleRootParams[(size_t)InitParticleRootParam::kFreeList].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeList); //Tableで利用する数


	initParticleRootSignatureDesc.pParameters = initParticleRootParams;   //ルートパラメータ配列へのポインタ
	initParticleRootSignatureDesc.NumParameters = _countof(initParticleRootParams);  //配列の長さ


	ComPtr<ID3DBlob> initCSignatureBlob;
	ComPtr<ID3DBlob> initCErrorBlob;
	hr = D3D12SerializeRootSignature(&initParticleRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &initCSignatureBlob, &initCErrorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(initCErrorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = device_->CreateRootSignature(0, initCSignatureBlob->GetBufferPointer(), initCSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&initRootSignature_));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC initParticlePSDesc{};
	initParticlePSDesc.pRootSignature = initRootSignature_.Get();
	initParticlePSDesc.CS = { initComputeShaderBlob->GetBufferPointer(),initComputeShaderBlob->GetBufferSize() };

	hr = device_->CreateComputePipelineState(&initParticlePSDesc, IID_PPV_ARGS(&initComputePS_));
	assert(SUCCEEDED(hr));


	//Emitter用ComputeShaderの設定
	D3D12_ROOT_SIGNATURE_DESC emitterRootSignatureDesc{};
	emitterRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER emitterRootParams[(size_t)EmitterRootParam::kParamNum] = {};

	emitterRootParams[(size_t)EmitterRootParam::kParticles].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	emitterRootParams[(size_t)EmitterRootParam::kParticles].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	emitterRootParams[(size_t)EmitterRootParam::kParticles].DescriptorTable.pDescriptorRanges = DescRangeForParticles; //Tableの中身の配列を指定
	emitterRootParams[(size_t)EmitterRootParam::kParticles].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForParticles); //Tableで利用する数

	emitterRootParams[(size_t)EmitterRootParam::kEmitter].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	emitterRootParams[(size_t)EmitterRootParam::kEmitter].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	emitterRootParams[(size_t)EmitterRootParam::kEmitter].Descriptor.ShaderRegister = 0;

	emitterRootParams[(size_t)EmitterRootParam::kPerFrame].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	emitterRootParams[(size_t)EmitterRootParam::kPerFrame].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	emitterRootParams[(size_t)EmitterRootParam::kPerFrame].Descriptor.ShaderRegister = 1;

	emitterRootParams[(size_t)EmitterRootParam::kFreeListIndex].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	emitterRootParams[(size_t)EmitterRootParam::kFreeListIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	emitterRootParams[(size_t)EmitterRootParam::kFreeListIndex].DescriptorTable.pDescriptorRanges = DescRangeForFreeListIndex; //Tableの中身の配列を指定
	emitterRootParams[(size_t)EmitterRootParam::kFreeListIndex].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeListIndex); //Tableで利用する数

	emitterRootParams[(size_t)EmitterRootParam::kFreeList].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	emitterRootParams[(size_t)EmitterRootParam::kFreeList].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	emitterRootParams[(size_t)EmitterRootParam::kFreeList].DescriptorTable.pDescriptorRanges = DescRangeForFreeList; //Tableの中身の配列を指定
	emitterRootParams[(size_t)EmitterRootParam::kFreeList].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeList); //Tableで利用する数

	emitterRootSignatureDesc.pParameters = emitterRootParams;   //ルートパラメータ配列へのポインタ
	emitterRootSignatureDesc.NumParameters = _countof(emitterRootParams);  //配列の長さ

	ComPtr<ID3DBlob> emitCSignatureBlob;
	ComPtr<ID3DBlob> emitCErrorBlob;
	hr = D3D12SerializeRootSignature(&emitterRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &emitCSignatureBlob, &emitCErrorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(emitCErrorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = device_->CreateRootSignature(0, emitCSignatureBlob->GetBufferPointer(), emitCSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&emitRootSignature_));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC emitPSDesc{};
	emitPSDesc.pRootSignature = emitRootSignature_.Get();
	emitPSDesc.CS = { emitComputeShaderBlob->GetBufferPointer(),emitComputeShaderBlob->GetBufferSize() };

	hr = device_->CreateComputePipelineState(&emitPSDesc, IID_PPV_ARGS(&emitComputePS_));
	assert(SUCCEEDED(hr));

	//Update用ComputeShaderの設定
	D3D12_ROOT_SIGNATURE_DESC updateRootSignatureDesc{};
	updateRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER updateRootParams[(size_t)UpdateRootParam::kParamNum] = {};

	updateRootParams[(size_t)UpdateRootParam::kParticles].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	updateRootParams[(size_t)UpdateRootParam::kParticles].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	updateRootParams[(size_t)UpdateRootParam::kParticles].DescriptorTable.pDescriptorRanges = DescRangeForParticles; //Tableの中身の配列を指定
	updateRootParams[(size_t)UpdateRootParam::kParticles].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForParticles); //Tableで利用する数

	updateRootParams[(size_t)UpdateRootParam::kPerFrame].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	updateRootParams[(size_t)UpdateRootParam::kPerFrame].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	updateRootParams[(size_t)UpdateRootParam::kPerFrame].Descriptor.ShaderRegister = 0;

	updateRootParams[(size_t)UpdateRootParam::kFreeListIndex].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	updateRootParams[(size_t)UpdateRootParam::kFreeListIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	updateRootParams[(size_t)UpdateRootParam::kFreeListIndex].DescriptorTable.pDescriptorRanges = DescRangeForFreeListIndex; //Tableの中身の配列を指定
	updateRootParams[(size_t)UpdateRootParam::kFreeListIndex].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeListIndex); //Tableで利用する数

	updateRootParams[(size_t)UpdateRootParam::kFreeList].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	updateRootParams[(size_t)UpdateRootParam::kFreeList].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //VertexShaderで使う
	updateRootParams[(size_t)UpdateRootParam::kFreeList].DescriptorTable.pDescriptorRanges = DescRangeForFreeList; //Tableの中身の配列を指定
	updateRootParams[(size_t)UpdateRootParam::kFreeList].DescriptorTable.NumDescriptorRanges = _countof(DescRangeForFreeList); //Tableで利用する数

	updateRootSignatureDesc.pParameters = updateRootParams;   //ルートパラメータ配列へのポインタ
	updateRootSignatureDesc.NumParameters = _countof(updateRootParams);  //配列の長さ

	ComPtr<ID3DBlob> updateCSignatureBlob;
	ComPtr<ID3DBlob> updateCErrorBlob;
	hr = D3D12SerializeRootSignature(&updateRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &updateCSignatureBlob, &updateCErrorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(updateCErrorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = device_->CreateRootSignature(0, updateCSignatureBlob->GetBufferPointer(), updateCSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&updateRootSignature_));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC updatePSDesc{};
	updatePSDesc.pRootSignature = updateRootSignature_.Get();
	updatePSDesc.CS = { updateComputeShaderBlob->GetBufferPointer(),updateComputeShaderBlob->GetBufferSize() };

	hr = device_->CreateComputePipelineState(&updatePSDesc, IID_PPV_ARGS(&updateComputePS_));
	assert(SUCCEEDED(hr));


}

GPUParticle* GPUParticle::Create(uint32_t textureHandle, int32_t particleNum) {

	GPUParticle* particle = new GPUParticle();

	particle->Init(textureHandle, particleNum);

	return particle;
}

void GPUParticle::preDraw() {

	commandList_->SetGraphicsRootSignature(rootSignature_.Get());

	commandList_->SetPipelineState(graphicsPipelineState_.Get());  //PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。設置物を設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void GPUParticle::postDraw() {}

void GPUParticle::Init(uint32_t textureHandle, int32_t particleNum) {

	uvHandle_ = textureHandle;
	maxParticleNum_ = particleNum;

	CreateBuffer();

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInstance()->GetSrvHeap() };
	commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_RESOURCE_BARRIER preBarrier = {};
	preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	preBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	preBarrier.Transition.pResource = particleBuff_.Get();
	preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &preBarrier);

	commandList_->SetComputeRootSignature(initRootSignature_.Get());
	commandList_->SetPipelineState(initComputePS_.Get());

	commandList_->SetComputeRootDescriptorTable((UINT)InitParticleRootParam::kParticles, uavHandle_.second);
	commandList_->SetComputeRootDescriptorTable((UINT)InitParticleRootParam::kFreeListIndex, freeListIndexUavHandle_.second);
	commandList_->SetComputeRootDescriptorTable((UINT)InitParticleRootParam::kFreeList, freeListUavHandle_.second);

	commandList_->Dispatch(UINT(maxParticleNum_ + 1023) / 1024, 1, 1);

	D3D12_RESOURCE_BARRIER postBarrier = {};
	postBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	postBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	postBarrier.Transition.pResource = particleBuff_.Get();
	postBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	postBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	postBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &postBarrier);

}

void GPUParticle::Update() {

	perFrameData->time += (1.0f / 60.0f);

	emitterSphereData_->frequencyTime += (1.0f / 60.0f);

	if (emitterSphereData_->frequency <= emitterSphereData_->frequencyTime) {
		emitterSphereData_->frequencyTime -= emitterSphereData_->frequency;
		emitterSphereData_->emit = 1;
	}
	else {
		emitterSphereData_->emit = 0;
	}

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInstance()->GetSrvHeap() };
	commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_RESOURCE_BARRIER preBarrier = {};
	preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	preBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	preBarrier.Transition.pResource = particleBuff_.Get();
	preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &preBarrier);

	commandList_->SetComputeRootSignature(emitRootSignature_.Get());
	commandList_->SetPipelineState(emitComputePS_.Get());

	commandList_->SetComputeRootDescriptorTable((UINT)EmitterRootParam::kParticles, uavHandle_.second);
	commandList_->SetComputeRootConstantBufferView((UINT)EmitterRootParam::kEmitter, emitterBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootConstantBufferView((UINT)EmitterRootParam::kPerFrame, perFrameBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootDescriptorTable((UINT)EmitterRootParam::kFreeListIndex, freeListIndexUavHandle_.second);
	commandList_->SetComputeRootDescriptorTable((UINT)EmitterRootParam::kFreeList, freeListUavHandle_.second);

	commandList_->Dispatch(1, 1, 1);

	D3D12_RESOURCE_BARRIER connectBarrier{};
	connectBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	connectBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	connectBarrier.UAV.pResource = particleBuff_.Get();
	commandList_->ResourceBarrier(1, &connectBarrier);

	commandList_->SetComputeRootSignature(updateRootSignature_.Get());
	commandList_->SetPipelineState(updateComputePS_.Get());

	commandList_->SetComputeRootDescriptorTable((UINT)UpdateRootParam::kParticles, uavHandle_.second);
	commandList_->SetComputeRootConstantBufferView((UINT)UpdateRootParam::kPerFrame, perFrameBuff_->GetGPUVirtualAddress());
	commandList_->SetComputeRootDescriptorTable((UINT)UpdateRootParam::kFreeListIndex, freeListIndexUavHandle_.second);
	commandList_->SetComputeRootDescriptorTable((UINT)UpdateRootParam::kFreeList, freeListUavHandle_.second);

	commandList_->Dispatch(UINT(maxParticleNum_ + 1023) / 1024, 1, 1);

	D3D12_RESOURCE_BARRIER postBarrier = {};
	postBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	postBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	postBarrier.Transition.pResource = particleBuff_.Get();
	postBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	postBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	postBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &postBarrier);



}

void GPUParticle::Draw(const Camera& camera) {

	perViewData_->viewProjectionMat_ = camera.GetMatView() * camera.GetMatProj();
	perViewData_->billboardMat_ = camera.GetBillBoadMatrix();

	D3D12_RESOURCE_BARRIER preBarrier = {};
	preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	preBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	preBarrier.Transition.pResource = particleBuff_.Get();
	preBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	preBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList_->ResourceBarrier(1, &preBarrier);

	//VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vbv_);
	commandList_->IASetIndexBuffer(&ibv_);
	
	//wvp用のCBufferの場所の設定
	//commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable((UINT)RootParameter::kParticleGPU, srvHandle_.second);
	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kPerView, perViewBuff_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, (UINT)RootParameter::kTexture, uvHandle_);

	commandList_->DrawIndexedInstanced(6, maxParticleNum_, 0, 0, 0);

}

void GPUParticle::CreateBuffer() {

	vertexBuff_ = CreateBufferResource(device_, sizeof(VertexData) * 4);

	vbv_.BufferLocation = vertexBuff_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = sizeof(VertexData) * 4;
	vbv_.StrideInBytes = sizeof(VertexData);

	VertexData* vertices = nullptr;
	vertexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertices));

	//左下
	vertices[0].pos_ = { -1.0f,-1.0f,0.0f,1.0f };
	vertices[0].uv_ = { 0.0f,1.0f };
	//左上
	vertices[1].pos_ = { -1.0f,1.0f,0.0f,1.0f };
	vertices[1].uv_ = { 0.0f,0.0f };
	//右下
	vertices[2].pos_ = { 1.0f,-1.0f,0.0f,1.0f };
	vertices[2].uv_ = { 1.0f,1.0f };

	//右上
	vertices[3].pos_ = { 1.0f,1.0f,0.0f,1.0f };
	vertices[3].uv_ = { 1.0f,0.0f };
	//右下
	//vertices[5] = vertices[2];

	indexBuff_ = CreateBufferResource(device_, sizeof(uint32_t) * 6);

	ibv_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibv_.SizeInBytes = sizeof(uint32_t) * 6;
	ibv_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indices = nullptr;
	indexBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indices));

	indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
	indices[3] = 1;  indices[4] = 3;  indices[5] = 2;

	perViewBuff_ = CreateBufferResource(device_, sizeof(PerView));

	perViewBuff_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));
	perViewData_->viewProjectionMat_ = MakeIdentity44();
	perViewData_->billboardMat_ = MakeIdentity44();

	emitterBuff_ = CreateBufferResource(device_, sizeof(EmitterSphere));

	emitterBuff_->Map(0, nullptr, reinterpret_cast<void**>(&emitterSphereData_));
	emitterSphereData_->translate = Vector3(0.0f, 0.0f, 0.0f);
	emitterSphereData_->radius = 5.0f;
	emitterSphereData_->count = 5000;
	emitterSphereData_->frequency = 0.3f;
	emitterSphereData_->frequencyTime = 0.0f;
	emitterSphereData_->emit = 0;

	perFrameBuff_ = CreateBufferResource(device_, sizeof(PerFrame));
	
	perFrameBuff_->Map(0, nullptr, reinterpret_cast<void**>(&perFrameData));
	perFrameData->time = 0;
	perFrameData->deltaTime = 1.0f / 60.0f;

	CreateUav();

}

void GPUParticle::CreateUav() {

	//particleBuff_の生成
	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapproperties{};
	uploadHeapproperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeof(ParticleCS) * maxParticleNum_; //リソースのサイズ。
	//バッファの場合はこれにする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれらにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	HRESULT hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&particleBuff_));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticleNum_;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = sizeof(ParticleCS);
	UINT handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	uavHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	uavHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(particleBuff_.Get(), nullptr, &uavDesc, uavHandle_.first);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = maxParticleNum_;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleCS);
	handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	srvHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(particleBuff_.Get(), &srvDesc, srvHandle_.first);

	//freeListIndexBuff_の生成
	D3D12_HEAP_PROPERTIES freeListIndexHeapProp{};
	freeListIndexHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC freeListIndexResourceDesc{};
	freeListIndexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	freeListIndexResourceDesc.Width = sizeof(uint32_t);
	freeListIndexResourceDesc.Height = 1;
	freeListIndexResourceDesc.DepthOrArraySize = 1;
	freeListIndexResourceDesc.MipLevels = 1;
	freeListIndexResourceDesc.SampleDesc.Count = 1;
	freeListIndexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	freeListIndexResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	hr = device_->CreateCommittedResource(&freeListIndexHeapProp, D3D12_HEAP_FLAG_NONE, &freeListIndexResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListIndexBuff_));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC freeListIndexUavDesc{};
	freeListIndexUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	freeListIndexUavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	freeListIndexUavDesc.Buffer.FirstElement = 0;
	freeListIndexUavDesc.Buffer.NumElements = 1;
	freeListIndexUavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	freeListIndexUavDesc.Buffer.StructureByteStride = sizeof(uint32_t);
	handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	freeListIndexUavHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	freeListIndexUavHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(freeListIndexBuff_.Get(), nullptr, &freeListIndexUavDesc, freeListIndexUavHandle_.first);

	//freeListBuff_の生成
	D3D12_HEAP_PROPERTIES freeListHeapProp{};
	freeListHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC freeListResourceDesc{};
	freeListResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	freeListResourceDesc.Width = sizeof(uint32_t) * maxParticleNum_;
	freeListResourceDesc.Height = 1;
	freeListResourceDesc.DepthOrArraySize = 1;
	freeListResourceDesc.MipLevels = 1;
	freeListResourceDesc.SampleDesc.Count = 1;
	freeListResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	freeListResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	hr = device_->CreateCommittedResource(&freeListHeapProp, D3D12_HEAP_FLAG_NONE, &freeListResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListBuff_));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC freeListUavDesc{};
	freeListUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	freeListUavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	freeListUavDesc.Buffer.FirstElement = 0;
	freeListUavDesc.Buffer.NumElements = maxParticleNum_;
	freeListUavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	freeListUavDesc.Buffer.StructureByteStride = sizeof(uint32_t);
	handleSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	freeListUavHandle_.first = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	freeListUavHandle_.second = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSrvHeap(), handleSize, DirectXCommon::GetInstance()->GetSrvHeapCount());
	DirectXCommon::GetInstance()->IncrementSrvHeapCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(freeListBuff_.Get(), nullptr, &freeListUavDesc, freeListUavHandle_.first);


}

ComPtr<IDxcBlob> GPUParticle::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer) {

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

ComPtr<ID3D12Resource> GPUParticle::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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
	HRESULT hr = device->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}