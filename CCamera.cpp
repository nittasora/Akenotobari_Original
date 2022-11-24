#include "CCamera.h"
#include "ModelData.h"
#include "UserMath.h"

void CCamera::Initilize()
{
	this->m_transform = new Transform;
	this->m_screen_scale = new Scale;
	this->m_2d_matrix = new Matrix4_4;

	*(this->m_transform) = {
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f }
	};

	this->m_screen_scale->x = 2.0f;
	this->m_screen_scale->y = 2.0f;
	this->m_screen_scale->z = 2.0f;
}

void CCamera::Update()
{
	float position_x = this->m_transform->position.x;
	float position_y = this->m_transform->position.y;
	float position_z = this->m_transform->position.z;

	float scale_x = this->m_transform->scale.x * (2.0f / this->m_screen_scale->x);
	float scale_y = this->m_transform->scale.y * (2.0f / this->m_screen_scale->y);
	float scale_z = this->m_transform->scale.z * (2.0f / this->m_screen_scale->z);

	Matrix4_4 translation_matrix = UserMath::GetTranslationMatrix(position_x, position_y, position_z);
	Matrix4_4 scale_matrix = UserMath::GetScaleMatrix(scale_x, scale_y, scale_z);

	*(this->m_2d_matrix) = UserMath::MutiplyMatrix(&translation_matrix, &scale_matrix);
}

void CCamera::Finalize()
{
	if (this->m_transform != nullptr) delete this->m_transform;
	if (this->m_screen_scale != nullptr) delete this->m_screen_scale;
	if (this->m_2d_matrix != nullptr) delete this->m_2d_matrix;
}

Transform CCamera::GetTransform()
{
	return *(this->m_transform);
}

void CCamera::SetTransform(const Transform * transform)
{
	*(this->m_transform) = *transform;
}

void CCamera::SetScreenScale(float x, float y)
{
	this->m_screen_scale->x = x;
	this->m_screen_scale->y = y;
}

Matrix4_4 CCamera::Get2DMatrix()
{
	return *(this->m_2d_matrix);
}
