#pragma once
///---------------------------------------------------------------------------------------------
// 
// 地面
// 静的オブジェクト
// 
///---------------------------------------------------------------------------------------------

#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "LevelLoader.h"
#include <memory>

//地面クラス
class Ground{
private:
	//オブジェクト
	std::unique_ptr<Object3d> obj_;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init(std::shared_ptr<Model> model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera);
	/// <summary>
	/// データのセット
	/// </summary>
	/// <param name="data">オブジェクトデータ</param>
	void SetData(const LevelData::ObjectData& data);

};

