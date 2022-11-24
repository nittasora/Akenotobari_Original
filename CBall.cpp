#include "CBall.h"

constexpr float ROTATION_SPEED = 10.0f;

void CBall::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_BALL);

	// 判定タグはダルマ
	this->m_tag = TAG_BALL;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosAndColl(BALL_WIDTH, BALL_HEIGHT);
}

void CBall::UnInitilize()
{

}

void CBall::Update()
{
	if (!(this->m_flg & OBJ_FLG_ACTIVE)) return;

	float max_speed_x = this->mf_max_speed;
	float max_speed_y = this->mf_max_speed;

	this->m_velocity.x += this->dir_x * (this->mf_max_speed / 10.0f);
	this->m_velocity.y += this->dir_y * (this->mf_max_speed / 10.0f);

	//this->m_velocity.x += (this->mf_max_speed / 10.0f);

	if (this->m_velocity.x > max_speed_x)this->m_velocity.x = max_speed_x;
	if (this->m_velocity.x < -max_speed_x)this->m_velocity.x = -max_speed_x;
	if (this->m_velocity.y > max_speed_y)this->m_velocity.y = max_speed_y;
	if (this->m_velocity.y < -max_speed_y)this->m_velocity.y = -max_speed_y;

	if (this->m_velocity.x != 0.0f) {
		this->m_transform.angle.z -= (this->m_velocity.x / this->mf_max_speed) * ROTATION_SPEED;
		if (this->m_transform.angle.z > 360.0f) this->m_transform.angle.z -= 360.0f;
		if (this->m_transform.angle.z < 0.0f) this->m_transform.angle.z += 360.0f;
	}
	if (this->m_velocity.y != 0.0f) {
		this->m_transform.angle.z -= (this->m_velocity.y / this->mf_max_speed) * ROTATION_SPEED;
		if (this->m_transform.angle.z > 360.0f) this->m_transform.angle.z -= 360.0f;
		if (this->m_transform.angle.z < 0.0f) this->m_transform.angle.z += 360.0f;
	}

	// 移動更新
	this->m_change.x += this->m_velocity.x;
	this->m_change.y += this->m_velocity.y;
}

void CBall::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
	ObjectTag other_tag = other->GetTag();

	if (other_tag == TAG_LION_TOP) {
		return;
	}

	switch (other_tag)
	{
	case TAG_RED_BLOCK:
	case TAG_BLUE_BLOCK:
		if (!other->IsActive()) {
			break;
		}
		this->Destroy();
		break;
	case TAG_MAPCHIP:
	case TAG_LION_BOTTOM:
	case TAG_BORDER_LEFT:
	case TAG_BORDER_RIGHT:
	case TAG_BORDER_TOP:
	case TAG_BORDER_BOTTOM:
		this->Destroy();
		break;
	case TAG_BORDER_PAGE:
		this->m_change.x += collider_res->distance_x;
		this->m_change.y += collider_res->distance_y;
		break;
	default:
		break;
	}
}

void CBall::Destroy()
{
	this->m_flg &= ~OBJ_FLG_ACTIVE;
	this->m_transform.position.x = -1000.0f;
}

void CBall::Create(float max_speed, float angle_z, FRONT_BACK fb)
{
	this->m_flg |= OBJ_FLG_ACTIVE;
	this->mf_max_speed = max_speed;

	this->dir_x = 0.0f;
	this->dir_y = 0.0f;

	this->m_change.x = 0.0f;
	this->m_change.y = 0.0f;

	this->m_velocity.x = 0.0f;
	this->m_velocity.y = 0.0f;

	if (fb == IS_FRONT) {
		this->m_anim.u = 0.0f;
	}
	else {
		this->m_anim.u = 1.0f;
	}

	if (angle_z <= 45.0f) this->dir_y = 1.0f;
	else if (angle_z <= 135.0f) this->dir_x = -1.0f;
	else if (angle_z <= 225.0f) this->dir_y = -1.0f;
	else this->dir_x = 1.0f;
}
