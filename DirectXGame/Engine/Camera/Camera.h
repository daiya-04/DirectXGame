#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include <d3d12.h>
#include <wrl.h>
#include <numbers>

struct CBufferDataCamera {
	Matrix4x4 matView;
	Matrix4x4 matProjection;
	Vector3 cameraPos;
};

class Camera {
public:

	Vector3 translation_ = { 0.0f, 0.0f, -10.0f };
	Vector3 rotation_ = {};

	

private:

	float fovAngleY = 45.0f * std::numbers::pi_v<float> / 180.0f;
	float aspectRatio = (float)16 / (float)9;
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	Matrix4x4 matView_ = MakeIdentity44();
	Matrix4x4 matProjection_ = MakeIdentity44();

	Microsoft::WRL::ComPtr<ID3D12Resource> cBuffer_;
	CBufferDataCamera* cMap_ = nullptr;

public:

	void Init();

	void CameraControl();

	void UpdateMatrix();

	void UpdateViewMatrix();

	void UpdateProjectionMatrix();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return cBuffer_->GetGPUVirtualAddress(); }

	Matrix4x4 GetBillBoadMatrix() const {
		Matrix4x4 billboardMat = matView_;
		billboardMat = billboardMat.Inverse();
		billboardMat.m[3][0] = 0.0f;
		billboardMat.m[3][1] = 0.0f;
		billboardMat.m[3][2] = 0.0f;

		return billboardMat;
	}

private:

	void CreateCBuffer();
	void Map();

};

