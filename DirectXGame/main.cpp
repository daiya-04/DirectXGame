#include <wrl.h>
#include <memory>
#include "MyGame.h"

using namespace Microsoft::WRL;

int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int) {
	std::unique_ptr<DaiEngine::DSFramework> game = std::make_unique<MyGame>();

	game->Run();
	
	return 0;
}
