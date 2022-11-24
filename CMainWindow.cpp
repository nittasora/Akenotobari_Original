#include "CMainWindow.h"
#include "CTimer.h"
#include "CInput.h"
#include "CDirect3D.h"
#include "CXAudio2.h"
#include "CSceneManager.h"

#include "CSingleton.h"

// ウィンドウの形式
#define WINDOW_CLASS WS_POPUPWINDOW

//WS_OVERLAPPEDWINDOW

// ウィンドウの初期座標
constexpr int SCREEN_POS_X = 0;					// ウインドウの初期 X 位置
constexpr int SCREEN_POS_Y = 0;					// ウインドウの初期 Y 位置

// クライアント領域の幅
constexpr int CLIENT_WIDTH = 1500;					// クライアント領域の横幅
constexpr int CLIENT_HEIGHT = 800;					// クライアント領域の縦幅

//=================================================
// ポップアップウィンドウの場合のウィンドウサイズ
//=================================================

#if WINDOW_CLASS == WS_POPUPWINDOW

#if defined(_DEBUG)

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 2;		// ウインドウの幅
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 2;	// ウインドウの幅

#else

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 2;		// ウインドウの幅
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 2;	// ウインドウの幅

#endif

//=================================================
// 通常ウィンドウの場合のウィンドウサイズ
//=================================================

#else

#if defined(_DEBUG)

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 16;		// ウインドウの幅
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 39;	// ウインドウの幅

#else

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 16;		// ウインドウの幅
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 39;	// ウインドウの幅

#endif

#endif

const char* CLASS_NAME = "朱の帳";	// ウインドウクラスの名前
const char* WINDOW_NAME = "朱の帳";	// ウィンドウの名前

// 拡張ウィンドウスタイル
constexpr DWORD WINDOW_EX_STYLE = 0;

// ウィンドウスタイル
constexpr DWORD WINDOW_STYLE = ((((WINDOW_CLASS | WS_VISIBLE) & ~WS_MINIMIZEBOX) & ~WS_MAXIMIZEBOX) & ~WS_SIZEBOX);

LRESULT CMainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int sts;
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_KEYDOWN:  // 何かキーが押されたら
		//switch (wp)
		//{
		//	//case VK_ESCAPE:  // ESCキーだったら
		//	//	int sts = MessageBoxA(hWnd, "終了しますか?", "終了確認", MB_YESNO);
		//	//	if (sts == IDYES) {
		//	//		DestroyWindow(hWnd);  // ウインドウを終了させる関数呼び出し
		//	//		this->mb_mainloop_is_continue = false;
		//	//		return 0;
		//	//	}
		//	//	else {
		//	//		return 0;
		//	//	}
		//	//	break;
		//}
		break;
	case WM_CLOSE:  // xボタンが押されたら
		sts = MessageBoxA(hWnd, "終了しますか?", "終了確認", MB_YESNO);
		if (sts == IDYES) {
			DestroyWindow(hWnd);  // ウインドウを終了させる関数呼び出し
			this->mb_mainloop_is_continue = false;
			return 0;
		}
		else {
			return 0;
		}
		break;
	case WM_ACTIVATE:  // ウインドウがアクティブまたは非アクティブに変化した場合
		if (LOWORD(wp) != WA_INACTIVE) {  // アクティブになった場合
			CSingleton<CTimer>::GetInstance()->OnResume();
		}
		break;

	case WM_EXITSIZEMOVE: // ウインドウのサイズ変更または移動が完了した場合
		CSingleton<CTimer>::GetInstance()->OnResume();
		break;
	case WM_DESTROY:
		return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

