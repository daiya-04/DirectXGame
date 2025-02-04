#pragma once
///---------------------------------------------------------------------------------------------
//
// Line
// 線の描画
//
///---------------------------------------------------------------------------------------------

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "Camera.h"

namespace DaiEngine {
	//ラインクラス
	class Line {
	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		//GPUに送るデータ
		struct DataForGPU {
			//ワールド・ビュー・プロジェクション行列
			Matrix4x4 wvp_;
			//色
			Vector4 color_;
		};

	public:

		/// <summary>
		/// 初期化
		/// </summary>
		static void Init();
		/// <summary>
		/// 描画前
		/// </summary>
		static void preDraw();
		/// <summary>
		/// 描画後
		/// </summary>
		static void postDraw() {}
		/// <summary>
		/// 設定すべての線の描画
		/// </summary>
		static void AllDraw();
		/// <summary>
		/// 線、一本の描画設定
		/// </summary>
		/// <param name="start">開始点</param>
		/// <param name="end">終点</param>
		/// <param name="camera">カメラ</param>
		/// <param name="color">色</param>
		static void Draw(const Vector3& start, const Vector3& end, const Camera& camera, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });



	private:

		/// <summary>
		/// リソースの生成
		/// </summary>
		/// <param name="device">デバイス</param>
		/// <param name="sizeInBytes">リソースサイズ</param>
		/// <returns></returns>
		static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	private:
		//描画数
		static UINT drawNum_;
		//描画最大数
		static const uint32_t kMaxDrawNum_;
		//データ群
		static DataForGPU* datas_;
		//インスタンシングリソース
		static ComPtr<ID3D12Resource> instancingResource_;
		//srvハンドル
		static D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
		static D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;


	private:

		Line() {};

	};
}
