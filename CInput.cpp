#include "CInput.h"

// XInput�p
#define PJX_LXPOS				0x0200
#define PJX_LYPOS				0x0201
#define PJX_LTRG				0x0202
#define PJX_RXPOS				0x0203
#define PJX_RYPOS				0x0204
#define PJX_RTRG				0x0205
#define PJX_BTNS				0x0206

CInput::CInput()
{
	ZeroMemory(&this->m_controller_state, sizeof(XINPUT_STATE));
	ZeroMemory(&this->m_controller_state, sizeof(XINPUT_STATE));

	this->mb_xinput_connected = false;
}

void CInput::Update()
{
	//=============================
	// �L�[���͊֌W
	//=============================

	// �O�̃t���[���̃L�[��Ԃ�ۑ�
	memcpy_s(this->sma_oldkeystate, sizeof(this->sma_oldkeystate), this->sma_keystate, sizeof(this->sma_keystate));

	// ���݂̃L�[��Ԃ��擾
	GetKeyboardState(this->sma_keystate);

	//=============================
	// XInput���͊֌W
	//=============================

	// �O�̃t���[���̃{�^�����͂�ۑ�
	this->m_old_controller_state = this->m_controller_state;

	// ���݂̃{�^�����͂��擾
	if (XInputGetState(0, &this->m_controller_state) != ERROR_SUCCESS) {
		// �R���g���[���\���G���[���N�����Ă���̂œ��͖����B
		ZeroMemory(&this->m_controller_state, sizeof(XINPUT_STATE));
		this->mb_xinput_connected = false;
	}
	else {
		this->mb_xinput_connected = true;
	}

	// ���A�i���O�X�e�B�b�N�f�b�h�]�[������
	if (this->m_controller_state.Gamepad.sThumbLX > -XI_LEFT_THUMB_DEADZONE &&
		this->m_controller_state.Gamepad.sThumbLX < XI_LEFT_THUMB_DEADZONE) {
		this->m_controller_state.Gamepad.sThumbLX = 0;
	}
	if (this->m_controller_state.Gamepad.sThumbLY > -XI_LEFT_THUMB_DEADZONE &&
		this->m_controller_state.Gamepad.sThumbLY < XI_LEFT_THUMB_DEADZONE) {
		this->m_controller_state.Gamepad.sThumbLY = 0;
	}

	// �E�A�i���O�X�e�B�b�N�f�b�h�]�[������
	if (this->m_controller_state.Gamepad.sThumbRX > -XI_RIGHT_THUMB_DEADZONE &&
		this->m_controller_state.Gamepad.sThumbRX < XI_RIGHT_THUMB_DEADZONE) {
		this->m_controller_state.Gamepad.sThumbRX = 0;
	}
	if (this->m_controller_state.Gamepad.sThumbRY > -XI_RIGHT_THUMB_DEADZONE &&
		this->m_controller_state.Gamepad.sThumbRY < XI_RIGHT_THUMB_DEADZONE) {
		this->m_controller_state.Gamepad.sThumbRY = 0;
	}
}

bool CInput::GetKeyPress(int vkey)
{
	// �� true ���ǂ���
	return this->sma_keystate[vkey] & 0x80;
}

bool CInput::GetKeyTrigger(int vkey)
{
	// �� true ���� �O false ���ǂ���
	return (this->sma_keystate[vkey] & 0x80) && !(this->sma_oldkeystate[vkey] & 0x80);
}

bool CInput::GetKeyUp(int vkey)
{
	// �� false ���� �O true ���ǂ���
	return !(this->sma_keystate[vkey] & 0x80) && (this->sma_oldkeystate[vkey] & 0x80);
}

void CInput::UpdateCursorPos()
{
	// �O�̃t���[���̃J�[�\���ʒu��ۑ�
	this->m_old_cursor_pos = this->m_cursor_pos;

	// �J�[�\���ʒu�擾
	GetCursorPos(&this->m_cursor_pos);
}

void CInput::UpdateClientCursorPos(HWND hwnd)
{
	// �O�̃t���[���̃J�[�\���ʒu��ۑ�
	this->m_old_cursor_client_pos = this->m_cursor_client_pos;

	// �J�[�\���ʒu�擾
	GetCursorPos(&this->m_cursor_client_pos);

	// �J�[�\���ʒu��ϊ�
	ScreenToClient(hwnd, &this->m_cursor_client_pos);
}

POINT CInput::GetCursorPosition()
{
	return this->m_cursor_pos;
}

POINT CInput::GetCursorClientPosition()
{
	return this->m_cursor_client_pos;
}

POINT CInput::GetOldCursorPosition()
{
	return this->m_old_cursor_pos;
}

POINT CInput::GetOldCursorClientPosition()
{
	return this->m_old_cursor_client_pos;
}

POINT CInput::GetCursorDefference()
{
	POINT deff;
	deff.x = this->m_old_cursor_pos.x - this->m_cursor_pos.x;
	deff.y = this->m_old_cursor_pos.y - this->m_cursor_pos.y;
	return deff;
}

