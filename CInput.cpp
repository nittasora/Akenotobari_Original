#include "CInput.h"

// XInput用
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
	// キー入力関係
	//=============================

	// 前のフレームのキー状態を保存
	memcpy_s(this->sma_oldkeystate, sizeof(this->sma_oldkeystate), this->sma_keystate, sizeof(this->sma_keystate));

	// 現在のキー状態を取得
	GetKeyboardState(this->sma_keystate);

	//=============================
	// XInput入力関係
	//=============================

	// 前のフレームのボタン入力を保存
	this->m_old_controller_state = this->m_controller_state;

	// 現在のボタン入力を取得
	if (XInputGetState(0, &this->m_controller_state) != ERROR_SUCCESS) {
		// コントローラ―がエラーを起こしているので入力無し。
		ZeroMemory(&this->m_controller_state, sizeof(XINPUT_STATE));
		this->mb_xinput_connected = false;
	}
	else {
		this->mb_xinput_connected = true;
	}

	// 左アナログスティックデッドゾーン処理
	if (this->m_controller_state.Gamepad.sThumbLX > -XI_LEFT_THUMB_DEADZONE &&
		this->m_controller_state.Gamepad.sThumbLX < XI_LEFT_THUMB_DEADZONE) {
		this->m_controller_state.Gamepad.sThumbLX = 0;
	}
	if (this->m_controller_state.Gamepad.sThumbLY > -XI_LEFT_THUMB_DEADZONE &&
		this->m_controller_state.Gamepad.sThumbLY < XI_LEFT_THUMB_DEADZONE) {
		this->m_controller_state.Gamepad.sThumbLY = 0;
	}

	// 右アナログスティックデッドゾーン処理
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
	// 今 true かどうか
	return this->sma_keystate[vkey] & 0x80;
}

bool CInput::GetKeyTrigger(int vkey)
{
	// 今 true かつ 前 false かどうか
	return (this->sma_keystate[vkey] & 0x80) && !(this->sma_oldkeystate[vkey] & 0x80);
}

bool CInput::GetKeyUp(int vkey)
{
	// 今 false かつ 前 true かどうか
	return !(this->sma_keystate[vkey] & 0x80) && (this->sma_oldkeystate[vkey] & 0x80);
}

void CInput::UpdateCursorPos()
{
	// 前のフレームのカーソル位置を保存
	this->m_old_cursor_pos = this->m_cursor_pos;

	// カーソル位置取得
	GetCursorPos(&this->m_cursor_pos);
}

void CInput::UpdateClientCursorPos(HWND hwnd)
{
	// 前のフレームのカーソル位置を保存
	this->m_old_cursor_client_pos = this->m_cursor_client_pos;

	// カーソル位置取得
	GetCursorPos(&this->m_cursor_client_pos);

	// カーソル位置を変換
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
	// 今 true かどうか
	return this->m_controller_state.Gamepad.wButtons & vkey;
}

bool CInput::GetButtonTrigger(int vkey)
{
	// 今 true かつ 前 false かどうか
	return (this->m_controller_state.Gamepad.wButtons & vkey) && !(this->m_old_controller_state.Gamepad.wButtons & vkey);
}

bool CInput::GetButtonUp(int vkey)
{
	// 今 false かつ 前 true かどうか
	return !(this->m_controller_state.Gamepad.wButtons & vkey) && (this->m_old_controller_state.Gamepad.wButtons & vkey);
}

bool CInput::GetTriggerButtonPress(bool left)
{
	// 左右トリガーがデッドゾーンより押されているかどうか
	if (left) return this->m_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD;
	else return this->m_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD;
}

bool CInput::GetTriggerButtonTrigger(bool left)
{
	// 今 左右トリガーがデッドゾーンより押されている かつ 前 左右トリガーがデッドゾーンより押されていないかどうか
	if (left) return (this->m_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD) && !(this->m_old_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD);
	else return (this->m_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD) && !(this->m_old_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD);
}

bool CInput::GetTriggerButtonUp(bool left)
{
	// 今 左右トリガーがデッドゾーンより押されているないかつ 前 左右トリガーがデッドゾーンより押されているかどうか
	if (left) return !(this->m_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD) && (this->m_old_controller_state.Gamepad.bLeftTrigger > XI_TRIGGER_THRESHOLD);
	else return !(this->m_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD) && (this->m_old_controller_state.Gamepad.bRightTrigger > XI_TRIGGER_THRESHOLD);
}

bool CInput::GetTriggerButtonPress(bool left, BYTE volume)
{
	// 大きさ指定版
	if (left) return this->m_controller_state.Gamepad.bLeftTrigger > volume;
	else return this->m_controller_state.Gamepad.bRightTrigger > volume;
}

bool CInput::GetTriggerButtonTrigger(bool left, BYTE volume)
{
	// 大きさ指定版
	if (left) return (this->m_controller_state.Gamepad.bLeftTrigger > volume) && !(this->m_old_controller_state.Gamepad.bLeftTrigger > volume);
	else return (this->m_controller_state.Gamepad.bRightTrigger > volume) && !(this->m_old_controller_state.Gamepad.bRightTrigger > volume);
}

bool CInput::GetTriggerButtonUp(bool left, BYTE volume)
{
	// 大きさ指定版
	if (left) return !(this->m_controller_state.Gamepad.bLeftTrigger > volume) && (this->m_old_controller_state.Gamepad.bLeftTrigger > volume);
	else return !(this->m_controller_state.Gamepad.bRightTrigger > volume) && (this->m_old_controller_state.Gamepad.bRightTrigger > volume);
}

float CInput::GetAxisX(bool left)
{
	float result = 0.0f;	// 結果
	float value = 0.0f;		// 値

	// 左右傾き取得
	if (left) {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbLX);
	}
	else {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbRX);
	}

	// 傾きを -1 ～ 1 に近い値に変換
	if (value > 0) result = (value / static_cast<float>(SHRT_MAX));
	if (value < 0) result = -(value / static_cast<float>(SHRT_MIN));
	return result;
}

float CInput::GetAxisY(bool left)
{
	float result = 0.0f;	// 結果
	float value = 0.0f;		// 値

	// 左右傾き取得
	if (left) {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbLY);
	}
	else {
		value = static_cast<float>(this->m_controller_state.Gamepad.sThumbRY);
	}

	// 傾きを -1 ～ 1 に近い値に変換
	if (value > 0) result = (value / static_cast<float>(SHRT_MAX));
	if (value < 0) result = -(value / static_cast<float>(SHRT_MIN));
	return result;
}

float CInput::GetAxisRawX(bool left)
{
	float result = 0.0f;	// 結果
	int value = 0;		// 値

	// 左右傾き取得
	if (left) value = this->m_controller_state.Gamepad.sThumbLX;
	else value = this->m_controller_state.Gamepad.sThumbRX;

	// 傾きを 負・無・正の方向で返す
	if (value > 0) result = 1.0f;
	if (value < 0) result = -1.0f;
	return result;
}

float CInput::GetAxisRawY(bool left)
{
	float result = 0.0f;	// 結果
	int value = 0;		// 値

	// 左右傾き取得
	if (left) value = this->m_controller_state.Gamepad.sThumbLY;
	else value = this->m_controller_state.Gamepad.sThumbRY;

	// 傾きを 負・無・正の方向で返す
	if (value > 0) result = 1.0f;
	if (value < 0) result = -1.0f;
	return result;
}

XINPUT_STATE CInput::GetControllerState()
{
	// コントローラ―の状態を返す
	return this->m_controller_state;
}

bool CInput::CheckConeectedXInput()
{
	return this->mb_xinput_connected;
}
