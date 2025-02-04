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
#include "Matrix44.h"
#include "LevelLoader.h"
#include "OBBCollider.h"
#include <memory>

//地面クラス
class Ground{
private:
	//オブジェクト
	std::unique_ptr<DaiEngine::Object3d> obj_;

	std::unique_ptr<DaiEngine::OBBCollider> collider_;
	Matrix4x4 rotateMat_ = MakeIdentity44();

public:

	~Ground();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init(std::shared_ptr<DaiEngine::Model> model);
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

	void OnCollision(DaiEngine::Collider* other);

};

