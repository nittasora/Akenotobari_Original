#include "CLionBottom.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CGameManager.h"

#include "CSingleton.h"

void CLionBottom::Initilize()
{
	this->m_tag = TAG_LION_BOTTOM;
	this->AddTexture(SPRITE_LION_BOTTOM);
	this->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);
	//this->m_anim.v = 1.0f;
}

void CLionBottom::UnInitilize()
{

}

void CLionBottom::Draw()
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
	Matrix4_4 rotation_x = UserMath::GetRotationXMatrix(this->m_transform.angle.x);
	Matrix4_4 rotation_y = UserMath::GetRotationYMatrix(this->m_transform.angle.y);
	Matrix4_4 rotation_z = UserMath::GetRotationZMatrix(this->m_transform.angle.z);

	Matrix4_4 camera_matrix = game_manager->Get2DCameraMatrix();

	Matrix4_4 all_matrix;
	all_matrix = UserMath::MutiplyMatrix(&rotation_z, &scale);
	all_matrix = UserMath::MutiplyMatrix(&rotation_x, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&rotation_y, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&tarnslate, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&camera_matrix, &all_matrix);

	for (size_t i = 0; i < SQUARE_VERTEX_NUM; i++)
	{
		Matrix4_1 mat = {
			this->m_default_vertex[i].x, this->m_default_vertex[i].y, this->m_default_vertex[i].z, 1.0f
		};

		mat = UserMath::MutiplyMatrix(&all_matrix, &mat);

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

void CLionBottom::FakeFoldDraw()
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
	Matrix4_4 rotation_x = UserMath::GetRotationXMatrix(this->m_transform.angle.x);
	Matrix4_4 rotation_y = UserMath::GetRotationYMatrix(this->m_transform.angle.y);
	Matrix4_4 rotation_z = UserMath::GetRotationZMatrix(this->m_transform.angle.z);

	Matrix4_4 axis_translate_true = UserMath::GetTranslationMatrix(this->m_axis.x, this->m_axis.y, this->m_axis.z);
	Matrix4_4 axis_translate_false = UserMath::GetTranslationMatrix(-this->m_axis.x, -this->m_axis.y, -this->m_axis.z);
	Matrix4_4 fold_rotation_y = UserMath::GetRotationYMatrix(this->mf_fold_angle_y);

	Matrix4_4 camera_matrix = game_manager->Get2DCameraMatrix();

	Matrix4_4 all_matrix;
	all_matrix = UserMath::MutiplyMatrix(&rotation_z, &scale);
	all_matrix = UserMath::MutiplyMatrix(&rotation_x, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&rotation_y, &all_matrix);
	if (this->mb_not_rotate) {
		auto temp_matrix = UserMath::GetRotationYMatrix(-this->mf_fold_angle_y);
		all_matrix = UserMath::MutiplyMatrix(&temp_matrix, &all_matrix);
	}
	all_matrix = UserMath::MutiplyMatrix(&tarnslate, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&axis_translate_false, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&fold_rotation_y, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&axis_translate_true, &all_matrix);
	all_matrix = UserMath::MutiplyMatrix(&camera_matrix, &all_matrix);

	for (size_t i = 0; i < SQUARE_VERTEX_NUM; i++)
	{
		Matrix4_1 mat = {
			this->m_default_vertex[i].x, this->m_default_vertex[i].y, this->m_default_vertex[i].z, 1.0f
		};

		mat = UserMath::MutiplyMatrix(&all_matrix, &mat);

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

void CLionBottom::SetDefaultUV(float left, float right, float bottom, float top)
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
