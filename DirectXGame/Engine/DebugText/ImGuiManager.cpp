#include "ImGuiManager.h"

#ifdef USE_IMGUI
#include "DirectXCommon.h"
#include "WinApp.h"
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <cassert>
#endif // _DEBUG

namespace DaiEngine {
	ImGuiManager* ImGuiManager::GetInstance() {
		static ImGuiManager instance;

		return &instance;
	}

	void ImGuiManager::Initialize() {

#ifdef USE_IMGUI

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHwnd());
		ImGui_ImplDX12_Init(DirectXCommon::GetInstance()->GetDevice(),
			static_cast<int>(DirectXCommon::GetInstance()->GetBackBufferCount()),
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DirectXCommon::GetInstance()->GetSrvHeap(),
			DirectXCommon::GetInstance()->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
			DirectXCommon::GetInstance()->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());

		DirectXCommon::GetInstance()->IncrementSrvHeapCount();

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#endif // _DEBUG

	}

	void ImGuiManager::Begin() {

#ifdef USE_IMGUI
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
#endif // _DEBUG

	}

	void ImGuiManager::End() {

#ifdef USE_IMGUI
		//ImGuiの内部コマンドを生成する
		ImGui::Render();
#endif // _DEBUG

	}

	void ImGuiManager::Draw() {

#ifdef USE_IMGUI
		ID3D12GraphicsCommandList* commandList_;
		commandList_ = DirectXCommon::GetInstance()->GetCommandList();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_);
#endif // _DEBUG

	}

	void ImGuiManager::Finalize() {

#ifdef USE_IMGUI
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
		ImGui::DestroyContext();
#endif // _DEBUG

	}
}
