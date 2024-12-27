#pragma once
#include <cstdint>

class Boss;

/// <summary>
/// ボスの行動の抽象クラス
/// </summary>
class IBossBehavior {
public:

	virtual ~IBossBehavior() = 0;

	virtual void Init() = 0;

	virtual void Update() = 0;

};

inline IBossBehavior::~IBossBehavior() {}
