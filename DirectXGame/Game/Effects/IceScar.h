#pragma once
///---------------------------------------------------------------------------------------------
// 
// 霜
// つららが着弾したときに地面に着く霜
// 
///---------------------------------------------------------------------------------------------

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "BaseScar.h"

#include <string>

//霜クラス
class IceScar : public BaseScar {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private: //静的メンバ変数
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature_;
	//パイプラインステート
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public: //静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="commandList">コマンドリスト</param>
	static void StaticInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>新しいオブジェクト</returns>
	static IceScar* Create(uint32_t textureHandle);
	/// <summary>
	/// 描画前処理
	/// </summary>
	static void preDraw();

public:

	~IceScar() override {}
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Init(uint32_t textureHandle) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// エフェクト更新
	/// </summary>
	void EffectUpdate() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// 衝突時
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(DaiEngine::Collider* other) override;
	/// <summary>
	/// 演出開始
	/// </summary>
	/// <param name="pos">中心座標</param>
	void EffectStart(const Vector3& pos) override;

private:

	//フェードアウト開始時間
	const int32_t kFadeOutStartTime_ = 60 * 2;

};

