#pragma once

#include <optional>

#include "IObject.h"

class BaseBlock : public IObject
{
public:
	/// <summary>
	/// ブロックの種類
	/// </summary>
	enum class Element : size_t
	{
		kNone = 0,
		kPlayer,
		kBody,
		kBlock,

		kCount,
	};

protected:

	enum Staging : size_t
	{
		kSROOT = 0,	// 待機
		kSMOVE,		// 移動
		kSSTOP,		// 停止
		kSLOAD,		// 積込
		

		kSCOUNT,	// カウント
	};

	// マップのどの場所にあるか
	Vector3 mapPosition_;
	// モデルの描画場所
	// ワールドトランスフォームにするかも
	Vector3 modelPosition_;

	Element element_;

	// 演出に使う
	// 今の演出
	Staging staging_ = Staging::kSROOT;

	// 次の演出
	std::optional<Staging> stagingRequest_ = std::nullopt;

	// 演出詰め合わせセット
	// 基底クラスで宣言しているメンバ関数ポインタには派生クラスの関数は入れられない
	// 厳密には仮想関数のアドレスを入れているので、派生先の関数とはアドレスが違うので
	// そもそも実行することが出来ない
	void (BaseBlock::*pStaging[(size_t)Staging::kSCOUNT])();

	// フレーム管理
	int32_t stagingFrame_ = 0;
	int32_t cStagingFrames_[kSCOUNT];

public:

	virtual void Initialize() override;

	virtual void Update() override;

	virtual void Draw() override = 0;

	virtual void ApplyVariables(const char* groupName);
	virtual void StorageVariables(const char* groupName);

protected:

	virtual void StagingInitialize();

	virtual void StagingRoot();
	virtual void StagingMove();
	virtual void StagingStop();
	virtual void StagingLoad();

};