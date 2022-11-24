#pragma once

// �l�p�`����O�p�`�|���S���ŕ\������ꍇ�̒��_��
constexpr size_t VERTEX_BUFFER_SIZE = 6;

// �l�p�`�̒��_�ʒu
enum SQUARE_VERTEX
{
	LEFT_TOP,
	RIGHT_TOP,
	LEFT_BOTTOM,
	RIGHT_BOTTOM,
	SQUARE_VERTEX_NUM
};

// ���f�����\�����钸�_�P��\���\����
struct ModelVertex
{
	float x, y, z;		// �ʒu
	float r, g, b, a;	// ���_�F
	float u, v;			// �e�N�X�`��UV���W
};

// ���W
struct Translate
{
	float x, y, z;
};

// �傫��
struct Scale
{
	float x, y, z;
};

// �p�x
struct Angle
{
	float x, y, z;
};

// UV���W
struct UV
{
	float u, v;
};

// �F
struct Color
{
	float r, g, b, a;
};

// ���x
struct Velocity
{
	float x, y;
};

// �g�����X�t�H�[�����
struct Transform
{
	Translate position;
	Scale scale;
	Angle angle;
};