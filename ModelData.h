#pragma once

// 四角形一つを三角形ポリゴンで表現する場合の頂点数
constexpr size_t VERTEX_BUFFER_SIZE = 6;

// 四角形の頂点位置
enum SQUARE_VERTEX
{
	LEFT_TOP,
	RIGHT_TOP,
	LEFT_BOTTOM,
	RIGHT_BOTTOM,
	SQUARE_VERTEX_NUM
};

// モデルを構成する頂点１つを表す構造体
struct ModelVertex
{
	float x, y, z;		// 位置
	float r, g, b, a;	// 頂点色
	float u, v;			// テクスチャUV座標
};

// 座標
struct Translate
{
	float x, y, z;
};

// 大きさ
struct Scale
{
	float x, y, z;
};

// 角度
struct Angle
{
	float x, y, z;
};

// UV座標
struct UV
{
	float u, v;
};

// 色
struct Color
{
	float r, g, b, a;
};

// 速度
struct Velocity
{
	float x, y;
};

// トランスフォーム情報
struct Transform
{
	Translate position;
	Scale scale;
	Angle angle;
};