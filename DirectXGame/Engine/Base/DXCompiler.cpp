#include "DXCompiler.h"
#include "Log.h"
#include <cassert>
#include <format>

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

DXCompiler* DXCompiler::GetInstance() {
	static DXCompiler instance;

	return &instance;
}

void DXCompiler::Init() {

	//DxcCompilerを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));

}

ComPtr<IDxcBlob> DXCompiler::ShaderCompile(const std::wstring& fileName, const wchar_t* profile) {

	//コンパイルするファイルのフルパスを合成
	std::wstring filePath = kDirectoryPath_ + fileName;

	//コンパイル開始のログ
	Log(ConvertString(std::format(L"Begin CompileShader, Path:{}, profile:{}\n", filePath, profile)));

	//hlslファイルを読む
	ComPtr<IDxcBlobEncoding> shaderSource;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuff{};
	shaderSourceBuff.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuff.Size = shaderSource->GetBufferSize();
	shaderSourceBuff.Encoding = DXC_CP_UTF8; //UTF8の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(),        //コンパイル対象のhlslファイル名
		L"-E",L"main",			 //エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,			 //shaderProfileの設定
		L"-Zi",L"-Qembed_debug",  //デバッグ用の情報を埋め込む
		L"-Od",					 //最適化を外しておく
		L"-Zpr",				 //メモリレイアウトは行優先
	};

	//実際にshaderをコンパイルする
	ComPtr<IDxcResult> shaderResult;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuff,			 //読み込んだファイル
		arguments,					 //コンパイルオプション
		_countof(arguments),		 //コンパイルオプションの数
		includeHandler_.Get(),		 //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)  //コンパイルの結果
	);
	assert(SUCCEEDED(hr));

	//警告・エラーが出たらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}

	//コンパイル結果から実行用のバイナリ部分を取得
	ComPtr<IDxcBlob> shaderBlob;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, Path:{}, profile:{}\n", filePath, profile)));

	return shaderBlob;
}
