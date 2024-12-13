#pragma once
///---------------------------------------------------------------------------------------------
// 
// ゲーム固有の処理のまとめ
// フレームワーククラスを継承
// 
///---------------------------------------------------------------------------------------------

#include "Framework.h"
#include <memory>
#include "PostEffect.h"

//ゲームクラス
class MyGame : public DSFramework {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init()override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;
	/// <summary>
	/// 解放
	/// </summary>
	void Finalize()override;

};

