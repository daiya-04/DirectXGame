#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SceneManager.h"
#include <memory>

class DSFramework{
private:

	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;

	bool endRequest_ = false;

public:

	virtual ~DSFramework() = default;

	virtual void Init();

	virtual void Update();

	virtual void Draw() = 0;

	virtual void Finalize();

	void Run();

	virtual bool IsEndRequest() { return endRequest_; }

};

