#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include <d3d12.h>
#include <wrl.h>
#include <numbers>

//カメラデータ
struct CBufferDataCamera {
	Matrix4x4 matView;
	Matrix4x4 matProjection;
	Vector3 cameraPos;
};

namespace DaiEngine {
	class Camera {
	public:
		//位置座標
		Vector3 translation_ = { 0.0f, 0.0f, -5.0f };
		//回転
		Vector3 rotation_ = {};

	private:

		float fovAngleY = 45.0f * std::numbers::pi_v<float> / 180.0f;
		float aspectRatio = (float)16 / (float)9;
		float nearZ = 0.1f;
		float farZ = 1000.0f;
		//ビュー行列
		Matrix4x4 matView_ = MakeIdentity44();
		//プロジェクション行列
		Matrix4x4 matProjection_ = MakeIdentity44();

		Microsoft::WRL::ComPtr<ID3D12Resource> cBuffer_;
		CBufferDataCamera* cMap_ = nullptr;

	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 行列更新
		/// </summary>
		void UpdateMatrix();
		/// <summary>
		/// ビュー行列更新
		/// </summary>
		void UpdateViewMatrix();
		/// <summary>
		/// 回転行列を使ったビュー行列
		/// </summary>
		/// <param name="rotateMat"></param>
		void UpdateViewMatrixRotate(const Matrix4x4& rotateMat);
		/// <summary>
		/// プロジェクション行列更新
		/// </summary>
		void UpdateProjectionMatrix();
		/// <summary>
		/// カメラ位置更新
		/// </summary>
		void UpdateCameraPos();

		void SetMatView(const Matrix4x4& matView) {
			matView_ = matView;
			cMap_->matView = matView;
		}
		const Matrix4x4 GetMatView() const { return matView_; }
		const Matrix4x4 GetMatProj() const { return matProjection_; }

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
}
