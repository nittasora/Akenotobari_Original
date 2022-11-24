#include "CGameObject.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CGameManager.h"

#include "CSingleton.h"

CGameObject::CGameObject()
{
	HRESULT hr;

	// 頂点バッファ作成
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.ByteWidth = sizeof(ModelVertex) * VERTEX_BUFFER_SIZE * 2;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	hr = CSingleton<CDirect3D>::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, NULL, &this->m_vertex_buffer);

	this->m_default_vertex[LEFT_TOP] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f
	};
	this->m_default_vertex[RIGHT_TOP] = {
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f
	};
	this->m_default_vertex[LEFT_BOTTOM] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f
	};
	this->m_default_vertex[RIGHT_BOTTOM] = {
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f
	};

	this->m_transform = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
	};
	this->m_change = {
		0.0f, 0.0f, 0.0f
	};

	this->m_color = {
		1.0f, 1.0f, 1.0f, 1.0f
	};

	this->m_anim = {
		0.0f, 0.0f
	};

	this->m_collider = {
		0.0f,
		0.0f,
		2.0f,
		2.0f,
		true
	};

	this->m_current_sprite = 0;

	this->mf_anim_time = 0.0f;

	this->m_velocity = { 0.0f, 0.0f };

	this->m_tag = TAG_DEFAULT;
	this->m_flg = 0;
	this->m_flg |= OBJ_FLG_ACTIVE;
	this->m_flg |= OBJ_FLG_DRAW;
}

CGameObject::~CGameObject()
{
	COM_SAFE_RELEASE(this->m_vertex_buffer);
}

void CGameObject::Update()
{
}

void CGameObject::Draw()
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

		//mat = UserMath::MutiplyMatrix(&scale, &mat);
		//mat = UserMath::MutiplyMatrix(&rotation_z, &mat);
		//mat = UserMath::MutiplyMatrix(&rotation_x, &mat);
		//mat = UserMath::MutiplyMatrix(&rotation_y, &mat);
		//mat = UserMath::MutiplyMatrix(&tarnslate, &mat);

		//mat = UserMath::MutiplyMatrix(&camera_matrix, &mat);

		calculate_verpos[i].x = mat.index[0];
		calculate_verpos[i].y = mat.index[1];
		calculate_verpos[i].z = 0.0f;	// 2Dなので使いません

		calculate_verpos[i].u = (this->m_default_vertex[i].u + anim_u) * this->m_sprites[this->m_current_sprite].slice.u;
		calculate_verpos[i].v = (this->m_default_vertex[i].v + anim_v) * this->m_sprites[this->m_current_sprite].slice.v;
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

void CGameObject::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
}

void CGameObject::UpdateTransform()
{
	this->m_transform.position.x += this->m_change.x;
	this->m_transform.position.y += this->m_change.y;
	this->m_transform.position.z += this->m_change.z;
	this->m_change.y = 0.0f;
	this->m_change.x = 0.0f;
}

bool CGameObject::GetGroundFlg()
{
	return false;
}

ObjectTag CGameObject::GetTag()
{
	return this->m_tag;
}

obj_flg CGameObject::GetFlg()
{
	return this->m_flg;
}

Transform CGameObject::GetTransform()
{
	return this->m_transform;
}

Color CGameObject::GetColor()
{
	return this->m_color;
}

UV CGameObject::GetAnim()
{
	return this->m_anim;
}

Collider CGameObject::GetCollider()
{
	return this->m_collider;
}

Collider CGameObject::GetConvertedCollider()
{
	Collider obj_collider = this->m_collider;

	obj_collider.center_x += (this->m_transform.position.x + this->m_change.x);
	obj_collider.center_y += (this->m_transform.position.y + this->m_change.y);

	obj_collider.width *= this->m_transform.scale.x;
	obj_collider.height *= this->m_transform.scale.y;

	return obj_collider;
}

Velocity CGameObject::GetVelocity()
{
	return this->m_velocity;
}

void CGameObject::SetTag(const ObjectTag * tag)
{
	this->m_tag = *tag;
}

void CGameObject::SetFlg(const obj_flg * flg)
{
	this->m_flg = *flg;
}

void CGameObject::SetTransform(const Transform * transform)
{
	this->m_transform = *transform;
}

void CGameObject::SetColor(const Color * color)
{
	this->m_color = *color;
}

void CGameObject::SetAnim(const UV * anim)
{
	this->m_anim = *anim;
}

void CGameObject::SetCollider(const Collider * collider)
{
	this->m_collider = *collider;
}

void CGameObject::SetVelocity(const Velocity * velocity)
{
	this->m_velocity = *velocity;
}

bool CGameObject::IsActive()
{
	return this->m_flg & OBJ_FLG_ACTIVE;
}

void CGameObject::IsActive(bool is_active)
{
	if (is_active) {
		this->m_flg |= OBJ_FLG_ACTIVE;
	}
	else {
		this->m_flg &= ~OBJ_FLG_ACTIVE;
	}
}

void CGameObject::AddTexture(int id)
{
	SpriteData sprite;
	CSingleton<CSpriteFactory>::GetInstance()->CreateSprite(&sprite, id);
	this->m_sprites.emplace_back(sprite);
	this->m_current_sprite = this->m_sprites.size() - 1;
}

void CGameObject::ChangeTexture(int id)
{
	CSingleton<CSpriteFactory>::GetInstance()->CreateSprite(&this->m_sprites[this->m_current_sprite], id);
}

SpriteData CGameObject::GetTexture()
{
	return this->m_sprites[this->m_current_sprite];
}

void CGameObject::SetTexture(const SpriteData * sprite)
{
	this->m_sprites[this->m_current_sprite] = *sprite;
}

void CGameObject::SetDefaultPosition(float width, float height)
{
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	this->m_default_vertex[LEFT_TOP] = {
		-half_width,   half_height, 0.0f, 0.0f, 0.0f
	};
	this->m_default_vertex[RIGHT_TOP] = {
		 half_width,   half_height, 0.0f, 1.0f, 0.0f
	};
	this->m_default_vertex[LEFT_BOTTOM] = {
		-half_width, -half_height, 0.0f, 0.0f, 1.0f
	};
	this->m_default_vertex[RIGHT_BOTTOM] = {
		 half_width, -half_height, 0.0f, 1.0f, 1.0f
	};
}

void CGameObject::SetDefaultPosAndColl(float width, float height)
{
	this->SetDefaultPosition(width, height);
	this->m_collider.width = width;
	this->m_collider.height = height;
}

void CGameObject::Damage()
{
}

