#include "Object3d.h"
#include <cassert>
#include <format>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"
#include "Log.h"
#include "DirectionalLight.h"

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

ID3D12Device* Object3d::device_ = nullptr;
ID3D12GraphicsCommandList* Object3d::commandList_ = nullptr;
ComPtr<ID3D12RootSignature> Object3d::rootSignature_;
ComPtr<ID3D12PipelineState> Object3d::graphicsPipelineState_;
PointLight* Object3d::pointLight_ = nullptr;
SpotLight* Object3d::spotLight_ = nullptr;

void Object3d::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	assert(device);
	assert(commandList);
	device_ = device;
	commandList_ = commandList;

	

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
	descriptorRange[0].NumDescriptors =	1; //数は1つ
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
	D3D12_ROOT_PARAMETER rootParameters[(size_t)RootParameter::kParamNum] = {};
	rootParameters[(size_t)RootParameter::kMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[(size_t)RootParameter::kMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   //PixelShaderで使う
	rootParameters[(size_t)RootParameter::kMaterial].Descriptor.ShaderRegister = 0;   //レジスタ番号0とバインド

	rootParameters[(size_t)RootParameter::kWorldTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[(size_t)RootParameter::kWorldTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   //VertexShaderで使う
	rootParameters[(size_t)RootParameter::kWorldTransform].Descriptor.ShaderRegister = 1;   //レジスタ番号0を使う

	rootParameters[(size_t)RootParameter::kCamera].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //CBVを使う
	rootParameters[(size_t)RootParameter::kCamera].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   //VertexShaderで使う
	rootParameters[(size_t)RootParameter::kCamera].Descriptor.ShaderRegister = 2;   //レジスタ番号0を使う

	rootParameters[(size_t)RootParameter::kTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[(size_t)RootParameter::kTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //PixelShaderで使う
	rootParameters[(size_t)RootParameter::kTexture].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
	rootParameters[(size_t)RootParameter::kTexture].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	rootParameters[(size_t)RootParameter::kDirectionLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[(size_t)RootParameter::kDirectionLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  //PixelShaderで使う
	rootParameters[(size_t)RootParameter::kDirectionLight].Descriptor.ShaderRegister = 3;  //レジスタ番号1を使う

	rootParameters[(size_t)RootParameter::kPointLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[(size_t)RootParameter::kPointLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  //PixelShaderで使う
	rootParameters[(size_t)RootParameter::kPointLight].Descriptor.ShaderRegister = 4;  //レジスタ番号1を使う

	rootParameters[(size_t)RootParameter::kSpotLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //CBVを使う
	rootParameters[(size_t)RootParameter::kSpotLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  //PixelShaderで使う
	rootParameters[(size_t)RootParameter::kSpotLight].Descriptor.ShaderRegister = 5;  //レジスタ番号1を使う

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
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	//ここをいじるといろいろなブレンドモードを設定できる
	//ノーマルブレンド
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//加算
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;*/
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
	ComPtr<IDxcBlob> verterShaderBlob = CompileShader(L"Resources/shaders/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(verterShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
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

}

Object3d* Object3d::Create(std::shared_ptr<Model> model,bool isAnimation) {
	

	Object3d* obj = new Object3d();
	obj->Initialize(model,isAnimation);

	return obj;

}

void Object3d::preDraw() {

	commandList_->SetGraphicsRootSignature(rootSignature_.Get());

	commandList_->SetPipelineState(graphicsPipelineState_.Get());  //PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。設置物を設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Object3d::postDraw() {



}

ComPtr<IDxcBlob> Object3d::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer) {

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


void Object3d::Initialize(std::shared_ptr<Model> model, bool isAnimation) {
	model_ = model;
	worldTransform_.Init();
	isAnimation_ = isAnimation;
	if (isAnimation_) {
		animation_ = Animation::LoadAnimationFile(model_->name_);
	}

}

void Object3d::Update() {

	worldTransform_.UpdateMatrix();
	worldTransform_.matWorld_ = model_->rootNode_.localMatrix_ * worldTransform_.matWorld_;

	if (isAnimation_) {
		animation_.Play(model_->rootNode_);
		worldTransform_.matWorld_ = animation_.GetLocalMatrix() * worldTransform_.matWorld_;
	}
	
	worldTransform_.Map();
}

void Object3d::Draw(const Camera& camera) {
	
	

	model_->SetVertexBuffers(commandList_);
	model_->SetGraphicsRootConstantBufferView(commandList_, (UINT)RootParameter::kMaterial);
	//wvp用のCBufferの場所の設定
	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kWorldTransform, worldTransform_.GetGPUVirtualAddress());

	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kCamera, camera.GetGPUVirtualAddress());
	
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, (UINT)RootParameter::kTexture, model_->GetUvHandle());

	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kDirectionLight, DirectionalLight::GetInstance()->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kPointLight, pointLight_->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootConstantBufferView((UINT)RootParameter::kSpotLight, spotLight_->GetGPUVirtualAddress());

	commandList_->DrawInstanced(model_->GetVertices(), 1, 0, 0);

}

