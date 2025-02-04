#pragma once
///---------------------------------------------------------------------------------------------
// 
// 岩
// 静的オブジェクト
// 
///---------------------------------------------------------------------------------------------

#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "LevelLoader.h"

#include <memory>

//岩クラス
class Rock {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init(const std::shared_ptr<DaiEngine::Model>& model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const DaiEngine::Camera& camera);
	/// <summary>
	/// データのセット
	/// </summary>
	/// <param name="data">オブジェクトデータ</param>
	void SetData(const LevelData::ObjectData& data);

private:
	//オブジェクト
	std::unique_ptr<DaiEngine::Object3d> obj_;

};

