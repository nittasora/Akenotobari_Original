#include "CImage.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CGameManager.h"

#include "CSingleton.h"

void CImage::Initilize()
{
}

void CImage::UnInitilize()
{
}

void CImage::Draw()
{
	if (!(this->m_flg & OBJ_FLG_DRAW)) return;
	if (this->m_sprites.empty()) return;

	CDirect3D* directx = CSingleton<CDirect3D>::GetInstance();
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();

	ModelVertex update_vertex[VERTEX_BUFFER_SIZE * 2];
	ModelVertex calculate_verpos[SQUARE_VERTEX_NUM];

	float anim_u = static_cast<float>(this->m_anim.u);
	float anim_v = static_cast<float>(this->m_anim.v);

	Matrix4_4 tarnslate = UserMath::GetTranslationMatrix(this->m_transform.position.x, this->m_transform.position.y, this->m_transform.position.z);
	Matrix4_4 scale = UserMath::GetScaleMatrix(this->m_transform.scale.x, this->m_transform.scale.y, this->m_transform.scale.z);
	Matrix4_4 rotation_z = UserMath::GetRotationZMatrix(this->m_transform.angle.z);

	Matrix4_4 camera_matrix = game_manager->Get2DCameraMatrix();

	for (size_t i = 0; i < SQUARE_VERTEX_NUM; i++)
	{
		Matrix4_1 mat = {
			this->m_default_vertex[i].x, this->m_default_vertex[i].y, this->m_default_vertex[i].z, 1.0f
		};

		mat = UserMath::MutiplyMatrix(&scale, &mat);
		mat = UserMath::MutiplyMatrix(&rotation_z, &mat);
		mat = UserMath::MutiplyMatrix(&tarnslate, &mat);

		mat = UserMath::MutiplyMatrix(&camera_matrix, &mat);

		calculate_verpos[i].x = mat.index[0];
		calculate_verpos[i].y = mat.index[1];
		calculate_verpos[i].z = 0.0f;	// 2Dなので使いません

		calculate_verpos[i].u = (this->m_sprites[this->m_current_sprite].slice.u * this->m_default_vertex[i].u) + (this->m_sprites[this->m_current_sprite].slice.u * anim_u);
		calculate_verpos[i].v = (this->m_sprites[this->m_current_sprite].slice.v * this->m_default_vertex[i].v) + (this->m_sprites[this->m_current_sprite].slice.v * anim_v);
		calculate_verpos[i].r = this->m_color.r;
		calculate_verpos[i].g = this->m_color.g;
		calculate_verpos[i].b = this->m_color.b;
		calculate_verpos[i].a = this->m_color.a;
	}

	update_vertex[0] = calculate_verpos[LEFT_TOP];
	update_vertex[1] = calculate_verpos[RIGHT_TOP];
	update_vertex[2] = calculate_verpos[LEFT_BOTTOM];
	update_vertex[3] = calculate_verpos[LEFT_BOTTOM];
	update_vertex[4] = calculate_verpos[RIGHT_TOP];
	update_vertex[5] = calculate_verpos[RIGHT_BOTTOM];

	update_vertex[6] = calculate_verpos[LEFT_TOP];
	update_vertex[7] = calculate_verpos[LEFT_BOTTOM];
	update_vertex[8] = calculate_verpos[RIGHT_TOP];
	update_vertex[9] = calculate_verpos[RIGHT_TOP];
	update_vertex[10] = calculate_verpos[LEFT_BOTTOM];
	update_vertex[11] = calculate_verpos[RIGHT_BOTTOM];

	ID3D11DeviceContext *context = directx->GetContext();

	// 頂点バッファのデータをvx配列のデータで更新する
	context->UpdateSubresource(
		this->m_vertex_buffer,		// 更新対象のバッファ
		0,				// インデックス(0)
		NULL,				// 更新範囲(nullptr)
		update_vertex,		// 反映データ
		0,				// データの1行のサイズ(0)
		0);				// 1深度スライスのサイズ(0)

	UINT strides = sizeof(ModelVertex);
	UINT offsets = 0;
	context->IASetVertexBuffers(0, 1, &this->m_vertex_buffer, &strides, &offsets);   // gpVertexBufferが頂点バッファ

	context->PSSetShaderResources(0, 1, &this->m_sprites[this->m_current_sprite].mSRV);

	context->Draw(VERTEX_BUFFER_SIZE * 2U, 0);
}

void CImage::UpdateTransform()
{
}

void CImage::SetImage(float left_x, float right_x, float bottom_y, float top_y)
{
	if (right_x < left_x) {
		float tmp = left_x;
		left_x = right_x;
		right_x = tmp;
	}
	if (top_y < bottom_y) {
		float tmp = bottom_y;
		bottom_y = top_y;
		top_y = tmp;
	}

	float width = right_x - left_x;
	float height = top_y - bottom_y;

	this->SetDefaultPosition(width, height);

	this->m_transform.position.x = left_x + (width / 2.0f);
	this->m_transform.position.y = bottom_y + (height / 2.0f);
}

void CImage::SetImageAndColl(float left_x, float right_x, float bottom_y, float top_y)
{
	if (right_x < left_x) {
		float tmp = left_x;
		left_x = right_x;
		right_x = tmp;
	}
	if (top_y < bottom_y) {
		float tmp = bottom_y;
		bottom_y = top_y;
		top_y = tmp;
	}

	float width = right_x - left_x;
	float height = top_y - bottom_y;

	this->SetDefaultPosition(width, height);

	this->m_transform.position.x = left_x + (width / 2.0f);
	this->m_transform.position.y = bottom_y + (height / 2.0f);

	this->m_collider.width = width;
	this->m_collider.height = height;
}

void CImage::SetDefaultUV(float left, float right, float bottom, float top)
{
	if (left < 0.0f) left = 0.0f;
	if (bottom < 0.0f) bottom = 0.0f;
	if (right > 1.0f) right = 1.0f;
	if (top > 1.0f) top = 1.0f;

	this->m_default_vertex[LEFT_TOP].u = left;
	this->m_default_vertex[RIGHT_TOP].u = right;
	this->m_default_vertex[LEFT_BOTTOM].u = left;
	this->m_default_vertex[RIGHT_BOTTOM].u = right;

	this->m_default_vertex[LEFT_TOP].v = top;
	this->m_default_vertex[RIGHT_TOP].v = top;
	this->m_default_vertex[LEFT_BOTTOM].v = bottom;
	this->m_default_vertex[RIGHT_BOTTOM].v = bottom;
}
