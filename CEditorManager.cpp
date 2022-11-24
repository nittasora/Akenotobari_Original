#include "CEditorManager.h"

#if defined(_DEBUG)

#include <array>
#include <vector>

#include "CInput.h"
#include "CTimer.h"
#include "CSceneManager.h"
#include "CFadeManager.h"

#include "CSingleton.h"

namespace {
	// �L�[��t����
	constexpr float KEY_TIME_LIMIT = 5.0f;

	// �閧�̃R�[�h
	std::array<int, 8> gai_secret_key_code = {
		'6',
		'3',
		'4',
		'7',
		'0',
		'0',
		'0',
		'1'
	};

	// ���͂����L�[�z��
	std::vector<int> gvi_key_code;

	// ���ԋL�^�p
	float gf_time;

	// ���͐����񐔋L�^�p
	size_t gui_input_count;
}

CEditorManager::CEditorManager()
{
	// �L�[�R�[�h������
	gvi_key_code.clear();

	// �^�C���̏�����
	gf_time = 0.0f;

	gui_input_count = 0;
}

CEditorManager::~CEditorManager()
{
}

void CEditorManager::Update()
{
	// �C���X�^���X�擾
	CTimer* timer = CSingleton<CTimer>::GetInstance();
	CInput* input = CSingleton<CInput>::GetInstance();
	CSceneManager* scene = CSingleton<CSceneManager>::GetInstance();

	// ���Ԃ��X�V
	gf_time += timer->GetDeltaTime();

	// �閧�̃R�[�h�Ɏg���Ă���L�[�����͂��ꂽ���e�X�g
	for (int i = 0; i < static_cast<int>(KEY_MAX); i++)
	{
		if (input->GetKeyTrigger(i)) {
			// ��t���Ԃ�������
			gf_time = 0.0f;

			// �閧�̃R�[�h�ƈ�v����Ȃ�i�߂�
			if (i == gai_secret_key_code[gui_input_count]) {
				gui_input_count++;
			}
			// ���Ȃ��Ȃ�Ύn�߂���
			else {
				gui_input_count = 0;
			}
			break;
		}
	}

	// ���͐����񐔂��閧�̃R�[�h�𒴂�����G�f�B�^�\�ֈړ�����
	if (gui_input_count >= gai_secret_key_code.size()) {
		scene->ChangeScene(SCENE_EDITOR_SELECT, FADE_TYPE_01);
	}

	// ���͎�t���Ԃ𒴂�����L�[�����Z�b�g����
	if (gf_time >= KEY_TIME_LIMIT) {
		gf_time = 0.0f;
		gui_input_count = 0;
	}
}

#else

CEditorManager::CEditorManager()
{
}

CEditorManager::~CEditorManager()
{
}

void CEditorManager::Update()
{
}

#endif