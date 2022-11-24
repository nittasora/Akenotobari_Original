#pragma once

// 作成 : 新田
//		  足立

//=============================
// 概要など
//=============================
// 画面遷移時のフェードを管理

#include <vector>
#include "FadeData.h"

class CGameObject;
class CImage;

class CFadeManager
{
public:
	CFadeManager();
	~CFadeManager();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// フェードインを開始
	void StartFadeIn();

	// フェードインを終了
	void StartFadeOut();
	void StartFadeOut(FADE_TYPE  type);

	// フェードが終わったかどうか
	bool CheckFadeInEnd();
	bool CheckFadeOutEnd();

	// フェードの種類を変える
	void SetType(FADE_TYPE  type);

private:

	// FADE_TYPE_00用の更新・描画処理
	void UpdateType00();
	void DrawType00();

	// FADE_TYPE_01用の更新・描画処理 (もっぱらエディタ―用)
	void UpdateType01();
	void DrawType01();

	// FADE_TYPE_02用の更新・描画処理 (もっぱら本番用)
	void UpdateType02();
	void DrawType02();

	// フェード関係のデータ
	struct
	{
		// 現在のフェードの状況
		FADE_CONDITION m_condeition;

		// 現在のフェードのタイプ
		FADE_TYPE  m_type;

		// 次のフェードの状況
		FADE_CONDITION m_next_condeition;

		// フェードに使ったフレーム数
		int mi_fade_count;
	} m_data;

	// オブジェクト関係
	struct
	{
		// delete用
		std::vector<CGameObject*> mvp_delete_obj;

		// 表示するオブジェクト
		std::vector<CImage*> mvp_image;
	} m_object;
};

