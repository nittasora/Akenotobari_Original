#pragma once

struct Transform;
struct Scale;
struct Matrix4_4;

class CCamera
{
public:
	// ������
	void Initilize();

	// �X�V
	void Update();

	// �I��
	void Finalize();

	// Transform�擾
	Transform GetTransform();

	// Transform�ݒ�
	void SetTransform(const Transform* transform);

	// �X�N���[���T�C�Y�ݒ�
	void SetScreenScale(float x, float y);

	// 2D�J�����p�s��̎擾
	Matrix4_4 Get2DMatrix();

private:
	Transform* m_transform;
	Scale* m_screen_scale;
	Matrix4_4* m_2d_matrix;
};

