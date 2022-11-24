#pragma once
#include "CUnit.h"
class CPiece : public CUnit
{
public:
	void Initilize() override;
	void UnInitilize() override;

	void SetPieceNum(int world, int stage, int piece_num);
};

