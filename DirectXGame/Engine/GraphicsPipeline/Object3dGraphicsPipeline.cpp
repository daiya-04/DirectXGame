#include "Object3dGraphicsPipeline.h"
#include "RootParameters.h"
#include "Log.h"
#include <cassert>

using namespace Object3dParam;


void Object3dGraphicsPipeline::Init() {

	BaseGraphicsPipeline::Init();

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; //0から始まる
	descriptorRange[0].NumDescriptors = 1; //数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE descriptorRangeForETex[1] = {};
	descriptorRangeForETex[0].BaseShaderRegister = 1; //0から始まる
	descriptorRangeForETex[0].NumDescriptors = 1; //数は1つ
	descriptorRangeForETex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	descriptorRangeForETex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

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
	D3D12_ROOT_PARAMETER rootParameters[static_cast<size_t>(RootParameter::kParamNum)] = {};
	rootParameters[static_cast<size_t>(RootParameter::kMaterial)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[static_cast<size_t>(RootParameter::kMaterial)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   //PixelShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kMaterial)].Descriptor.ShaderRegister = 0;   //レジスタ番号0とバインド

	rootParameters[static_cast<size_t>(RootParameter::kWorldTransform)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[static_cast<size_t>(RootParameter::kWorldTransform)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   //VertexShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kWorldTransform)].Descriptor.ShaderRegister = 1;   //レジスタ番号0を使う

	rootParameters[static_cast<size_t>(RootParameter::kCamera)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[static_cast<size_t>(RootParameter::kCamera)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   //VertexShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kCamera)].Descriptor.ShaderRegister = 2;   //レジスタ番号0を使う

	rootParameters[static_cast<size_t>(RootParameter::kTexture)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(RootParameter::kTexture)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //PixelShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kTexture)].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(RootParameter::kTexture)].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	rootParameters[static_cast<size_t>(RootParameter::kEnvironmentTex)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[static_cast<size_t>(RootParameter::kEnvironmentTex)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //PixelShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kEnvironmentTex)].DescriptorTable.pDescriptorRanges = descriptorRangeForETex; //Tableの中身の配列を指定
	rootParameters[static_cast<size_t>(RootParameter::kEnvironmentTex)].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForETex); //Tableで利用する数

	rootParameters[static_cast<size_t>(RootParameter::kDirectionLight)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[static_cast<size_t>(RootParameter::kDirectionLight)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  //PixelShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kDirectionLight)].Descriptor.ShaderRegister = 3;  //レジスタ番号1を使う

	rootParameters[static_cast<size_t>(RootParameter::kPointLight)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[static_cast<size_t>(RootParameter::kPointLight)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  //PixelShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kPointLight)].Descriptor.ShaderRegister = 4;  //レジスタ番号1を使う

	rootParameters[static_cast<size_t>(RootParameter::kSpotLight)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[static_cast<size_t>(RootParameter::kSpotLight)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  //PixelShaderで使う
	rootParameters[static_cast<size_t>(RootParameter::kSpotLight)].Descriptor.ShaderRegister = 5;  //レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;   //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);  //配列の長さ

	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
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

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	ComPtr<IDxcBlob> verterShaderBlob = dxCompiler_->ShaderCompile(L"Object3d.VS.hlsl", L"vs_6_0");
	assert(verterShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = dxCompiler_->ShaderCompile(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	for (size_t index = 0; index < static_cast<size_t>(DaiEngine::BlendMode::BlendModeNum); index++) {

		D3D12_BLEND_DESC blendDesc = GetBlendDisc(static_cast<DaiEngine::BlendMode>(index));

		//POSを生成する
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
		pipelineStateDesc.pRootSignature = rootSignature_.Get(); //RootSignature
		pipelineStateDesc.InputLayout = inputLayoutDesc;  //InputLayout
		pipelineStateDesc.VS = { verterShaderBlob->GetBufferPointer(),verterShaderBlob->GetBufferSize() };//VerterShader
		pipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };//pixelShader
		pipelineStateDesc.BlendState = blendDesc; //blendState
		pipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
		//書き込むRTVの情報
		pipelineStateDesc.NumRenderTargets = 1;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//利用するトポロジ（形状）のタイプ。三角形
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//どのように画面に色を打ち込むかの設定
		pipelineStateDesc.SampleDesc.Count = 1;
		pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//DepthStencilの設定
		pipelineStateDesc.DepthStencilState = depthStencilDesc;
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//実際に生成

		hr = device_->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState_[index]));
		assert(SUCCEEDED(hr));

	}

}

void Object3dGraphicsPipeline::preDraw(DaiEngine::BlendMode blendMode) {
	BaseGraphicsPipeline::preDraw(blendMode);
}
