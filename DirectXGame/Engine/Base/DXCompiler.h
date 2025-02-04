#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>

namespace DaiEngine {
	class DXCompiler {
	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:

		static DXCompiler* GetInstance();

		void Init();
		//シェーダーのコンパイル
		ComPtr<IDxcBlob> ShaderCompile(const std::wstring& fileName, const wchar_t* profile);

	private:

		ComPtr<IDxcUtils> dxcUtils_;
		ComPtr<IDxcCompiler3> dxcCompiler_;
		ComPtr<IDxcIncludeHandler> includeHandler_;

		const std::wstring kDirectoryPath_ = L"Resources/shaders/";

	private:

		DXCompiler() = default;
		~DXCompiler() = default;
		DXCompiler(const DXCompiler&) = delete;
		DXCompiler& operator=(const DXCompiler&) = delete;

	};
}
