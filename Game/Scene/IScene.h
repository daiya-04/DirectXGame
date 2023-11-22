#pragma once

#include "../Math/Math.h"
#include "../../TextureManager.h"
#include "../../ModelManager.h"
#include "../../Sprite.h"
#include "../../Object3d.h"
#include "../../WorldTransform.h"
#include "../../ViewProjection.h"
#include "../../Input.h"

class IScene
{
public:

	// サブクラスシーン
	enum SCENE : int
	{
		kTITLE,	// タイトル
		kGAME,	// ゲーム
		//kCLEAR,	// クリア

		kCountScene,	// シーン最大数
	};

	//	シーン番号管理
	static int sSceneNo_;

	static Input* input_;

public:

	static void StaticInitialize(Input* input);

	// 純粋仮想関数

	// 初期化
	virtual void Initialize() = 0;
	// シーンリセット
	virtual void Reset() = 0;

	// 更新
	virtual void Update() = 0;

	// モデルの描画
	virtual void DrawModel() = 0;
	// UI の描画
	virtual void DrawUI() = 0;

	// デバッグ表示
	virtual void DebugGUI() = 0;

	// デストラクタ
	virtual ~IScene();

	// シーン番号の取得
	int GetSceneNo();

};