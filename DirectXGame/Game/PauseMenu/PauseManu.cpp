#include "PauseManu.h"

void PauseManu::Init()
{
	uint32_t UITex = TextureManager::Load("BlackTexture.png");
	UI_BackGraund = std::make_unique<Sprite>(Sprite(UITex, { 0.0f,0.0f }, { 1280.0f,720.0f }, 0.0f, { 0.0f,0.0f }, { 0.2f,0.2f,0.2f,0.5f }));
	UI_BackGraund->Initialize();
	
	UITex = TextureManager::Load("ReStart.png");
	UI_ReStert = std::make_unique<Sprite>(Sprite(UITex, { 640.0f,290.0f }, { 256.0f,72.0f }, 0.0f, { 0.5f,0.5f }, { 0.4f,0.4f,0.4f,1.0f }));
	UI_ReStert->Initialize();

	UITex = TextureManager::Load("select.png");
	UI_ReturnSelect = std::make_unique<Sprite>(Sprite(UITex, { 640.0f,420.0f }, { 384.0f,72.0f }, 0.0f, { 0.5f,0.5f }, { 0.4f,0.4f,0.4f,1.0f }));
	UI_ReturnSelect->Initialize();

	UITex = TextureManager::Load("back.png");
	UI_ReturnBack = std::make_unique<Sprite>(Sprite(UITex, { 640.0f,500.0f }, { 256.0f,72.0f }, 0.0f, { 0.5f,0.5f }, { 0.4f,0.4f,0.4f,1.0f }));
	UI_ReturnBack->Initialize();
	
	UITex = TextureManager::Load("Arrow.png");
	UI_Arrow = std::make_unique<Sprite>(Sprite(UITex, { 640.0f,500.0f }, { 64.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 0.4f,0.4f,0.4f,1.0f }));
	UI_Arrow->Initialize();
}

void PauseManu::Update()
{
	if (Input::GetInstance()->GetMoveXZ().z > 0) {
		Cursor = 0;
	}
	else if (Input::GetInstance()->GetMoveXZ().z < 0) {
		Cursor = 1;
	}
#ifdef _DEBUG
	ImGui::Begin("Pause");
	ImGui::InputInt("Cursor",&Cursor);
	ImGui::End();
#endif
	IsRestert = false;
	IsReturnSelect = false;
	UI_Arrow->SetPosition(CursorPos[Cursor]);
	if (Cursor == 0) {
		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {
			IsRestert = true;
		}
	}
	else if (Cursor == 1) {
		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {
			IsReturnSelect = true;
		}
	}
}

void PauseManu::Draw()
{
	UI_BackGraund->Draw();
	UI_ReStert->Draw();
	UI_ReturnSelect->Draw();
	UI_ReturnBack->Draw();
	UI_Arrow->Draw();
}

void PauseManu::Reset()
{
	UI_Arrow->SetPosition(CursorPos[0]);
	Cursor = 0;
}


