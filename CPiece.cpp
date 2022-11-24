#include "CPiece.h"

void CPiece::Initilize()
{
	// �摜����
	this->AddTexture(SPRITE_PIECE_0);

	// ����^�O�̓J�P��
	this->m_tag = TAG_PIECE_1;

	// �摜�E�R���C�_�[�T�C�Y�̓K�p
	this->SetDefaultPosAndColl(PIECE_0_WIDTH, PIECE_0_HEIGHT);
}

void CPiece::UnInitilize()
{
}

void CPiece::SetPieceNum(int world, int stage, int piece_num)
{
	if (world >= PIECE_NUM_WORLD) return;
	if (world < 0) return;
	if (stage >= PIECE_NUM_STAGE) return;
	if (stage < 0) return;

	this->m_anim.v = world;

	switch (piece_num)
	{
	case 0:
		this->m_anim.u = stage * 3;
		this->m_tag = TAG_PIECE_1;
		break;
	case 1:
		this->m_anim.u = stage * 3 + 1;
		this->m_tag = TAG_PIECE_2;
		break;
	case 2:
		this->m_anim.u = stage * 3 + 2;
		this->m_tag = TAG_PIECE_3;
		break;
	default:
		this->m_anim.u = 0;
		this->m_tag = TAG_PIECE_1;
		break;
	}
}
