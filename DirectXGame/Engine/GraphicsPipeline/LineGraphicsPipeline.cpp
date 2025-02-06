#include "LineGraphicsPipeline.h"
#include "Log.h"
#include <cassert>

void LineGraphicsPipeline::Init() {

	BaseGraphicsPipeline::Init();

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE range[1] = {};
	range[0].BaseShaderRegister = 0; //0から始まる
	range[0].NumDescriptors = 1; //数は1つ
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[1] = {};

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = range; //Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(range); //Tableで利用する数

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
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//無効
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//Lineなので塗りつぶさない
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//Shaderをコンパイルする
	ComPtr<IDxcBlob> verterShaderBlob = dxCompiler_->ShaderCompile(L"Line.VS.hlsl", L"vs_6_0");
	assert(verterShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = dxCompiler_->ShaderCompile(L"Line.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	//Depthの機能を有効化する
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
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
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


	primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
}


void LineGraphicsPipeline::preDraw(DaiEngine::BlendMode blendMode) {
	BaseGraphicsPipeline::preDraw(blendMode);
}

