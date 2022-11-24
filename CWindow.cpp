#include "CWindow.h"

LRESULT CWindow::StaticWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CWindow* This = (CWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!This) {//取得できなかった(ウィンドウ生成中)場合
		if (msg == WM_CREATE) {
			This = (CWindow*)((LPCREATESTRUCT)lp)->lpCreateParams;
			if (This) {
				SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)This);
				return This->WndProc(hWnd, msg, wp, lp);
			}
		}
	}
	else {//取得できた場合(ウィンドウ生成後)
		return This->WndProc(hWnd, msg, wp, lp);
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

CWindow::CWindow()
{
}

CWindow::~CWindow()
{
}

bool CWindow::RegistWindow(const char * window_class_name, const HINSTANCE hInst)
{
	strcpy_s(this->window_class_name, window_class_name);
	m_hInstance = hInst;
	WNDCLASSEX wc;

	// ウィンドウクラスの情報を設定
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = CWindow::StaticWndProc;				// ウィンドウプロシージャ
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;									// インスタンスハンドル
	wc.hIcon = (HICON)LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->window_class_name;				// ウィンドウクラス名

	// ウィンドウクラスを登録する
	if (RegisterClassEx(&wc) == 0) { return false; }

	return true;
}

LRESULT CWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

HWND CWindow::GetHWnd()
{
	return this->m_hwnd;
}

HINSTANCE CWindow::GetHInstance()
{
	return this->m_hInstance;
}

bool CWindow::nuCreateWindow(DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
	m_hwnd = CreateWindowEx(
		dwExStyle,					// 拡張ウィンドウスタイル
		this->window_class_name,	// ウィンドウクラス名
		lpWindowName,				// タイトルバーに表示する文字列
		dwStyle,					//  ウィンドウの種類
		x,							// ウィンドウを表示する位置（X座標）
		y,							// ウィンドウを表示する位置（Y座標）
		nWidth,						// ウィンドウの幅
		nHeight,					// ウィンドウの高さ
		hWndParent,					// 親ウィンドウのウィンドウハンドル
		hMenu,						// メニューハンドル
		m_hInstance,				// インスタンスハンドル
		this						// その他の作成データ
	);
	if (m_hwnd == nullptr)
		return false;

	return true;
}
