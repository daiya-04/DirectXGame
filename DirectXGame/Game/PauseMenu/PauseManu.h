#pragma once
#include "ImGuiManager.h"
#include "Input.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "WorldTransform.h"

class PauseManu
{
public:
	void Init();
	void Update();
	void Draw();

	void Reset();
#pragma region
	bool GetIsRestert() { return IsRestert; }
#pragma endregion

private:
#pragma region
	std::unique_ptr<Sprite> UI_BackGraund;
	std::unique_ptr<Sprite> UI_ReStert;
	std::unique_ptr<Sprite> UI_ReturnSelect;
	std::unique_ptr<Sprite> UI_ReturnBack;
	std::unique_ptr<Sprite> UI_Arrow;
#pragma endregion スプライト
	bool IsRestert = false;
	bool IsReturnSelect = false;
	Vector2 CursorPos[2] = {
		{400.0f,284.0f},
		{400.0f,424.0f}
	};

	int Cursor = 0;
};