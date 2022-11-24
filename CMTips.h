#pragma once

#include "CMenuScene.h"

enum TIPS_CONDITION
{
	TC_1_1,
	TC_1_2,
	TC_MAX
};

class CImage;
class CGameObject;
class CGameManager;

class CMTips : public CMenuScene
{
public:
	// ����������
	// �V�[���J�n���̏�����
	void Initialize() override;

	// �I������
	// �V�[���I�����̏�����
	void Finalize() override;

	// �X�V����
	void Update() override;

	// �`�揈��
	void Draw() override;

	// �����蔻��
	void ColliderTest();

private:
	std::vector<CGameObject*> mvp_objects;
	std::vector<CGameObject*> mvp_tips;

	TIPS_CONDITION m_condition;
	bool mb_is_act;

	int mi_count;

	CGameManager* mp_game_manager;
};

