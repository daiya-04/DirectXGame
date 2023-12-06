#include "ImGuiManager.h"

#ifdef _DEBUG
#include "DirectXCommon.h"
#include "WinApp.h"
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <cassert>
#endif // _DEBUG



ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;

	return &instance;
}

void ImGuiManager::Initialize([[maybe_unused]] WinApp* win, [[maybe_unused]] DirectXCommon* dxCommon){

#ifdef _DEBUG

	assert(dxCommon);
	dxCommon_ = dxCommon;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(win->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
		static_cast<int>(dxCommon_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		dxCommon_->GetSrvHeap(),
		dxCommon_->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon_->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());

	dxCommon_->IncrementSrvHeapCount();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
#endif // _DEBUG

}

void ImGuiManager::Begin(){

#ifdef _DEBUG
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG

}

void ImGuiManager::End(){

#ifdef _DEBUG
	//ImGuiの内部コマンドを生成する
	ImGui::Render();
#endif // _DEBUG

}

void ImGuiManager::Draw(){

#ifdef _DEBUG
	ID3D12GraphicsCommandList* commandList_;
	commandList_ = dxCommon_->GetCommandList();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_);
#endif // _DEBUG

}

void ImGuiManager::Finalize(){

#ifdef _DEBUG
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX12_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG
	
}
