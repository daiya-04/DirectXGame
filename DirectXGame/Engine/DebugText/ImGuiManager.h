#pragma once
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG



class WinApp;
class DirectXCommon;

class ImGuiManager{
private:

	DirectXCommon* dxCommon_ = nullptr;

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

