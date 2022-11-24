#pragma once

enum ObjectTag {
	// デファルトタグ
	TAG_DEFAULT,

	// カーソル用タグ
	TAG_CURSOR,

	// プレイヤー用タグ
	TAG_PLAYER,

	// マップチップ用タグ
	TAG_MAPCHIP,

	// 見えないマップチップ
	TAG_INVISIBLE_BLOCK,

	// ステージ境界線
	TAG_BORDER_LEFT,
	TAG_BORDER_RIGHT,
	TAG_BORDER_TOP,
	TAG_BORDER_BOTTOM,

	// ページの境界
	TAG_BORDER_PAGE,

	// ゴール
	TAG_GOAL,

	// カケラ
	TAG_PIECE_1,
	TAG_PIECE_2,
	TAG_PIECE_3,

	// 赤ブロック
	TAG_RED_BLOCK,

	// 青ブロック
	TAG_BLUE_BLOCK,

	// 竜巻
	TAG_TORNADO,

	// ダルマ
	TAG_DHARMA,

	// 大砲
	TAG_CANNON,

	// 弾
	TAG_BALL,

	// リフト
	TAG_LION_TOP,
	TAG_LION_BOTTOM,
};

// オブジェクトのフラグ用変数
using obj_flg = int;

// オブジェクトが動いているか
constexpr obj_flg OBJ_FLG_ACTIVE = (1 << 0);

// オブジェクトが描画されるか
constexpr obj_flg OBJ_FLG_DRAW = (1 << 1);
