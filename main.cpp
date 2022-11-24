#undef UNICODE  // Unicodeではなく、マルチバイト文字を使う

#include <Windows.h>
#include "CMainWindow.h"
#include "UserMath.h"

#include "CSingleton.h"

//=============================================================================
// メイン関数
//  (WINAPI : Win32API関数を呼び出す時の規約)
//  hInstance     : このアプリケーションのインスタンスハンドル(識別子)
//  hPrevInstance : 常にNULL(16bit時代の名残り)
//  lpCmdLine     : コマンドラインから受け取った引数へのポインタ
//  nCmdShow      : アプリケーションの初期ウインドウ表示方法の指定
//=============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// return で返す値
	int result = 0;

	//// ユーザー定義の数学初期化関数
	UserMath::Initialize();

	// メインウィンドウ
	CMainWindow* main_window = CSingleton<CMainWindow>::GetInstance();

	// メインウィンドウの初期化
	main_window->Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	// メインループ
	result = main_window->MainLoop();

	// メインウィンドウの解放
	main_window->Finalize();

	// ユーザー定義の数学解放関数
	UserMath::Finalize();

	// Singleton終了処理 
	CSingletonFinalizer::Finalize();

	return result;
}


