#include <dxgidebug.h>
#include <wrl.h>
#include <memory>
#include "MyGame.h"

#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リリースリークチェック
		ComPtr<IDXGIDebug> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int) {
	//D3DResourceLeakChecker leakCheck;
	std::unique_ptr<DSFramework> game = std::make_unique<MyGame>();

	game->Run();
	
	return 0;
}
