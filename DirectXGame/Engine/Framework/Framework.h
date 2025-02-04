#pragma once
///---------------------------------------------------------------------------------------------
//
// Framework
//
///---------------------------------------------------------------------------------------------


#include "WinApp.h"
#include "DirectXCommon.h"
#include "SceneManager.h"
#include <memory>

namespace DaiEngine {
	//フレームワーククラス
	class DSFramework {
	private:
		//終了リクエスト
		bool endRequest_ = false;

	public:

		virtual ~DSFramework() = default;
		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Init();
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update();
		/// <summary>
		/// 描画
		/// </summary>
		virtual void Draw() = 0;
		/// <summary>
		/// 解消
		/// </summary>
		virtual void Finalize();
		/// <summary>
		/// ゲーム全体
		/// </summary>
		void Run();
		/// <summary>
		/// 終了のリクエストが来ているか
		/// </summary>
		/// <returns>来ていたらtrue、それ以外はfalse</returns>
		virtual bool IsEndRequest() { return endRequest_; }

	};
}
