#include <random>
#include "UserMath.h"

namespace UserMath {
	namespace {
		// 計算用のデータを格納する構造体
		struct
		{
			// π の値
			double gd_pi;

			// π の値
			float gf_pi;

			// ラジアン -> 度数変換ように計算済みの π の値
			double gd_calculated_pi;

			// ラジアン -> 度数変換ように計算済みの π の値
			float gf_calculated_pi;

			// XorShift用構造体
			struct
			{
				// シード値
				UINT32 mui_seed[4];

				// シード値を算出
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

// 初期化処理
bool UserMath::Initialize()
{
	// π 算出
	DataStruct.gd_pi = std::acos(-1);

	DataStruct.gd_calculated_pi = (DataStruct.gd_pi / 180.0);

	// π 算出
	DataStruct.gf_pi = std::acosf(-1);

	DataStruct.gf_calculated_pi = (DataStruct.gf_pi / 180.0f);

	// XorShift のシード値を設定
	std::random_device rnddevice;
	DataStruct.XorShift_DataStruct.SetSeed(rnddevice());

	return false;
}

// 終了処理
bool UserMath::Finalize()
{
	return false;
}

// オーソドックスな乱数生成
UINT32 UserMath::XOS_Rand()
{
	UINT32 t = (DataStruct.XorShift_DataStruct.mui_seed[0] ^ (DataStruct.XorShift_DataStruct.mui_seed[0] << 11));
	DataStruct.XorShift_DataStruct.mui_seed[0] = DataStruct.XorShift_DataStruct.mui_seed[1];
	DataStruct.XorShift_DataStruct.mui_seed[1] = DataStruct.XorShift_DataStruct.mui_seed[2];
	DataStruct.XorShift_DataStruct.mui_seed[2] = DataStruct.XorShift_DataStruct.mui_seed[3];
	return DataStruct.XorShift_DataStruct.mui_seed[3] = ((DataStruct.XorShift_DataStruct.mui_seed[3] ^ (DataStruct.XorShift_DataStruct.mui_seed[3] >> 19)) ^ (t ^ (t >> 8)));
}

// 0 ～ max までの値を返す
// max : 最大値
UINT32 UserMath::XOS_Rand(UINT32 max)
{
	if (max < 0) return 0;
	UINT32 rnd_result = XOS_Rand();
	return (rnd_result % (max + 1));
}

// min ～ max までの値を返す
// max : 最大値
// min : 最小値
UINT32 UserMath::XOS_Rand(UINT32 min, UINT32 max)
{
	if (max < 0 || min < 0 || min >= max) return 0;
	UINT32 rnd_result = XOS_Rand();
	return ((rnd_result % (max - min + 1)) + min);
}

// 度数をラジアンに変換
double UserMath::GetRadianByDegree(double degree)
{
	return degree * DataStruct.gd_calculated_pi;
}

// 度数をラジアンに変換
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
	// 以下 4 * 4 行列と 4 * 1 行列の合成のテンプレート
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
	// 以下 4 * 4 行列と 4 * 4 行列の合成のテンプレート
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

	// 以下平行移動行列の作成

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

	// 以下拡大縮小行列の作成

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

	// 以下X軸回転行列の作成

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

	// 以下Y軸回転行列の作成

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

	// 以下Z軸回転行列の作成

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

