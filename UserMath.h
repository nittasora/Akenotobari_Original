#pragma once

#include <Windows.h>
#include <cmath>

// 4 * 4 �̍s��
struct Matrix4_4 {
	float index[4][4];
};

// 4 * 1 �̍s��
struct Matrix4_1 {
	float index[4];
};

namespace UserMath
{
	//=============================
	// ��������I���Ȃ�
	//=============================

	// ����������
	bool Initialize();
	// �I������
	bool Finalize();

	//=============================
	// XorShift �ɂ�闐������
	//=============================

	// �I�[�\�h�b�N�X�ȗ�������
	UINT32 XOS_Rand();

	// 0 �` max �܂ł̒l��Ԃ�
	// max : �ő�l
	UINT32 XOS_Rand(UINT32 max);

	// min �` max �܂ł̒l��Ԃ�
	// max : �ő�l
	// min : �ŏ��l
	UINT32 XOS_Rand(UINT32 min, UINT32 max);

	//=============================
	// �p�x�֌W
	//=============================

	// �x�������W�A���ɕϊ�
	double GetRadianByDegree(double degree);

	// �x�������W�A���ɕϊ�
	float GetRadianByDegree(float degree);

	// ���W�A����x���ɕϊ�
	double GetDegree(double radian);

	// ���W�A����x���ɕϊ�
	float GetRadian(float radian);

	//=============================
	// �s��֌W
	//=============================

	Matrix4_1 MutiplyMatrix(const Matrix4_4* mat4_4, const Matrix4_1* mat4_1);
	Matrix4_4 MutiplyMatrix(const Matrix4_4* mat4_4_left, const Matrix4_4* mat4_4_right);

	Matrix4_4 GetTranslationMatrix(float x, float y, float z);
	Matrix4_4 GetScaleMatrix(float x, float y, float z);
	Matrix4_4 GetRotationXMatrix(float x);
	Matrix4_4 GetRotationYMatrix(float y);
	Matrix4_4 GetRotationZMatrix(float z);
};