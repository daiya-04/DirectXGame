#pragma once
#include <cstdint>

class Player;

/// <summary>
///	プレイヤーの行動の抽象クラス
/// </summary>
class IPlayerBehavior {
public:

	virtual ~IPlayerBehavior() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

};
