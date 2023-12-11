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

	//
	//vertexDataCube[0].position = { -1.0f,1.0f,-1.0f,1.0f };    //左上前
	//vertexDataCube[0].texcoord = { 0.0f,0.0f };				   
	//vertexDataCube[1].position = { 1.0f,1.0f,-1.0f,1.0f };     //右上前
	//vertexDataCube[1].texcoord = { 1.0f,0.0f };				   
	//vertexDataCube[2].position = { -1.0f,-1.0f,-1.0f,1.0f };   //左下前
	//vertexDataCube[2].texcoord = { 0.0f,1.0f };				   
	//vertexDataCube[3] = vertexDataCube[1];                     //右上前
	//vertexDataCube[4].position = { 1.0f,-1.0f,-1.0f,1.0f };    //右下前
	//vertexDataCube[4].texcoord = { 1.0f,1.0f };				   
	//vertexDataCube[5] = vertexDataCube[2];                     //左下前
	//vertexDataCube[6].position = { -1.0f,1.0f,1.0f,1.0f };     //左上奥
	//vertexDataCube[6].texcoord = {0.0f,0.0f};				   
	//vertexDataCube[7].position = vertexDataCube[0].position;   //左上前
	//vertexDataCube[7].texcoord = { 1.0f,0.0f };				   
	//vertexDataCube[8].position = { -1.0f,-1.0f,1.0f,1.0f };    //左下奥
	//vertexDataCube[8].texcoord = { 0.0f,1.0f };				   
	//vertexDataCube[9] = vertexDataCube[7];                     //左上前
	//vertexDataCube[10].position = vertexDataCube[2].position;  //左下前
	//vertexDataCube[10].texcoord = { 1.0f,1.0f };			   
	//vertexDataCube[11] = vertexDataCube[8];                    //左下奥
	//vertexDataCube[12].position = { 1.0f,1.0f,1.0f,1.0f };     //右上奥
	//vertexDataCube[12].texcoord = { 0.0f,0.0f };			   
	//vertexDataCube[13].position = vertexDataCube[6].position;  //左上奥
	//vertexDataCube[13].texcoord = { 1.0f,0.0f };			   
	//vertexDataCube[14].position = { 1.0f,-1.0f,1.0f,1.0f };    //右下奥
	//vertexDataCube[14].texcoord = { 0.0f,1.0f };			   
	//vertexDataCube[15] = vertexDataCube[13];                   //左上奥
	//vertexDataCube[16].position = vertexDataCube[8].position;  //左下奥
	//vertexDataCube[16].texcoord = { 1.0f,1.0f };			   
	//vertexDataCube[17] = vertexDataCube[14];                   //右下奥
	//vertexDataCube[18].position = vertexDataCube[1].position;  //右上前
	//vertexDataCube[18].texcoord = { 0.0f,0.0f };			   
	//vertexDataCube[19].position = vertexDataCube[12].position; //右上奥
	//vertexDataCube[19].texcoord = { 1.0f,0.0f };
	//vertexDataCube[20].position = vertexDataCube[4].position;  //右下前
	//vertexDataCube[20].texcoord = { 0.0f,1.0f };
	//vertexDataCube[21] = vertexDataCube[19];                   //右上奥
	//vertexDataCube[22].position = vertexDataCube[14].position; //右下奥
	//vertexDataCube[22].texcoord = { 1.0f,1.0f };
	//vertexDataCube[23] = vertexDataCube[20];                   //右下前
	//vertexDataCube[24] = vertexDataCube[0];                    //左上前
	//vertexDataCube[25] = vertexDataCube[13];                   //左上奥
	//vertexDataCube[26].position = vertexDataCube[18].position; //右上前
	//vertexDataCube[26].texcoord = { 0.0f,1.0f };
	//vertexDataCube[27] = vertexDataCube[25];                   //左上前
	//vertexDataCube[28].position = vertexDataCube[19].position; //右上奥
	//vertexDataCube[28].texcoord = { 1.0f,1.0f };
	//vertexDataCube[29] = vertexDataCube[26];                   //右上前
	//vertexDataCube[30].position = vertexDataCube[8].position;  //左下奥
	//vertexDataCube[30].texcoord = { 0.0f,0.0f };
	//vertexDataCube[31].position = vertexDataCube[2].position;  //左下前
	//vertexDataCube[31].texcoord = { 1.0f,0.0f };
	//vertexDataCube[32].position = vertexDataCube[14].position; //右下奥
	//vertexDataCube[32].texcoord = { 0.0f,1.0f };
	//vertexDataCube[33] = vertexDataCube[31];                   //左下前
	//vertexDataCube[34].position = vertexDataCube[4].position;  //右下前
	//vertexDataCube[34].texcoord = { 1.0f,1.0f };
	//vertexDataCube[35] = vertexDataCube[32];                   //右下奥

	game->Run();
	
	return 0;
}
