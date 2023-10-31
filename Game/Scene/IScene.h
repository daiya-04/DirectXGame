#pragma once

#include "../Math/Math.h"

class IScene 
{
public:	

	// サブクラスシーン
	enum SCENE : int
	{
		kTITLE,	// タイトル
		//kSTAEG,	// ステージ
		//kCLEAR,	// クリア

		kCountScene,	// シーン最大数
	};

	//	シーン番号管理
	static int sSceneNo_;


public:

	// 純粋仮想関数

	// 初期化
	virtual void Initialize() = 0;

	// 更新
	virtual void Update() = 0;

	// モデルの描画
	virtual void DrawModel() = 0;
	// UI の描画
	virtual void DrawUI() = 0;


	// デストラクタ
	virtual ~IScene();

	// シーン番号の取得
	int GetSceneNo();

};