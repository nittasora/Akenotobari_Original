#pragma once

#include "CMenuScene.h"

enum PAUSE_BUTTON
{
	PB_OPTION,
	PB_RULE,
	PB_SELECT,
	PB_BACK,

	PB_MAX
};

enum PAUSE_MENU_CONDITION
{
	PAUSE_MAIN,
	PAUSE_OPTION,
	PAUSE_RULE
};

class CImage;
class CButton;
class CCursor;
class CGameObject;

class CMPause : public CMenuScene
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
	void MainUpdate();
	void OptionUpdate();
	void RuleUpdate();

	std::vector<CGameObject*> mvp_objects;
	std::vector<CGameObject*> mvp_button;
	std::vector<CGameObject*> mvp_cursor;
	std::vector<CGameObject*> mvp_frame;
	CImage* mp_back;
	CImage* mp_header;
	CImage* mp_fade;
	CCursor* mp_cursor;
	//CImage* mp_rule;

	std::vector<CGameObject*> mvp_sound_obj;
	CImage* mp_sound_bar;
	CImage* mp_sound_cursor;
	CImage* mp_sound_back;
	CImage* mp_sound_frame;

	CImage* mp_rule_back;

	int mi_cursor_line;
	int mi_old_cursor_line;
	float mf_old_axis;
	PAUSE_MENU_CONDITION m_pause_condition;
	bool mb_cursor_move;
};

