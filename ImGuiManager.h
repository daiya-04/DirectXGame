#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"

#include <wrl.h>


class ImGuiManager{
private:

	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

public:

	static ImGuiManager* GetInstance();

	//初期化
	void Initialize(WinApp* win,DirectXCommon* dxCommon);
	//ImGuiの受付開始
	void Begin();
	//ImGui受付終了
	void End();
	//画面への描画
	void Draw();
	//解放
	void Finalize();

private:

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

};

