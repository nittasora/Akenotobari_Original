#pragma once

#include <Windows.h>
#include <xinput.h>
#pragma comment(lib, "xinput.lib")

//=============================================
// �L�[�֌W
//=============================================

// �L�[�̐�
constexpr size_t KEY_MAX = 256;

//=============================================
// XInput���͎擾�֌W
//=============================================

// �����{�^���������ݗp				// ���̒l
#define XI_BUTTON_UP				XINPUT_GAMEPAD_DPAD_UP
#define XI_BUTTON_DOWN				XINPUT_GAMEPAD_DPAD_DOWN
#define XI_BUTTON_LEFT				XINPUT_GAMEPAD_DPAD_LEFT
#define XI_BUTTON_RIGHT				XINPUT_GAMEPAD_DPAD_RIGHT

// START�Ȃǂ̏������ݗp			// ���̒l
#define XI_BUTTON_START				XINPUT_GAMEPAD_START
#define XI_BUTTON_BACK				XINPUT_GAMEPAD_BACK

// �������E�X�e�B�b�N��������		// ���̒l
#define XI_BUTTON_LEFT_THUMB		XINPUT_GAMEPAD_LEFT_THUMB
#define XI_BUTTON_RIGHT_THUMB		XINPUT_GAMEPAD_RIGHT_THUMB

// �������E�{�^��(not�g���K�[)		// ���̒l
#define XI_BUTTON_LEFT_SHOULDER		XINPUT_GAMEPAD_LEFT_SHOULDER
#define XI_BUTTON_RIGHT_SHOULDER	XINPUT_GAMEPAD_RIGHT_SHOULDER

// A,B�Ȃǂ̃{�^���������ݗp		// ���̒l
#define XI_BUTTON_A					XINPUT_GAMEPAD_A
#define XI_BUTTON_B					XINPUT_GAMEPAD_B
#define XI_BUTTON_X					XINPUT_GAMEPAD_X
#define XI_BUTTON_Y					XINPUT_GAMEPAD_Y

//=============================================
// XInput�ݒ�֌W
//=============================================

// ���E�̃A�i���O�X�e�B�b�N�̃f�b�h�]�[��
#define XI_LEFT_THUMB_DEADZONE		XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
#define XI_RIGHT_THUMB_DEADZONE		XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE

// ���E�g���K�[�{�^����臒l�B����ȏ�Ȃ牟���Ă��邱�Ƃɂ���
#define XI_TRIGGER_THRESHOLD		XINPUT_GAMEPAD_TRIGGER_THRESHOLD

class CInput
{
public:
	CInput();

	// �X�V����
	void Update();

	// �L�[�{�[�h�֘A
	bool GetKeyPress(int vkey);
	bool GetKeyTrigger(int vkey);
	bool GetKeyUp(int vkey);

	// �}�E�X�J�[�\���̍��W�X�V
	void UpdateCursorPos();
	void UpdateClientCursorPos(HWND hwnd);

	// �}�E�X�J�[�\���̍��W���擾
	POINT GetCursorPosition();
	POINT GetCursorClientPosition();

	// �O�}�E�X�J�[�\���̍��W���擾
	POINT GetOldCursorPosition();
	POINT GetOldCursorClientPosition();

	// �}�E�X�J�[�\���̍��W�̍����擾
	POINT GetCursorDefference();
	POINT GetCursorClientDefference();

	// XInput�֘A	->	�{�^������
	bool GetButtonPress(int vbutton);
	bool GetButtonTrigger(int vbutton);
	bool GetButtonUp(int vbutton);

	// XInput�֘A	->	�g���K�[�{�^������(true �ō��g���K�[)
	bool GetTriggerButtonPress(bool left);
	bool GetTriggerButtonTrigger(bool left);
	bool GetTriggerButtonUp(bool left);

	// XInput�֘A	->	�g���K�[�{�^������(true �ō��g���K�[) -> 臒l��n��(���鏈���ȂǗp)
	bool GetTriggerButtonPress(bool left, BYTE volume);
	bool GetTriggerButtonTrigger(bool left, BYTE volume);
	bool GetTriggerButtonUp(bool left, BYTE volume);

	// XInput�֘A	->	�A�i���O�X�e�B�b�N����(true �ō��X�e�B�b�N)
	// -> ���������͂Ȃ�ŏ� -1.0f, ���͖����� 0.0f, �E�������͂Ȃ�ő� 1.0f�Ԃ�
	// ��������ŕς��܂��B
	float GetAxisX(bool left);
	float GetAxisY(bool left);

	// XInput�֘A	->	�A�i���O�X�e�B�b�N����(true �ō��X�e�B�b�N)
	// -> ���������͂Ȃ� -1.0f, ���͖����� 0.0f, �E�������͂Ȃ� 1.0f�Ԃ�
	float GetAxisRawX(bool left);
	float GetAxisRawY(bool left);

	// �R���g���[���\�̏����擾
	XINPUT_STATE GetControllerState();

	// XInput�R���g���[���[���ڑ�����Ă��邩
	bool CheckConeectedXInput();

private:

	// �L�[�{�[�h���͂�ۑ�����ϐ�
	BYTE sma_keystate[KEY_MAX];
	BYTE sma_oldkeystate[KEY_MAX];

	// �}�E�X�J�[�\���̈ʒu
	POINT m_cursor_pos;
	POINT m_old_cursor_pos;

	// �}�E�X�J�[�\���̈ʒu
	POINT m_cursor_client_pos;
	POINT m_old_cursor_client_pos;

	// XInput��ۑ�����ϐ�
	XINPUT_STATE m_controller_state;
	XINPUT_STATE m_old_controller_state;

	// �R���g���[���[���q�����Ă��邩�ǂ���
	bool mb_xinput_connected;
};
