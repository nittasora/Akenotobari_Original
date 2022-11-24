#include <random>
#include "UserMath.h"

namespace UserMath {
	namespace {
		// �v�Z�p�̃f�[�^���i�[����\����
		struct
		{
			// �� �̒l
			double gd_pi;

			// �� �̒l
			float gf_pi;

			// ���W�A�� -> �x���ϊ��悤�Ɍv�Z�ς݂� �� �̒l
			double gd_calculated_pi;

			// ���W�A�� -> �x���ϊ��悤�Ɍv�Z�ς݂� �� �̒l
			float gf_calculated_pi;

			// XorShift�p�\����
			struct
			{
				// �V�[�h�l
				UINT32 mui_seed[4];

				// �V�[�h�l���Z�o
				void SetSeed(UINT32 seed)
				{
					for (UINT32 i = 0; i < 4; i++) {
						this->mui_seed[i] = seed = 1812433253U * (seed ^ (seed >> 30)) + i;
					}
				}

			} XorShift_DataStruct;

		} DataStruct;
	}
}

// ����������
bool UserMath::Initialize()
{
	// �� �Z�o
	DataStruct.gd_pi = std::acos(-1);

	DataStruct.gd_calculated_pi = (DataStruct.gd_pi / 180.0);

	// �� �Z�o
	DataStruct.gf_pi = std::acosf(-1);

	DataStruct.gf_calculated_pi = (DataStruct.gf_pi / 180.0f);

	// XorShift �̃V�[�h�l��ݒ�
	std::random_device rnddevice;
	DataStruct.XorShift_DataStruct.SetSeed(rnddevice());

	return false;
}

// �I������
bool UserMath::Finalize()
{
	return false;
}

// �I�[�\�h�b�N�X�ȗ�������
UINT32 UserMath::XOS_Rand()
{
	UINT32 t = (DataStruct.XorShift_DataStruct.mui_seed[0] ^ (DataStruct.XorShift_DataStruct.mui_seed[0] << 11));
	DataStruct.XorShift_DataStruct.mui_seed[0] = DataStruct.XorShift_DataStruct.mui_seed[1];
	DataStruct.XorShift_DataStruct.mui_seed[1] = DataStruct.XorShift_DataStruct.mui_seed[2];
	DataStruct.XorShift_DataStruct.mui_seed[2] = DataStruct.XorShift_DataStruct.mui_seed[3];
	return DataStruct.XorShift_DataStruct.mui_seed[3] = ((DataStruct.XorShift_DataStruct.mui_seed[3] ^ (DataStruct.XorShift_DataStruct.mui_seed[3] >> 19)) ^ (t ^ (t >> 8)));
}

// 0 �` max �܂ł̒l��Ԃ�
// max : �ő�l
UINT32 UserMath::XOS_Rand(UINT32 max)
{
	if (max < 0) return 0;
	UINT32 rnd_result = XOS_Rand();
	return (rnd_result % (max + 1));
}

// min �` max �܂ł̒l��Ԃ�
// max : �ő�l
// min : �ŏ��l
UINT32 UserMath::XOS_Rand(UINT32 min, UINT32 max)
{
	if (max < 0 || min < 0 || min >= max) return 0;
	UINT32 rnd_result = XOS_Rand();
	return ((rnd_result % (max - min + 1)) + min);
}

// �x�������W�A���ɕϊ�
double UserMath::GetRadianByDegree(double degree)
{
	return degree * DataStruct.gd_calculated_pi;
}

// �x�������W�A���ɕϊ�
float UserMath::GetRadianByDegree(float degree)
{
	return degree * DataStruct.gf_calculated_pi;
}

double UserMath::GetDegree(double radian)
{
	return radian / DataStruct.gd_calculated_pi;
}

float UserMath::GetRadian(float radian)
{
	return radian / DataStruct.gf_calculated_pi;
}

Matrix4_1 UserMath::MutiplyMatrix(const Matrix4_4 * mat4_4, const Matrix4_1 * mat4_1)
{
	// �ȉ� 4 * 4 �s��� 4 * 1 �s��̍����̃e���v���[�g
	Matrix4_1 tmp = {};
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			tmp.index[i] += (mat4_4->index[i][j] * mat4_1->index[j]);
		}
	}
	return tmp;
}

Matrix4_4 UserMath::MutiplyMatrix(const Matrix4_4 * mat4_4_left, const Matrix4_4 * mat4_4_right)
{
	// �ȉ� 4 * 4 �s��� 4 * 4 �s��̍����̃e���v���[�g
	Matrix4_4 tmp = {};
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			for (size_t k = 0; k < 4; k++)
			{
				tmp.index[i][j] += (mat4_4_left->index[i][k] * mat4_4_right->index[k][j]);
			}
		}
	}
	return tmp;
}

