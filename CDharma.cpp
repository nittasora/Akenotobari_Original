#include "CDharma.h"
#include "CInputParameter.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CGameManager.h"

#include "CSingleton.h"

constexpr float ROTATION_SPEED = 10.0f;

void CDharma::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_DHARMA);

	// 判定タグはダルマ
	this->m_tag = TAG_DHARMA;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosition(DHARMA_WIDTH, DHARMA_HEIGHT);

	this->m_collider.width = DHARMA_COLLIDER_WIDTH + (DHARMA_WIDTH * 2.0f);
	this->m_collider.height = DHARMA_COLLIDER_HEIGHT + DHARMA_HEIGHT;
	this->m_collider.center_x = 0.0f;
	this->m_collider.center_y = -(DHARMA_COLLIDER_WIDTH / 2.0f);

	this->m_body_collider.is_active = true;
	this->m_body_collider.center_x = 0.0f;
	this->m_body_collider.center_y = 0.0f;
	this->m_body_collider.width = DHARMA_WIDTH;
	this->m_body_collider.height = DHARMA_HEIGHT;

	this->m_wall_checker.is_active = true;
	this->m_wall_checker.width = DHARMA_COLLIDER_WIDTH;
	this->m_wall_checker.height = DHARMA_COLLIDER_HEIGHT;

	this->m_hole_checker.is_active = true;
	this->m_hole_checker.width = DHARMA_COLLIDER_WIDTH;
	this->m_hole_checker.height = DHARMA_COLLIDER_HEIGHT;

	this->mf_gravity = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_dharma_gravity;

	this->mb_is_ground = false;
	this->mb_next_is_ground = false;
	this->mb_is_tornade = false;
	this->mb_next_is_tornade = false;
	this->mb_is_not_turn = true;
	this->mb_is_hit_border = false;
}

void CDharma::UnInitilize()
{

}

void CDharma::Update()
{
	if (!this->mb_is_not_turn || this->mb_is_hit_border) {
		this->mb_is_left = !this->mb_is_left;
		this->m_velocity.x = 0.0f;
	}

	this->m_velocity.y -= this->mf_gravity;

	this->mb_is_ground = this->mb_next_is_ground;
	this->mb_is_tornade = this->mb_next_is_tornade;

	float max_speed_x = this->mf_max_speed;
	float max_speed_y = this->mf_max_speed * 2.0f;

	if (this->mb_is_tornade) {
		max_speed_x = this->mf_max_speed / 2.0f;
		max_speed_y = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_player_vertical_tornade_speed;
		this->m_velocity.y += this->mf_gravity + 1.6f;

		if (this->mb_is_left) {
			this->m_velocity.x -= this->mf_max_speed / 10.0f;
		}
		else {
			this->m_velocity.x += this->mf_max_speed / 10.0f;
		}
	}
	else if (this->mb_is_ground) {
		if (this->mb_is_left) {
			this->m_velocity.x -= this->mf_max_speed / 20.0f;
		}
		else {
			this->m_velocity.x += this->mf_max_speed / 20.0f;
		}
	}

	//最大速度をこえないようにする
	if (this->m_velocity.x > max_speed_x)this->m_velocity.x = max_speed_x;
	if (this->m_velocity.x < -max_speed_x)this->m_velocity.x = -max_speed_x;
	if (this->m_velocity.y > max_speed_y)this->m_velocity.y = max_speed_y;
	if (this->m_velocity.y < -max_speed_y)this->m_velocity.y = -max_speed_y;

	this->m_transform.angle.z -= (this->m_velocity.x / this->mf_max_speed) * ROTATION_SPEED;
	if (this->m_transform.angle.z > 360.0f) this->m_transform.angle.z -= 360.0f;
	if (this->m_transform.angle.z < 0.0f) this->m_transform.angle.z += 360.0f;

	if (this->mb_is_left) {
		this->m_wall_checker.center_x = -((DHARMA_COLLIDER_WIDTH + (DHARMA_WIDTH * 1.5f)) / 2.0f);
		this->m_wall_checker.center_y = 0.0f;
		this->m_hole_checker.center_x = -((DHARMA_COLLIDER_WIDTH + (DHARMA_WIDTH * 1.5f)) / 2.0f);
		this->m_hole_checker.center_y = -((DHARMA_COLLIDER_HEIGHT + (DHARMA_HEIGHT * 1.5f)) / 2.0f);
	}
	else {
		this->m_wall_checker.center_x = (DHARMA_COLLIDER_WIDTH + (DHARMA_WIDTH * 1.5f)) / 2.0f;
		this->m_wall_checker.center_y = 0.0f;
		this->m_hole_checker.center_x = (DHARMA_COLLIDER_WIDTH + (DHARMA_WIDTH * 1.5f)) / 2.0f;
		this->m_hole_checker.center_y = -((DHARMA_COLLIDER_HEIGHT + (DHARMA_HEIGHT * 1.5f)) / 2.0f);
	}

	// 移動更新
	this->m_change.y += this->m_velocity.y;
	this->m_change.x += this->m_velocity.x;

	// 設置判定を切る
	this->mb_next_is_ground = false;
	this->mb_next_is_tornade = false;
	this->mb_is_not_turn = false;
	this->mb_is_hit_border = false;
}