POINT CInput::GetCursorClientDefference()
{
	POINT deff;
	deff.x = this->m_old_cursor_client_pos.x - this->m_cursor_client_pos.x;
	deff.y = this->m_old_cursor_client_pos.y - this->m_cursor_client_pos.y;
	return deff;
}

bool CInput::GetButtonPress(int vkey)
{
	// �� true ���ǂ���
	return this->m_controller_state.Gamepad.wButtons & vkey;
}

bool CInput::GetButtonTrigger(int vkey)
{
	// �� true ���� �O false ���ǂ���
	return (this->m_controller_state.Gamepad.wButtons & vkey) && !(this->m_old_controller_state.Gamepad.wButtons & vkey);
}

bool CInput::GetButtonUp(int vkey)
{
	// �� false ���� �O true ���ǂ���
	return !(this->m_controller_state.Gamepad.wButtons & vkey) && (this->m_old_controller_state.Gamepad.wButtons & vkey);
}

bool CInput::GetTriggerButtonPress(bool left)
{
	// ���E�g���K�[���f�b�h�]�[����艟����Ă��邩�ǂ���
	if (left) return this->m_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD;
	else return this->m_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD;
}

bool CInput::GetTriggerButtonTrigger(bool left)
{
	// �� ���E�g���K�[���f�b�h�]�[����艟����Ă��� ���� �O ���E�g���K�[���f�b�h�]�[����艟����Ă��Ȃ����ǂ���
	if (left) return (this->m_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD) && !(this->m_old_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD);
	else return (this->m_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD) && !(this->m_old_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD);
}

bool CInput::GetTriggerButtonUp(bool left)
{
	// �� ���E�g���K�[���f�b�h�]�[����艟����Ă���Ȃ����� �O ���E�g���K�[���f�b�h�]�[����艟����Ă��邩�ǂ���
	if (left) return !(this->m_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD) && (this->m_old_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD);
	else return !(this->m_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD) && (this->m_old_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD);
}

bool CInput::GetTriggerButtonPress(bool left, BYTE volume)
{
	// �傫���w���
	if (left) return this->m_controller_state.Gamepad.bLeftTrigger > volume;
	else return this->m_controller_state.Gamepad.bRightTrigger > volume;
}

bool CInput::GetTriggerButtonTrigger(bool left, BYTE volume)
{
	// �傫���w���
	if (left) return (this->m_controller_state.Gamepad.bLeftTrigger > volume) && !(this->m_old_controller_state.Gamepad.bLeftTrigger > volume);
	else return (this->m_controller_state.Gamepad.bRightTrigger > volume) && !(this->m_old_controller_state.Gamepad.bRightTrigger > volume);
}

bool CInput::GetTriggerButtonUp(bool left, BYTE volume)
{
	// �傫���w���
	if (left) return !(this->m_controller_state.Gamepad.bLeftTrigger > volume) && (this->m_old_controller_state.Gamepad.bLeftTrigger > volume);
	else return !(this->m_controller_state.Gamepad.bRightTrigger > volume) && (this->m_old_controller_state.Gamepad.bRightTrigger > volume);
}

float CInput::GetAxisX(bool left)
{
	float result = 0.0f;	// ����
	float value = 0.0f;		// �l

	// ���E�X���擾
	if (left) {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbLX);
	}
	else {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbRX);
	}

	// �X���� -1 �` 1 �ɋ߂��l�ɕϊ�
	if (value > 0) result = (value / static_cast<float>(SHRT_MAX));
	if (value < 0) result = -(value / static_cast<float>(SHRT_MIN));
	return result;
}

float CInput::GetAxisY(bool left)
{
	float result = 0.0f;	// ����
	float value = 0.0f;		// �l

	// ���E�X���擾
	if (left) {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbLY);
	}
	else {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbRY);
	}

	// �X���� -1 �` 1 �ɋ߂��l�ɕϊ�
	if (value > 0) result = (value / static_cast<float>(SHRT_MAX));
	if (value < 0) result = -(value / static_cast<float>(SHRT_MIN));
	return result;
}

float CInput::GetAxisRawX(bool left)
{
	float result = 0.0f;	// ����
	int value = 0;		// �l

	// ���E�X���擾
	if (left) value = this->m_controller_state.Gamepad.sThumbLX;
	else value = this->m_controller_state.Gamepad.sThumbRX;

	// �X���� ���E���E���̕����ŕԂ�
	if (value > 0) result = 1.0f;
	if (value < 0) result = -1.0f;
	return result;
}

float CInput::GetAxisRawY(bool left)
{
	float result = 0.0f;	// ����
	int value = 0;		// �l

	// ���E�X���擾
	if (left) value = this->m_controller_state.Gamepad.sThumbLY;
	else value = this->m_controller_state.Gamepad.sThumbRY;

	// �X���� ���E���E���̕����ŕԂ�
	if (value > 0) result = 1.0f;
	if (value < 0) result = -1.0f;
	return result;
}

XINPUT_STATE CInput::GetControllerState()
{
	// �R���g���[���\�̏�Ԃ�Ԃ�
	return this->m_controller_state;
}

bool CInput::CheckConeectedXInput()
{
	return this->mb_xinput_connected;
}
