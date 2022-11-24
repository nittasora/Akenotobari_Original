#include "CLionTop.h"
#include "CInputParameter.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CGameManager.h"
#include "CTimer.h"

#include "CSingleton.h"

void CLionTop::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_LION_TOP);

	// 判定タグはリフト
	this->m_tag = TAG_LION_TOP;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosAndColl(LION_WIDTH, LION_HEIGHT);

	this->mf_time = 0.0f;

	this->m_condition = LION_WAIT_DOWN;
	this->m_next_condition = LION_WAIT_DOWN;
}

void CLionTop::UnInitilize()
{
}

void CLionTop::Update()
{
	this->m_condition = this->m_next_condition;

	CTimer* timer = CSingleton<CTimer>::GetInstance();

	switch (this->m_condition)
	{
	case LION_WAIT_DOWN:
		this->m_velocity.y = 0.0f;
		this->mf_time += timer->GetDeltaTime();

		if (this->mf_time >= this->mf_down_wait) {
			this->mf_time = 0.0f;
			this->m_next_condition = LION_DOWN;
		}
		break;
	case LION_DOWN:
		this->m_velocity.y -= this->mf_down_max_speed / 10.0f;
		if (this->m_velocity.y <= -this->mf_down_max_speed) this->m_velocity.y = -this->mf_down_max_speed;
		break;
	case LION_WAIT_UP:
		this->m_velocity.y = 0.0f;
		this->mf_time += timer->GetDeltaTime();

		if (this->mf_time >= this->mf_up_wait) {
			this->mf_time = 0.0f;
			this->m_next_condition = LION_UP;
		}
		break;
	case LION_UP:
		this->m_velocity.y += this->mf_up_max_speed / 10.0f;
		if (this->m_velocity.y >= this->mf_up_max_speed) this->m_velocity.y = this->mf_up_max_speed;
		if (this->m_transform.position.y + this->m_velocity.y >= this->m_start_pos.y) {
			this->m_velocity.y = this->m_start_pos.y - this->m_transform.position.y;
			this->mf_time = 0.0f;
			this->m_next_condition = LION_WAIT_DOWN;
		}
		break;
	default:
		break;
	}

	this->m_change.y += this->m_velocity.y;
	this->m_change.x += this->m_velocity.x;
}

void CLionTop::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
	ObjectTag other_tag = other->GetTag();

	if (other_tag == TAG_LION_TOP) {
		return;
	}

	switch (other_tag)
	{
	case TAG_RED_BLOCK:
	case TAG_BLUE_BLOCK:
		// 設置判定
		if (!other->IsActive()) {
			break;
		}

		if (collider_res->distance_y > 0) {
			this->m_velocity.y = 0.0f;
		}
		this->m_change.y += collider_res->distance_y;
		break;
	case TAG_BORDER_PAGE:
	case TAG_MAPCHIP:
	case TAG_CANNON:
		// 設置判定
		if (collider_res->distance_y > 0) {
			this->m_velocity.y = 0.0f;
		}
		this->m_change.y += collider_res->distance_y;
		break;
	case TAG_LION_BOTTOM:
		// 設置判定
		if (collider_res->distance_y > 0) {
			this->mb_next_is_ground = true;
			this->m_velocity.y = 0.0f;
		}
		this->m_change.y += collider_res->distance_y;

		if (this->m_condition == LION_DOWN) {
			this->mf_time = 0.0f;
			this->m_next_condition = LION_WAIT_UP;
		}
		break;
	case TAG_PLAYER:
	{
		if (this->m_condition == LION_DOWN && collider_res->distance_y < 0) {
			if (other->GetGroundFlg()) {
				this->m_change.y += collider_res->distance_y;
				this->mf_time = 0.0f;
				this->m_next_condition = LION_WAIT_UP;
			}
			else {
				this->m_change.y -= collider_res->distance_y;
			}
		}
	}
	break;
	case TAG_DHARMA:
	{
		if (this->m_condition == LION_DOWN && collider_res->distance_y > 0) {
			if (other->GetGroundFlg()) {
				this->m_change.y += collider_res->distance_y;
				this->mf_time = 0.0f;
				this->m_next_condition = LION_WAIT_UP;
			}
			else {
				this->m_change.y -= collider_res->distance_y;
			}
		}
	}
	break;
	default:
		break;
	}
}

void CLionTop::SetData(LionData * data)
{
	if (data->mf_up_max_speed > DEFAULT_INPUT_VALUE) this->mf_up_max_speed = data->mf_up_max_speed;
	else this->mf_up_max_speed = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_lion_up_max_speed;

	if (data->mf_up_wait > DEFAULT_INPUT_VALUE) this->mf_up_wait = data->mf_up_wait;
	else this->mf_up_wait = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_lion_up_wait;

	if (data->mf_down_max_speed > DEFAULT_INPUT_VALUE) this->mf_down_max_speed = data->mf_down_max_speed;
	else this->mf_down_max_speed = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_lion_down_max_speed;

	if (data->mf_down_wait > DEFAULT_INPUT_VALUE) this->mf_down_wait = data->mf_down_wait;
	else this->mf_down_wait = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_lion_down_wait;

	this->m_start_pos = data->position;

}
