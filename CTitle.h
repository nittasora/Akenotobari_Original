#pragma once

#include "CScene.h"
#include "SpriteData.h"

class CGameObject;
class CImage;
class CButton;
class CCursor;
class CEditorManager;

class CTitle : public CScene
{
public:
	// �R���X�g���N�^
	CTitle();

	// �f�X�g���N�^
	~CTitle();

	// ����������
	// �V�[���J�n���̏�����
	void Initialize() override final;

	// �I������
	// �V�[���I�����̏�����
	void Finalize() override final;

	// �X�V����
	void Update() override final;

	// �`�揈��
	void Draw() override final;

	// �|�[�Y���X�V����
	void PauseUpdate() override final;

	// �|�[�Y���`�揈��
	void PauseDraw() override final;

	// �����蔻��
	void ColliderTest();

private:
	void CommonUpdate();

	std::vector<CGameObject*> mvp_objects;
	CCursor* mp_cursor;

	int mi_world_num;

	int mi_cursor_line;
	int mi_old_cursor_line;
	float mf_old_axis;

	std::vector<CGameObject*> mvp_back;
	std::vector<CGameObject*> mvp_chip;
	std::vector<CGameObject*> mvp_button;
	std::vector<CGameObject*> mvp_cursor;
	std::vector<CGameObject*> mvp_frame;
	CGameObject* mp_player;

	std::vector<SpriteData> mv_sprite_data;

	CEditorManager* mp_editor_manager;
};