void CMainWindow::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	//=============================
	// WinMainの情報格納
	//=============================

	this->mh_instance = hInstance;
	this->mh_previnstance = hPrevInstance;
	this->mlp_cmdline = lpCmdLine;
	this->n_cmdshow = nCmdShow;

	//=============================
	// フラグの初期化
	//=============================

	this->mb_initialize_error = false;		// エラーなしとする
	this->mb_mainloop_is_continue = true;	// ループは続けるものとする

	//=============================
	// ウィンドウ作成と登録
	//=============================

	this->RegistWindow(CLASS_NAME, this->mh_instance);
	this->nuCreateWindow(WINDOW_EX_STYLE, WINDOW_NAME, WINDOW_STYLE, SCREEN_POS_X, SCREEN_POS_Y, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr);

	//=============================
	// インスタンス取得関係
	//=============================

	CDirect3D* cdirect3d = CSingleton<CDirect3D>::GetInstance();
	CXAudio2* cxaudio = CSingleton<CXAudio2>::GetInstance();

	//=============================
	// インスタンス群初期化部
	//=============================

	// 成否判定用
	HRESULT hr;

	// CDirect3Dの初期化
	hr = cdirect3d->Initialize(this->m_hwnd);
	if (this->ErrorCheck(hr, "Direct3Dの初期化に失敗")) return;

	// 画面クリア
	FLOAT clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	cdirect3d->GetContext()->ClearRenderTargetView(cdirect3d->GetRenderTargetView(), clear_color);

	// 画面切り替え
	cdirect3d->GetSwapChain()->Present(0, 0);

	// CXAudio2の初期化
	hr = cxaudio->Initialize();
	if (this->ErrorCheck(hr, "XAudio2の初期化に失敗")) return;

	//=============================
	// クライアント領域幅の格納
	//=============================

	RECT rect;
	if (GetClientRect(this->m_hwnd, &rect)) {
		this->m_client_size.width = rect.right;
		this->m_client_size.height = rect.bottom;
	}

	// ウインドウの表示
	ShowWindow(this->m_hwnd, nCmdShow);	// 指定されたウィンドウの表示状態を設定(ウィンドウを表示)
	UpdateWindow(this->m_hwnd);			// ウィンドウの状態を直ちに反映(ウィンドウのクライアント領域を更新)

}

int CMainWindow::MainLoop()
{
	// エラー発生時ループを行わない
	if (this->mb_initialize_error) return 0;

	// メッセージ情報
	MSG msg;

	CTimer* timer = CSingleton<CTimer>::GetInstance();
	CInput* input = CSingleton<CInput>::GetInstance();
	CXAudio2* cxaudio = CSingleton<CXAudio2>::GetInstance();
	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	scene_manager->Initialize();

	timer->OnResume();

	// メッセージループ　→　ゲームのメインループ
	// 第１引数：メッセージ情報
	// 第２引数：ウィンドウのハンドル
	// 第３引数：最初のメッセージ
	// 第４引数：最後のメッセージ
	while (this->mb_mainloop_is_continue) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message == WM_QUIT) {
				break;
			}
			else {
				// 呼び出し側スレッドのメッセージキューからメッセージを取得
				TranslateMessage(&msg);					// 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);					// ウィンドウプロシージャへメッセージを送出
			}
		}
		else {  // メッセージが何も発生していない時
			// ↓　ゲームに関するプログラムを書く場所

			if (timer->ReachedNextFrame() == TRUE) {

				// 入力更新
				input->Update();
				input->UpdateCursorPos();
				input->UpdateClientCursorPos(this->m_hwnd);

				// 音楽更新
				cxaudio->Update();

				scene_manager->Update();

				if (!scene_manager->CheckLoopContinue()) {
					this->mb_mainloop_is_continue = false;
				}
			}

			// ↑　ゲームに関するプログラムを書く場所
		}
	}

	scene_manager->Finalize();

	return (int)msg.wParam;
}

void CMainWindow::Finalize()
{
	//=============================
	// インスタンス解放関係
	//=============================

}

bool CMainWindow::ErrorCheck(HRESULT hr, LPCSTR lpText)
{
	// エラー発生
	if (FAILED(hr)) {
		// 警告文を出し(制作者よりもユーザー向け)
		MessageBoxA(this->m_hwnd, lpText, "エラー", MB_OK | MB_ICONERROR);
		this->mb_initialize_error = true;
		this->mb_mainloop_is_continue = false;
		return true;
	}

	return false;
}

bool CMainWindow::TextBox(LPCSTR lpText)
{
	// メッセージボックス
	int result = MessageBoxA(this->m_hwnd, lpText, "確認", MB_YESNO | MB_ICONWARNING);
	return result == IDYES;
}
