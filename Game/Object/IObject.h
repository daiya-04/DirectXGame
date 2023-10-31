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

	static ViewProjection* viewProjection_;

	WorldTransform transformBase_;
	std::vector<WorldTransform> transforms_;
	std::vector<std::unique_ptr<Object3d>> models_;

public:

	virtual void Initialize();

	virtual void Update();

	virtual void Draw();

protected:

	// 読み込んでいるすべてのモデルを描画
	void DrawAllModel();

	// 行列の更新
	void UpdateMatrixs();

};