Matrix4_4 UserMath::GetTranslationMatrix(float x, float y, float z)
{
	Matrix4_4 tmp;

	// �ȉ����s�ړ��s��̍쐬

	tmp.index[0][0] = 1.0f;
	tmp.index[0][1] = 0.0f;
	tmp.index[0][2] = 0.0f;
	tmp.index[0][3] = x;

	tmp.index[1][0] = 0.0f;
	tmp.index[1][1] = 1.0f;
	tmp.index[1][2] = 0.0f;
	tmp.index[1][3] = y;

	tmp.index[2][0] = 0.0f;
	tmp.index[2][1] = 0.0f;
	tmp.index[2][2] = 1.0f;
	tmp.index[2][3] = z;

	tmp.index[3][0] = 0.0f;
	tmp.index[3][1] = 0.0f;
	tmp.index[3][2] = 0.0f;
	tmp.index[3][3] = 1.0f;
	return tmp;
}

Matrix4_4 UserMath::GetScaleMatrix(float x, float y, float z)
{
	Matrix4_4 tmp;

	// �ȉ��g��k���s��̍쐬

	tmp.index[0][0] = x;
	tmp.index[0][1] = 0.0f;
	tmp.index[0][2] = 0.0f;
	tmp.index[0][3] = 0.0f;

	tmp.index[1][0] = 0.0f;
	tmp.index[1][1] = y;
	tmp.index[1][2] = 0.0f;
	tmp.index[1][3] = 0.0f;

	tmp.index[2][0] = 0.0f;
	tmp.index[2][1] = 0.0f;
	tmp.index[2][2] = z;
	tmp.index[2][3] = 0.0f;

	tmp.index[3][0] = 0.0f;
	tmp.index[3][1] = 0.0f;
	tmp.index[3][2] = 0.0f;
	tmp.index[3][3] = 1.0f;

	return tmp;
}

Matrix4_4 UserMath::GetRotationXMatrix(float x)
{
	float radian = UserMath::GetRadianByDegree(x);
	float sin_x = std::sinf(radian);
	float cos_x = std::cosf(radian);

	Matrix4_4 tmp;

	// �ȉ�X����]�s��̍쐬

	tmp.index[0][0] = 1.0f;
	tmp.index[0][1] = 0.0f;
	tmp.index[0][2] = 0.0f;
	tmp.index[0][3] = 0.0f;

	tmp.index[1][0] = 0.0f;
	tmp.index[1][1] = cos_x;
	tmp.index[1][2] = -sin_x;
	tmp.index[1][3] = 0.0f;

	tmp.index[2][0] = 0.0f;
	tmp.index[2][1] = sin_x;
	tmp.index[2][2] = cos_x;
	tmp.index[2][3] = 0.0f;

	tmp.index[3][0] = 0.0f;
	tmp.index[3][1] = 0.0f;
	tmp.index[3][2] = 0.0f;
	tmp.index[3][3] = 1.0f;

	return tmp;
}

Matrix4_4 UserMath::GetRotationYMatrix(float y)
{
	float radian = UserMath::GetRadianByDegree(y);
	float sin_y = std::sinf(radian);
	float cos_y = std::cosf(radian);

	Matrix4_4 tmp;

	// �ȉ�Y����]�s��̍쐬

	tmp.index[0][0] = cos_y;
	tmp.index[0][1] = 0.0f;
	tmp.index[0][2] = sin_y;
	tmp.index[0][3] = 0.0f;

	tmp.index[1][0] = 0.0f;
	tmp.index[1][1] = 1.0f;
	tmp.index[1][2] = 0.0f;
	tmp.index[1][3] = 0.0f;

	tmp.index[2][0] = -sin_y;
	tmp.index[2][1] = 0.0f;
	tmp.index[2][2] = cos_y;
	tmp.index[2][3] = 0.0f;

	tmp.index[3][0] = 0.0f;
	tmp.index[3][1] = 0.0f;
	tmp.index[3][2] = 0.0f;
	tmp.index[3][3] = 1.0f;

	return tmp;
}

Matrix4_4 UserMath::GetRotationZMatrix(float z)
{
	float radian = UserMath::GetRadianByDegree(z);
	float sin_z = std::sinf(radian);
	float cos_z = std::cosf(radian);

	Matrix4_4 tmp;

	// �ȉ�Z����]�s��̍쐬

	tmp.index[0][0] = cos_z;
	tmp.index[0][1] = -sin_z;
	tmp.index[0][2] = 0.0f;
	tmp.index[0][3] = 0.0f;

	tmp.index[1][0] = sin_z;
	tmp.index[1][1] = cos_z;
	tmp.index[1][2] = 0.0f;
	tmp.index[1][3] = 0.0f;

	tmp.index[2][0] = 0.0f;
	tmp.index[2][1] = 0.0f;
	tmp.index[2][2] = 1.0f;
	tmp.index[2][3] = 0.0f;

	tmp.index[3][0] = 0.0f;
	tmp.index[3][1] = 0.0f;
	tmp.index[3][2] = 0.0f;
	tmp.index[3][3] = 1.0f;

	return tmp;
}

