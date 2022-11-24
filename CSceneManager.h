#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// �Q�[���V�[���̊Ǘ�
// �X���b�h�Z�[�t����Ȃ��̂Œ��ӂ��Ă�������

#include <array>
#include <thread>
#include <Windows.h>
#include "SceneData.h"
#include "CScene.h"
#include "FadeData.h"

// �X���b�h�̏��
enum TreadCondition
{
	THREAD_UN_USE,
	THREAD_USE,
	THREAD_WAIT
};

class CDirect3D;
class CFadeManager;
class CMenuManager;
class CGameManager;

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();

	//=============================
	// �����֌W
	//=============================

	// ����������
	void Initialize();

	// �I������
	void Finalize();

	// �X�V����
	void Update();

	// �V�[���؂�ւ�(���������ꍇ�����ҏ���) -> ���s�� false
	bool ChangeScene(SCENE_INDEX next_sceneindex);
	bool ChangeScene(SCENE_INDEX next_sceneindex, FADE_TYPE type);

	// ���[�v���I������t���O�𗧂Ă�
	void FinishLoop();

	// ���[�v�������Ă��邩��Ԃ�
	bool CheckLoopContinue();

	// �X�V���������ւ��܂�
	void ChangeUpdate(bool is_normal);

	// �������E�I��
	void SceneInitializer(SCENE_INDEX index);
	void SceneUnInitializer(SCENE_INDEX index);

private:
	//=============================
	// �V�[���Ǘ��p
	//=============================

	// ���݂̃V�[���C���f�b�N�X
	SCENE_INDEX m_current_scene_index;

	// ���̃V�[���C���f�b�N�X
	SCENE_INDEX m_next_scene_index;

	// �O�̃V�[���C���f�b�N�X
	SCENE_INDEX m_old_scene_index;

	// ���[�v�𑱂��邩�ǂ����̃t���O
	bool mb_loop_continue;

	// �������p�X���b�h
	struct
	{
		std::thread thread;
		TreadCondition condition;
	} m_initializer;

	// ����p�X���b�h
	struct
	{
		std::thread thread;
		TreadCondition condition;
	} m_un_initializer;

	// �E�B���h�E�h��Ԃ��悤�J���[
	FLOAT mfa_clear_color[4];

	// �֐��|�C���^�ɕʖ���t����
	using SceneFunc = void(CScene::*)();

	// �X�V�E�`��
	SceneFunc mp_update;
	SceneFunc mp_draw;

	// �V�[������ւ��p
	CScene* mp_next_scene;
	CScene* mp_current_scene;
	CScene* mp_old_scene;

	// �e��}�l�[�W���[
	CDirect3D* direct3d;
	CFadeManager* fade_manager;
	CMenuManager* menu_manager;
	CGameManager* game_manager;
};

