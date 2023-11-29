#pragma once

#include <optional>

#include "IObject.h"
#include "../../Object3d.h"

#include "../../GlobalVariables.h"

class BaseBlock : public IObject
{
public:
	static float kBlockSize;

	/// <summary>
	/// ブロックの種類
	/// </summary>
	enum class Element : size_t
	{
		kNone = 0,
		kPlayer,
		kBody,
		kBlock,
		kHead,

		kCount,
	};

	struct StageVector
	{
		size_t x = 0, y = 0, z = 0;
	};

protected:

	// ぶっ飛ばす先
	const float kOverPosition = 100.0f;

	enum Staging : size_t
	{
		kSROOT = 0,	// 待機
		kSMOVE,		// 移動
		kSSTOP,		// 停止
		kSLOAD,		// 積込
		kSFALL,		// 落下

		kSOVER,		// 超過

		kSCOUNT,	// カウント
	};

	GlobalVariables* variables_ = nullptr;

	StageVector preMapPosition_;
	// マップのどの場所にあるか
	StageVector mapPosition_;
	// 次の位置
	// いまのところ上昇用
	StageVector nextMapPosition_;

	// ぶっ飛ばされる場所
	Vector3 overPosition_ = { 0.0f,0.0f,0.0f };

	// モデルの描画場所
	// ワールドトランスフォームにするかも
	// マップの位置からの参照
	WorldTransform modelTransform_;

	Element element_;

	// 演出に使う
	// 今の演出
	Staging staging_ = Staging::kSROOT;

	// 次の演出
	std::optional<Staging> stagingRequest_ = std::nullopt;
	std::optional<Staging> stagingNextRequest_ = std::nullopt;

	// モデル
	// 外部で生成されたものを受け取る
	Object3d* model_ = nullptr;

	// 演出詰め合わせセット
	// 基底クラスで宣言しているメンバ関数ポインタには派生クラスの関数は入れられない
	// 厳密には仮想関数のアドレスを入れているので、派生先の関数とはアドレスが違うので
	// そもそも実行することが出来ない
	//void (BaseBlock::* pStaging[(size_t)Staging::kSCOUNT])();

	// フレーム管理
	int32_t stagingFrame_ = 0;
	int32_t cStagingFrames_[kSCOUNT];

	bool isDraw_ = true;

public:

	virtual void Initialize() override;
	virtual void Initialize(const StageVector& pos);

	virtual void Update() override;

	virtual void Draw() override = 0;

	virtual void ApplyVariables(const char* groupName);
	virtual void StorageVariables(const char* groupName);

public:

	const StageVector& GetMapPosition() const { return mapPosition_; }
	Element GetElement() const { return element_; }
	void SetModel(Object3d* model) { model_ = model; }
	virtual void AddModel(Object3d* model) { model; };
	void MoveMapPosition(const StageVector& pos)
	{
		preMapPosition_ = mapPosition_;
		stagingRequest_ = kSMOVE;
		mapPosition_ = pos;
	}
	void FallMapPosition(const StageVector& pos)
	{
		preMapPosition_ = mapPosition_;
		stagingRequest_ = kSFALL;
		mapPosition_ = pos;
	}
	// 画面外にぶっ飛ばす
	void OverMapPosition(size_t direct);

	void SetMapPosition(const StageVector& pos, Staging staging)
	{
		preMapPosition_ = mapPosition_;
		stagingRequest_ = staging;
		mapPosition_ = pos;
	}
	void SetNextMapPosition(const StageVector& pos)
	{
		nextMapPosition_ = pos;
		stagingNextRequest_ = kSLOAD;
	}


	bool GetIsStaging() const
	{
		// 今待機中じゃないの？
		bool one = staging_ != kSROOT;
		// 次の行動が待機じゃないの？
		bool two = false;
		if (stagingRequest_)
		{
			two = stagingRequest_ != kSROOT;
		}
		// 予約している行動があるの？
		bool three = stagingNextRequest_.has_value();

		return one || two || three;
	}

	void SetIsDraw(bool flag) { isDraw_ = flag; }

protected:

	virtual void StagingInitialize();

	virtual void StagingRoot();
	virtual void StagingMove();
	virtual void StagingStop();
	virtual void StagingLoad();
	virtual void StagingFall();
	virtual void StagingOver();

};