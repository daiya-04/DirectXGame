#include "ImGuiManager.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <cassert>

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;

	return &instance;
}

void ImGuiManager::Initialize(WinApp* win, DirectXCommon* dxCommon){

	assert(dxCommon);
	dxCommon_ = dxCommon;

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HRESULT hr = dxCommon_->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap_));
	assert(SUCCEEDED(hr));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(win->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
		static_cast<int>(dxCommon_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

}

void ImGuiManager::Begin(){

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void ImGuiManager::End(){

	//ImGuiの内部コマンドを生成する
	ImGui::Render();

}

void ImGuiManager::Draw(){


	ID3D12GraphicsCommandList* commandList_;
	commandList_ = dxCommon_->GetCommandList();

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_);

}

void ImGuiManager::Finalize(){

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX12_Shutdown();
	ImGui::DestroyContext();

	srvDescriptorHeap_.Reset();

}
