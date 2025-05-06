#pragma once
#include <cstdint>

class Player;

/// <summary>
/// プレイヤーの状態クラス
/// </summary>
class IPlayerState {
public:

	virtual ~IPlayerState() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void OnCollision() = 0;

};
