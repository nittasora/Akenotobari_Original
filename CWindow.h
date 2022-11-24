#pragma once

#include <Windows.h>

// 作成 : 新田

//=============================
// 概要など
//=============================
// WindowsAPIによるウィンドウ作成等を行うクラス
// 基本的に触ることはない

class CWindow
{
private:
	char window_class_name[1024];

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

protected:

	HWND m_hwnd;

	HINSTANCE m_hInstance;

public:
	CWindow();
	virtual ~CWindow();

	// ウィンドウ定義用メンバ関数
	bool RegistWindow(const char* window_class_name, const HINSTANCE hInst);

	// ウィンドウプロシージャ
	virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	// ウィンドウハンドル取得
	HWND GetHWnd();

	// インスタンス取得
	HINSTANCE GetHInstance();

	// ウィンドウ作成用メンバ関数
	bool nuCreateWindow(
		DWORD dwExStyle,		// 拡張ウィンドウスタイル
		LPCTSTR lpWindowName,	// ウィンドウ名
		DWORD dwStyle,			// ウィンドウスタイル
		int x,					// ウィンドウの横方向の位置
		int y,					// ウィンドウの縦方向の位置
		int nWidth,				// ウィンドウの幅
		int nHeight,			// ウィンドウの高さ
		HWND hWndParent,		// 親ウィンドウまたはオーナーウィンドウのハンドル
		HMENU hMenu				// メニューハンドルまたは子ウィンドウ ID
	);
};

