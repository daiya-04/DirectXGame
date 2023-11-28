#pragma once

#include <vector>
#include <list>

#include "BaseBlock.h"
#include "../../Input.h"

class BlockManager;

/// <summary>
/// シングルトンパターンにしたい
/// </summary>
class MapManager final
{
public:

	template<typename Ty>
	using StageArray = std::vector<std::vector<std::vector<Ty>>>;

	struct StageData
	{
		BaseBlock::StageVector kMaxStageSize_;
		StageArray<BaseBlock::Element> array_;
	};

	// 移動方向
	enum MoveDirect : size_t
	{
		dNONE,
		dFRONT,
		dBACK,
		dRIGHT,
		dLEFT,
		dDOWN,
	};

private:

	enum class MovedResult : size_t
	{
		kFAIL,		// 動けない
		kSUCCECES,	// 止まれる
		kOVER,		// 行き過ぎ
	};

	struct InspecterMove
	{
		MovedResult result_;
		BaseBlock::StageVector start_;
		BaseBlock::StageVector end_;
	};

	struct PlayerChunk
	{
		// 一番下のブロック座標
		BaseBlock::StageVector position_;
		// 体が何個あるか : 0 ~ ...
		size_t bodyNum_;
	};

	struct DarumaChunk
	{
		// これは y 座標を考慮しない
		BaseBlock::StageVector position_;
		// 一個以上でクリアできる
		size_t bodyNum_;
	};



private:

	StageData preData_;
	StageData currentData_;

	Input* input_ = nullptr;

	// プレイヤーの位置を追従し続ける
	BaseBlock::StageVector playerPosition_;
	BlockManager* blockManager_ = nullptr;

	// 演出中でないことを確認する
	bool isStaging_ = false;

	// 動いた結果を格納する
	PlayerChunk playerChunk_;
	// 動かす予定の開始位置と終了位置のリスト
	// 動けたかどうかをフラグで判定
	std::list<InspecterMove> moveLists_;

	// ブロックを射出するモードフラグ
	// false : 移動, true : 射出
	bool isShotFlag_ = false;
	
	
	// クリアフラグ
	bool isCleared_ = false;

	// クリア状態かどうかを判定するためのもの
	// 頭の数だけ用意する
	std::vector<DarumaChunk> darumas_;

public:

	static MapManager* GetInstance();

	void Initialize();

	void Update();

	void Draw();

	void DebugGUI();

	// ステージ情報を設定する関数
	void SetStageData(const StageData& data);

private:

	MapManager() = default;
	~MapManager() = default;
	MapManager(const MapManager& obj) = delete;
	const MapManager& operator=(const MapManager& obj) = delete;

	// 落とす
	void FallFloatingBlock();
	bool ChainFall(const BaseBlock::StageVector& pos);

	// 走査によるフラグの管理のみをしたい
	// 今は変更までしてしまっている
	// 多分無理
	void GetOperate();

	// 重くなりそうなので最初の一回のみ
	void GetPlayerInfomation();
	void GetPlayerChunk();

	// 動かす為の処理をまとめる
	// バッファなどを用意して複数の状況に対応したい
	// 積み上げが起こった時、行動を予約する
	void MoveMainObject(MoveDirect direct);

	// 動く方向の確認
	void CheckMoveAction(MoveDirect direct);

	// 動いた結果を精査
	void InspecMovetAction(MoveDirect direct);

	// 結果を反映
	void ApplyMoveAction();

	// ブロックを射出させる
	void ShotSubObject(MoveDirect direct);

	// 動く方向の確認
	void CheckShotAction(MoveDirect direct);

	// 動いた結果を精査
	void InspectShotAction(MoveDirect direct);

	// 結果を反映
	void ApplyShotAction(MoveDirect direct);

	// クリア判定する前の条件をわかりやすくする
	void ReserveClear();

	// クリアしているか
	void CheckClear();

private:


	// 方向を確認する
	InspecterMove CheckDirect(BaseBlock::StageVector position, MoveDirect direct);
	// プレイヤーの移動のみ対応
	bool CheckDirectPlayer(BaseBlock::StageVector position, MoveDirect direct, BaseBlock::Element element);
	bool CheckDirectBody(BaseBlock::StageVector position, MoveDirect direct, BaseBlock::Element element, BaseBlock::StageVector limit, BaseBlock::StageVector& nextPos);

	// 場所を指定して属性変更
	void SetElement(BaseBlock::StageVector position, BaseBlock::Element element);
	const BaseBlock::Element& GetElement(BaseBlock::StageVector position);

	// 移動先に体がないことを確認する
	// 再起関数?
	// 上に別の頭があったら無理
	BaseBlock::StageVector GetOnBody(const BaseBlock::StageVector& pos);

	// 移動方向の絶対値を取得
	size_t GetMoveLength(const BaseBlock::StageVector& a, const BaseBlock::StageVector& b);
	// 体があるかを確認
	// 位置を変更する？

};