#pragma once

#include "../Math/Math.h"
#include "../../Sprite.h"
#include "../../Object3d.h"
#include "../../WorldTransform.h"
#include "../../ViewProjection.h"
#include "../../Input.h"

class IObject
{
protected:
	static Input* input_;

	static ViewProjection* viewProjection_;

	WorldTransform transformBase_;
	std::vector<WorldTransform> transforms_;
	std::vector<std::unique_ptr<Object3d>> models_;

public:

	static void StaticInitialize(Input* input, ViewProjection* vp = nullptr);
	static void StaticSetViewProjection(ViewProjection* view) { viewProjection_ = view; }


	virtual void Initialize();

	virtual void Update();

	virtual void Draw();

	// 行列の更新
	void UpdateMatrixs();
public:

	void AddWorldTransform(WorldTransform& wt, bool setParent = false);

protected:

	// 読み込んでいるすべてのモデルを描画
	void DrawAllModel();


};