#include "Player.h"

void Player::Initialize()
{
	transformBase_.rotation_ = { 0.0f,0.0f,0.0f };
	transformBase_.translation_ = { 0.0f,0.0f,0.0f };
}

void Player::Update()
{

	
	UpdateMatrixs();
}

void Player::Draw()
{
	DrawAllModel();
}
