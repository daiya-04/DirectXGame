#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "Camera.h"

class Line{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	
	struct DataForGPU {
		Matrix4x4 wvp_;
		Vector4 color_;
	};

public:

	//共通部分の関数

	static void Init();

	static void preDraw();

	static void postDraw() {}

	static void AllDraw();

	static void Draw(const Vector3& start, const Vector3& end, const Camera& camera, const Vector4& color = {1.0f,1.0f,1.0f,1.0f});

	

private:
	
	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

private:

	static UINT drawNum_;
	static const uint32_t kMaxDrawNum_;

	static DataForGPU* datas_;
	static ComPtr<ID3D12Resource> instancingResource_;
	static D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	static D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;


private:

	Line() {};

};

