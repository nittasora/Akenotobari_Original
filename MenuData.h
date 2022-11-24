#pragma once

// シーンの要素番号
enum MENU_INDEX
{
	MENU_NONE,								// メニュー無し
	MENU_PAUSE,								// オプションメニュー
	MENU_TIPS,								// オプションメニュー
	MENU_INDEX_MAX,						// メニューの数
};

// 最初のシーン
constexpr MENU_INDEX START_MENU_INDEX = MENU_NONE;