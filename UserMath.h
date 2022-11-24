#pragma once

#include <Windows.h>
#include <cmath>

// 4 * 4 の行列
struct Matrix4_4 {
	float index[4][4];
};

// 4 * 1 の行列
struct Matrix4_1 {
	float index[4];
};

namespace UserMath
{
	//=============================
	// 初期化や終了など
	//=============================

	// 初期化処理
	bool Initialize();
	// 終了処理
	bool Finalize();

	//=============================
	// XorShift による乱数生成
	//=============================

	// オーソドックスな乱数生成
	UINT32 XOS_Rand();

	// 0 ～ max までの値を返す
	// max : 最大値
	UINT32 XOS_Rand(UINT32 max);

	// min ～ max までの値を返す
	// max : 最大値
	// min : 最小値
	UINT32 XOS_Rand(UINT32 min, UINT32 max);

	//=============================
	// 角度関係
	//=============================

	// 度数をラジアンに変換
	double GetRadianByDegree(double degree);

	// 度数をラジアンに変換
	float GetRadianByDegree(float degree);

	// ラジアンを度数に変換
	double GetDegree(double radian);

	// ラジアンを度数に変換
	float GetRadian(float radian);

	//=============================
	// 行列関係
	//=============================

	Matrix4_1 MutiplyMatrix(const Matrix4_4* mat4_4, const Matrix4_1* mat4_1);
	Matrix4_4 MutiplyMatrix(const Matrix4_4* mat4_4_left, const Matrix4_4* mat4_4_right);

	Matrix4_4 GetTranslationMatrix(float x, float y, float z);
	Matrix4_4 GetScaleMatrix(float x, float y, float z);
	Matrix4_4 GetRotationXMatrix(float x);
	Matrix4_4 GetRotationYMatrix(float y);
	Matrix4_4 GetRotationZMatrix(float z);
};