#pragma once

#include <Windows.h>
#include <xinput.h>
#pragma comment(lib, "xinput.lib")

//=============================================
// キー関係
//=============================================

// キーの数
constexpr size_t KEY_MAX = 256;

//=============================================
// XInput入力取得関係
//=============================================

// 方向ボタン書き込み用				// 元の値
#define XI_BUTTON_UP				XINPUT_GAMEPAD_DPAD_UP
#define XI_BUTTON_DOWN				XINPUT_GAMEPAD_DPAD_DOWN
#define XI_BUTTON_LEFT				XINPUT_GAMEPAD_DPAD_LEFT
#define XI_BUTTON_RIGHT				XINPUT_GAMEPAD_DPAD_RIGHT

// STARTなどの書き込み用			// 元の値
#define XI_BUTTON_START				XINPUT_GAMEPAD_START
#define XI_BUTTON_BACK				XINPUT_GAMEPAD_BACK

// 多分左右スティック押し込み		// 元の値
#define XI_BUTTON_LEFT_THUMB		XINPUT_GAMEPAD_LEFT_THUMB
#define XI_BUTTON_RIGHT_THUMB		XINPUT_GAMEPAD_RIGHT_THUMB

// 多分左右ボタン(notトリガー)		// 元の値
#define XI_BUTTON_LEFT_SHOULDER		XINPUT_GAMEPAD_LEFT_SHOULDER
#define XI_BUTTON_RIGHT_SHOULDER	XINPUT_GAMEPAD_RIGHT_SHOULDER

// A,Bなどのボタン書き込み用		// 元の値
#define XI_BUTTON_A					XINPUT_GAMEPAD_A
#define XI_BUTTON_B					XINPUT_GAMEPAD_B
#define XI_BUTTON_X					XINPUT_GAMEPAD_X
#define XI_BUTTON_Y					XINPUT_GAMEPAD_Y

//=============================================
// XInput設定関係
//=============================================

// 左右のアナログスティックのデッドゾーン
#define XI_LEFT_THUMB_DEADZONE		XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
#define XI_RIGHT_THUMB_DEADZONE		XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE

// 左右トリガーボタンの閾値。これ以上なら押していることにする
#define XI_TRIGGER_THRESHOLD		XINPUT_GAMEPAD_TRIGGER_THRESHOLD

class CInput
{
public:
	CInput();

	// 更新処理
	void Update();

	// キーボード関連
	bool GetKeyPress(int vkey);
	bool GetKeyTrigger(int vkey);
	bool GetKeyUp(int vkey);

	// マウスカーソルの座標更新
	void UpdateCursorPos();
	void UpdateClientCursorPos(HWND hwnd);

	// マウスカーソルの座標を取得
	POINT GetCursorPosition();
	POINT GetCursorClientPosition();

	// 前マウスカーソルの座標を取得
	POINT GetOldCursorPosition();
	POINT GetOldCursorClientPosition();

	// マウスカーソルの座標の差を取得
	POINT GetCursorDefference();
	POINT GetCursorClientDefference();

	// XInput関連	->	ボタン入力
	bool GetButtonPress(int vbutton);
	bool GetButtonTrigger(int vbutton);
	bool GetButtonUp(int vbutton);

	// XInput関連	->	トリガーボタン入力(true で左トリガー)
	bool GetTriggerButtonPress(bool left);
	bool GetTriggerButtonTrigger(bool left);
	bool GetTriggerButtonUp(bool left);

	// XInput関連	->	トリガーボタン入力(true で左トリガー) -> 閾値を渡す(走る処理など用)
	bool GetTriggerButtonPress(bool left, BYTE volume);
	bool GetTriggerButtonTrigger(bool left, BYTE volume);
	bool GetTriggerButtonUp(bool left, BYTE volume);

	// XInput関連	->	アナログスティック入力(true で左スティック)
	// -> 左向き入力なら最小 -1.0f, 入力無しで 0.0f, 右向き入力なら最大 1.0f返す
	// 方向け具合で変わります。
	float GetAxisX(bool left);
	float GetAxisY(bool left);

	// XInput関連	->	アナログスティック入力(true で左スティック)
	// -> 左向き入力なら -1.0f, 入力無しで 0.0f, 右向き入力なら 1.0f返す
	float GetAxisRawX(bool left);
	float GetAxisRawY(bool left);

	// コントローラ―の情報を取得
	XINPUT_STATE GetControllerState();

	// XInputコントローラーが接続されているか
	bool CheckConeectedXInput();

private:

	// キーボード入力を保存する変数
	BYTE sma_keystate[KEY_MAX];
	BYTE sma_oldkeystate[KEY_MAX];

	// マウスカーソルの位置
	POINT m_cursor_pos;
	POINT m_old_cursor_pos;

	// マウスカーソルの位置
	POINT m_cursor_client_pos;
	POINT m_old_cursor_client_pos;

	// XInputを保存する変数
	XINPUT_STATE m_controller_state;
	XINPUT_STATE m_old_controller_state;

	// コントローラーが繋がっているかどうか
	bool mb_xinput_connected;
};
