#include "CCannon.h"
#include "CInputParameter.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CGameManager.h"
#include "CStageManager.h"
#include "CTimer.h"

#include "CSingleton.h"

void CCannon::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_CANNON);

	// 判定タグはゴール
	this->m_tag = TAG_CANNON;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosAndColl(CANNON_WIDTH, CANNON_HEIGHT);

	this->mf_time = 0.0f;
}

void CCannon::UnInitilize()
{
}

void CCannon::Update()
{
	CTimer* timer = CSingleton<CTimer>::GetInstance();
	this->mf_time += timer->GetDeltaTime();
	if (this->mf_time >= this->mf_wait) {
		this->mf_time = 0.0f;
		this->m_stage_manager->CreateBall(this->m_transform.position, this->mf_max_speed, this->mf_angle_z, this->m_front_back);
	}
}

void CCannon::SetData(CannonData * data, CStageManager* stage_manager, FRONT_BACK fb)
{
	this->m_stage_manager = stage_manager;
	this->m_front_back = fb;

	if (data->mf_angle_z > DEFAULT_INPUT_VALUE) this->mf_angle_z = data->mf_angle_z;
	else this->mf_angle_z = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_cannon_angle_z;

	if (data->mf_max_speed > DEFAULT_INPUT_VALUE) this->mf_max_speed = data->mf_max_speed;
	else this->mf_max_speed = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_cannon_max_speed;

	if (data->mf_wait > DEFAULT_INPUT_VALUE) this->mf_wait = data->mf_wait;
	else this->mf_wait = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_cannon_wait;
}

