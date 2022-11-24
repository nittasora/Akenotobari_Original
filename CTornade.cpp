#include "CTornade.h"
#include "CTimer.h"
#include "CInput.h"

#include "CSingleton.h"

void CTornade::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_TORNADO);

	// 判定タグは竜巻
	this->m_tag = TAG_TORNADO;

	// アニメーション再生数は無し
	this->mi_anim_count = 0;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosAndColl(TORNADE_WIDTH, TORNADE_HEIGHT);
}

void CTornade::UnInitilize()
{
}

void CTornade::Update()
{
	// 経過時間の計算
	this->mf_anim_time += CSingleton <CTimer>::GetInstance()->GetDeltaTime();

	// 時間のコピー
	float tmp_time = this->mf_anim_time;

	// アニメーションのコマが決定したかどうか
	bool is_hit = false;

	// 経過時間からアニメーションのコマを割り出す
	for (size_t i = 0; i < this->m_sprites[this->m_current_sprite].anim_max; i++)
	{
		// 経過時間が次のコマまでの時間より小さい -> そのコマで決定
		if (tmp_time < this->m_sprites[this->m_current_sprite].count_check) {

			// アニメーションのコマが決定しました
			is_hit = true;

			// アニメーションのコマ計算
			this->m_anim.u = static_cast<float>(static_cast<int>(i) % this->m_sprites[this->m_current_sprite].slice_x);
			this->m_anim.v = static_cast<float>(static_cast<int>(i) / this->m_sprites[this->m_current_sprite].slice_x);
			break;
		}
		else {
			// 経過時間が次のコマまでの時間より大きい
			tmp_time -= this->m_sprites[this->m_current_sprite].count_check;
		}
	}

	if (!is_hit) {
		this->mf_anim_time = tmp_time;
		this->m_anim.u = 0.0f;
		this->m_anim.v = 0.0f;
	}
}

