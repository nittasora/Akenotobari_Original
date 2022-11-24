#include "CPlayerRunImage.h"
#include "CSingleton.h"
#include "CTimer.h"
#include "CInputParameter.h"

void CPlayerRunImage::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_PLAYER_RUN00);
	this->AddTexture(SPRITE_PLAYER_RUN01);

	// 初期画像は瞬き無し
	this->m_current_sprite = PA_RUN00;

	// 判定タグはプレイヤー
	this->m_tag = TAG_PLAYER;

	// アニメーション再生数は無し
	this->mi_anim_count = 0;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	this->m_sprites[PA_RUN00].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN00].anim_max);
	this->m_sprites[PA_RUN01].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN01].anim_max);
}

void CPlayerRunImage::UnInitilize()
{
}

void CPlayerRunImage::Update()
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

	// 現在の画像から分岐します
	switch (this->m_current_sprite)
	{
	case PA_RUN00:
		// アニメーションが最後まで再生されました
		if (!is_hit) {
			// 経過時間を更新
			this->mf_anim_time = tmp_time;

			// アニメーション再生数を増やす。
			this->mi_anim_count++;

			// count_max回やりました
			if (this->mi_anim_count >= 2) {
				// アニメーション再生数をリセット
				this->mi_anim_count = 0;

				// 画像変更
				this->m_current_sprite = PA_RUN01;
			}

			// アニメーションのコマを0にする
			this->m_anim.u = 0.0f;
			this->m_anim.v = 0.0f;
		}
		break;
	case PA_RUN01:
		// アニメーションが最後まで再生されました
		if (!is_hit) {
			// 経過時間を更新
			this->mf_anim_time = tmp_time;

			// アニメーション再生数をリセット
			this->mi_anim_count = 0;

			// 画像変更
			this->m_current_sprite = PA_RUN00;

			// アニメーションのコマを0にする
			this->m_anim.u = 0.0f;
			this->m_anim.v = 0.0f;
		}
		break;
	}
}
