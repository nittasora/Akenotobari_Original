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
	// キー受付時間
	constexpr float KEY_TIME_LIMIT = 5.0f;

	// 秘密のコード
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

	// 入力したキー配列号
	std::vector<int> gvi_key_code;

	// 時間記録用
	float gf_time;

	// 入力成功回数記録用
	size_t gui_input_count;
}

CEditorManager::CEditorManager()
{
	// キーコードを消す
	gvi_key_code.clear();

	// タイムの初期化
	gf_time = 0.0f;

	gui_input_count = 0;
}

CEditorManager::~CEditorManager()
{
}

void CEditorManager::Update()
{
	// インスタンス取得
	CTimer* timer = CSingleton<CTimer>::GetInstance();
	CInput* input = CSingleton<CInput>::GetInstance();
	CSceneManager* scene = CSingleton<CSceneManager>::GetInstance();

	// 時間を更新
	gf_time += timer->GetDeltaTime();

	// 秘密のコードに使われているキーが入力されたかテスト
	for (int i = 0; i < static_cast<int>(KEY_MAX); i++)
	{
		if (input->GetKeyTrigger(i)) {
			// 受付時間を初期化
			gf_time = 0.0f;

			// 秘密のコードと一致するなら進める
			if (i == gai_secret_key_code[gui_input_count]) {
				gui_input_count++;
			}
			// しないならば始めから
			else {
				gui_input_count = 0;
			}
			break;
		}
	}

	// 入力成功回数が秘密のコードを超えたらエディタ―へ移動する
	if (gui_input_count >= gai_secret_key_code.size()) {
		scene->ChangeScene(SCENE_EDITOR_SELECT, FADE_TYPE_01);
	}

	// 入力受付時間を超えたらキーをリセットする
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