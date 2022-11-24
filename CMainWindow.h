#pragma once

#include "CWindow.h"

// 作成 : 新田

//=============================
// 概要など
//=============================
// WindowsAPIによるウィンドウ作成等を行うクラス
// 元main.cpp内の処理が多い

class CMainWindow : public CWindow
{
public:
	//=============================
	// 処理関係
	//=============================

	// メインウィンドウのウィンドウプロック関数
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) override;

	// 初期化処理
	void Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	// メインループ処理
	int MainLoop();

	// 終了処理
	void Finalize();

	// エラーチェック用
	bool ErrorCheck(HRESULT hr, LPCSTR lpText);
	bool TextBox(LPCSTR lpText);

private:
	// WinMainに渡される変数群
	HINSTANCE mh_instance;
	HINSTANCE mh_previnstance;
	LPSTR mlp_cmdline;
	int n_cmdshow;

	// 初期化エラー判別用
	bool mb_initialize_error = true;

	// メインループが続けられるかどうか
	bool mb_mainloop_is_continue = false;;

	// クライアントサイズ用
	struct {
		LONG width;
		LONG height;
	}m_client_size;

};