void CDharma::Draw()
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

	for (size_t i = 0; i < SQUARE_VERTEX_NUM; i++)
	{
		calculate_verpos[i].u = 1.0f - calculate_verpos[i].u;
	}

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

void CDharma::FakeFoldDraw()
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

	for (size_t i = 0; i < SQUARE_VERTEX_NUM; i++)
	{
		calculate_verpos[i].u = 1.0f - calculate_verpos[i].u;
	}

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

void CDharma::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
	ObjectTag other_tag = other->GetTag();

	if (other_tag == TAG_DHARMA) {
		return;
	}

	{
		Collider obj_collider = this->m_body_collider;

		obj_collider.center_x += (this->m_transform.position.x + this->m_change.x);
		obj_collider.center_y += (this->m_transform.position.y + this->m_change.y);

		obj_collider.width *= this->m_transform.scale.x;
		obj_collider.height *= this->m_transform.scale.y;

		// それぞれのコライダーを取得
		Collider other_collider = other->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			switch (other_tag)
			{
			case TAG_PLAYER:
				other->Damage();
				break;
			case TAG_RED_BLOCK:
			case TAG_BLUE_BLOCK:
				// 設置判定
				if (!other->IsActive()) {
					break;
				}

				if (hit.distance_y > 0) {
					this->mb_next_is_ground = true;
					this->m_velocity.y = 0.0f;
				}
				if (hit.distance_x != 0.0f && !this->mb_is_ground) {
					this->m_velocity.x = 0.0f;
				}
				this->m_change.x += hit.distance_x;
				this->m_change.y += hit.distance_y;
				break;
			case TAG_LION_BOTTOM:
			case TAG_BORDER_PAGE:
			case TAG_BORDER_LEFT:
			case TAG_BORDER_RIGHT:
			case TAG_BORDER_TOP:
			case TAG_MAPCHIP:
			case TAG_CANNON:
				// 設置判定
				if (hit.distance_y > 0) {
					this->mb_next_is_ground = true;
					this->m_velocity.y = 0.0f;
				}
				if (hit.distance_x != 0.0f && !this->mb_is_ground) {
					this->m_velocity.x = 0.0f;
				}
				this->m_change.x += hit.distance_x;
				this->m_change.y += hit.distance_y;
				break;
			case TAG_TORNADO:
				this->mb_next_is_tornade = true;
				break;
			case TAG_LION_TOP:
				// 設置判定
				if (hit.distance_y > 0) {
					this->mb_next_is_ground = true;
					this->m_velocity.y = 0.0f;
				}
				if (hit.distance_x != 0.0f && !this->mb_is_ground) {
					this->m_velocity.x = 0.0f;
				}
				this->m_change.x += hit.distance_x;
				if (!this->mb_is_ground && hit.distance_y < 0) this->m_change.y += hit.distance_y;
				break;
			default:
				break;
			}

			hit.distance_x *= -1.0f;
			hit.distance_y *= -1.0f;
			other->OnCollision(&hit, this);
		}

	}

	if (!this->mb_is_tornade && this->mb_is_ground) {
		Collider obj_collider = this->m_hole_checker;

		obj_collider.center_x += (this->m_transform.position.x + this->m_change.x);
		obj_collider.center_y += (this->m_transform.position.y + this->m_change.y);

		obj_collider.width *= this->m_transform.scale.x;
		obj_collider.height *= this->m_transform.scale.y;

		// それぞれのコライダーを取得
		Collider other_collider = other->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			switch (other_tag)
			{
			case TAG_RED_BLOCK:
			case TAG_BLUE_BLOCK:
				if (!other->IsActive()) {
					break;
				}
				this->mb_is_not_turn = true;
				break;
			case TAG_LION_BOTTOM:
			case TAG_BORDER_PAGE:
			case TAG_MAPCHIP:
			case TAG_CANNON:
				this->mb_is_not_turn = true;
				break;
			default:
				break;
			}
		}
	}
	else {
		this->mb_is_not_turn = true;
	}

	if (this->mb_is_not_turn) {
		Collider obj_collider = this->m_wall_checker;

		obj_collider.center_x += (this->m_transform.position.x + this->m_change.x);
		obj_collider.center_y += (this->m_transform.position.y + this->m_change.y);

		obj_collider.width *= this->m_transform.scale.x;
		obj_collider.height *= this->m_transform.scale.y;

		// それぞれのコライダーを取得
		Collider other_collider = other->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			switch (other_tag)
			{
			case TAG_RED_BLOCK:
			case TAG_BLUE_BLOCK:
				if (!other->IsActive()) {
					break;
				}
				this->mb_is_not_turn = false;
				break;
			case TAG_LION_BOTTOM:
			case TAG_BORDER_PAGE:
			case TAG_MAPCHIP:
			case TAG_CANNON:
			case TAG_LION_TOP:
				this->mb_is_not_turn = false;
				break;
			case TAG_BORDER_LEFT:
			case TAG_BORDER_RIGHT:
				this->mb_is_hit_border = true;
				break;
			default:
				break;
			}
		}

	}


}

void CDharma::SetData(DharmaData * data)
{
	if (data->mf_max_speed > DEFAULT_INPUT_VALUE) this->mf_max_speed = data->mf_max_speed;
	else this->mf_max_speed = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_dharma_max_speed;

	this->mb_is_left = data->mb_is_left;
}
