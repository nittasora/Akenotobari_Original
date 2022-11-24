#include "CPlayer.h"
#include "CTimer.h"
#include "CInput.h"
#include "CInputParameter.h"
#include "CGameManager.h"
#include "CXAudio2.h"

#include "CSingleton.h"

void CPlayer::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_PLAYER_IDLE_00);
	this->AddTexture(SPRITE_PLAYER_IDLE_01);
	this->AddTexture(SPRITE_PLAYER_IDLETORUN);
	this->AddTexture(SPRITE_PLAYER_RUN00);
	this->AddTexture(SPRITE_PLAYER_RUN01);
	this->AddTexture(SPRITE_PLAYER_RUNTOIDLE);
	//this->AddTexture(SPRITE_PLAYER_JUMP_UP);
	this->AddTexture(SPRITE_PLAYER_JUMP);
	this->AddTexture(SPRITE_PLAYER_JUMP_DOWN);
	this->AddTexture(SPRITE_PLAYER_HIT);
	this->AddTexture(SPRITE_PLAYER_GAMEOVER);

	// 初期画像は待機
	this->m_current_sprite = PA_IDLE_00;

	// 判定タグはプレイヤー
	this->m_tag = TAG_PLAYER;

	// アニメーション再生数は無し
	this->mi_anim_count = 0;

	// 設置判定を切る
	this->mb_is_ground = false;
	this->mb_next_is_ground = false;
	this->mb_is_tornade = false;
	this->mb_next_is_tornade = false;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosition(PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);
	this->m_collider.width = PLAYER_COLLIDER_WIDTH;
	this->m_collider.height = PLAYER_COLLIDER_HEIGHT;
	this->m_collider.center_y = -4.0f;

	this->mf_current_invincible_time = 0.0f;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	this->mf_horizontal_max_speed = cip_data.M_DATA.mf_player_horizontal_max_speed;
	this->mf_vertical_max_speed = cip_data.M_DATA.mf_player_vertical_max_speed;
	this->mf_acceleration_ground = cip_data.M_DATA.mf_player_acceleration_ground;
	this->mf_acceleration_air = cip_data.M_DATA.mf_player_acceleration_air;
	this->mf_deceleration_ground = cip_data.M_DATA.mf_player_deceleration_ground;
	this->mf_deceleration_air = cip_data.M_DATA.mf_player_deceleration_air;
	this->mf_vertical_jump_power = cip_data.M_DATA.mf_player_vertical_jump_power;
	this->mf_horizontal_jump_power = cip_data.M_DATA.mf_player_horizontal_jump_power;
	this->mf_horizontal_tornade_speed = cip_data.M_DATA.mf_player_horizontal_tornade_speed;
	this->mf_vertical_tornade_speed = cip_data.M_DATA.mf_player_vertical_tornade_speed;
	this->mf_gravity_up = cip_data.M_DATA.mf_player_gravity_up;
	this->mf_gravity_down = cip_data.M_DATA.mf_player_gravity_down;
	this->mf_invincible_time = cip_data.M_DATA.mf_player_invincible_time;
	this->mf_flashing_time = cip_data.M_DATA.mf_player_flashing_time;

	this->m_sprites[PA_IDLE_00].count_check = cip_data.M_DATA.mf_player_idle_loop_time / static_cast<float>(this->m_sprites[PA_IDLE_00].anim_max);
	this->m_sprites[PA_IDLE_01].count_check = cip_data.M_DATA.mf_player_idle_loop_time / static_cast<float>(this->m_sprites[PA_IDLE_01].anim_max);
	this->m_sprites[PA_IDLETORUN].count_check = cip_data.M_DATA.mf_player_idle_to_run_loop_time / static_cast<float>(this->m_sprites[PA_IDLETORUN].anim_max);
	this->m_sprites[PA_RUN00].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN00].anim_max);
	this->m_sprites[PA_RUN01].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN01].anim_max);
	this->m_sprites[PA_RUNTOIDLE].count_check = cip_data.M_DATA.mf_player_run_to_idle_loop_time / static_cast<float>(this->m_sprites[PA_RUNTOIDLE].anim_max);
	//this->m_sprites[PA_JUMP_UP].count_check = cip_data.M_DATA.mf_player_jump_up_loop_time / static_cast<float>(this->m_sprites[PA_JUMP_UP].anim_max);
	this->m_sprites[PA_JUMP].count_check = cip_data.M_DATA.mf_player_jump_loop_time / static_cast<float>(this->m_sprites[PA_JUMP].anim_max);
	this->m_sprites[PA_JUMP_DOWN].count_check = cip_data.M_DATA.mf_player_jump_down_loop_time / static_cast<float>(this->m_sprites[PA_JUMP_DOWN].anim_max);
	this->m_sprites[PA_HIT].count_check = cip_data.M_DATA.mf_player_damage_loop_time / static_cast<float>(this->m_sprites[PA_HIT].anim_max);
	this->m_sprites[PA_OVER].count_check = cip_data.M_DATA.mf_player_down_loop_time / static_cast<float>(this->m_sprites[PA_OVER].anim_max);
}

void CPlayer::UnInitilize()
{
}

void CPlayer::Update()
{
	// Sound
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();

	// 入力
	CInput* controller = CSingleton<CInput>::GetInstance();

	this->Animation();

	this->mb_is_ground = this->mb_next_is_ground;
	this->mb_is_tornade = this->mb_next_is_tornade;

	// 重力計算
	if (this->m_velocity.y > 0.0f) this->m_velocity.y -= this->mf_gravity_up;
	else this->m_velocity.y -= this->mf_gravity_down;

	//計算処理
	float input_x = controller->GetAxisRawX(true);

	float acceleration_x = this->mf_acceleration_ground;
	float deceleration_x = this->mf_deceleration_ground;
	if (!this->mb_is_ground) {
		acceleration_x = this->mf_acceleration_air;
		deceleration_x = this->mf_deceleration_air;
	}

	if (input_x == 0.0f) {
		if (controller->GetKeyPress('A') || controller->GetButtonPress(XI_BUTTON_LEFT)) {
			this->m_velocity.x -= acceleration_x;
		}
		if (controller->GetKeyPress('D') || controller->GetButtonPress(XI_BUTTON_RIGHT)) {
			this->m_velocity.x += acceleration_x;
		}
	}
	else {
		this->m_velocity.x += input_x * acceleration_x;
	}

	if (controller->GetButtonTrigger(XI_BUTTON_A) && this->mb_is_ground) {
		audio->Start(SOUND_LABEL_SE_JUMP);
		this->m_velocity.y = this->mf_vertical_jump_power;
		//this->m_velocity.x /= this->mf_horizontal_jump_power;
		this->mb_is_ground = false;

		//this->SetAnimation(PA_JUMP_UP, 0.0f, PLAYER_JUMP_UP_WIDTH, PLAYER_JUMP_UP_HEIGHT);

		//this->m_velocity.y = 4.2f;
		//this->m_velocity.x /= 2.6f;
		//this->mb_is_ground = false;
	}
	if (controller->GetKeyTrigger(VK_SPACE) && this->mb_is_ground) {
		audio->Start(SOUND_LABEL_SE_JUMP);
		this->m_velocity.y = this->mf_vertical_jump_power;
		//this->m_velocity.x /= this->mf_horizontal_jump_power;
		this->mb_is_ground = false;

		//this->SetAnimation(PA_JUMP_UP, 0.0f, PLAYER_JUMP_UP_WIDTH, PLAYER_JUMP_UP_HEIGHT);

		//this->m_velocity.y = 4.2f;
		//this->m_velocity.x /= 2.6f;
		//this->mb_is_ground = false;
	}

	//最大速度
	float max_horizontal_velocity = this->mf_horizontal_max_speed;
	float max_vertical_velocity = this->mf_vertical_max_speed;
	if (this->mb_is_tornade) {
		if (this->m_velocity.y > 0.0f) this->m_velocity.y += 0.6f;
		else this->m_velocity.y += 1.8f;

		if (controller->GetKeyPress(VK_SPACE)) {
			this->m_velocity.y = this->mf_vertical_jump_power;
		}
		else if (controller->GetButtonPress(XI_BUTTON_A)) {
			this->m_velocity.y = this->mf_vertical_jump_power;
		}
		else {
			max_horizontal_velocity = this->mf_horizontal_tornade_speed;
			max_vertical_velocity = this->mf_vertical_tornade_speed;
		}
	}
	else if (!this->mb_is_ground) {
		max_horizontal_velocity = this->mf_horizontal_jump_power;
	}

	// 反転処理
	if (this->m_velocity.x > 0.0f) {
		this->m_transform.angle.y = 0.0f;
	}
	if (this->m_velocity.x < 0.0f) {
		this->m_transform.angle.y = 180.0f;
	}

	// 減速処理
	if (this->m_velocity.x > 0.0f) {  // ＋の値の時
		this->m_velocity.x -= deceleration_x;

		if (this->m_velocity.x < 0.0f) { // 減速しすぎてマイナスになった
			this->m_velocity.x = 0.0f;  // 停止させる
		}
	}
	else if (this->m_velocity.x < 0.0f) { // ーの値の時
		this->m_velocity.x += deceleration_x;

		if (this->m_velocity.x > 0.0f) { // 減速しすぎてプラスになった
			this->m_velocity.x = 0.0f;  // 停止させる
		}
	}

	//最大速度をこえないようにする
	if (this->m_velocity.x > max_horizontal_velocity)this->m_velocity.x = max_horizontal_velocity;
	if (this->m_velocity.x < -max_horizontal_velocity)this->m_velocity.x = -max_horizontal_velocity;
	if (this->m_velocity.y > max_vertical_velocity)this->m_velocity.y = max_vertical_velocity;
	if (this->m_velocity.y < -max_vertical_velocity)this->m_velocity.y = -max_vertical_velocity;

	// 移動更新
	this->m_change.y += this->m_velocity.y;
	//if (this->m_current_sprite != PA_JUMP_UP && this->m_current_sprite != PA_JUMP_DOWN) {
	//	this->m_change.x += this->m_velocity.x;
	//}
	this->m_change.x += this->m_velocity.x;

	// 設置判定を切る
	this->mb_next_is_ground = false;
	this->mb_next_is_tornade = false;

	if (this->mf_current_invincible_time > 0) {
		int flash_count = static_cast<int>(this->mf_current_invincible_time / this->mf_flashing_time) % 2;
		if (flash_count == 0) {
			this->m_color.a = 0.0f;
		}
		else {
			this->m_color.a = 1.0f;
		}
		this->mf_current_invincible_time -= CSingleton <CTimer>::GetInstance()->GetDeltaTime();
	}
	else {
		this->m_color.a = 1.0f;
	}
}

void CPlayer::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
	ObjectTag other_tag = other->GetTag();
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();

	switch (other_tag)
	{
	case TAG_RED_BLOCK:
	case TAG_BLUE_BLOCK:
		// 設置判定
		if (!other->IsActive()) {
			break;
		}

		if (collider_res->distance_y > 0) {
			this->mb_next_is_ground = true;
			this->m_velocity.y = 0.0f;
		}
		if (collider_res->distance_x != 0.0f && !this->mb_is_ground) {
			this->m_velocity.x = 0.0f;
		}
		this->m_change.x += collider_res->distance_x;
		this->m_change.y += collider_res->distance_y;
		break;
	case TAG_LION_BOTTOM:
	case TAG_BORDER_PAGE:
	case TAG_MAPCHIP:
	case TAG_CANNON:
	case TAG_BORDER_TOP:
		// 設置判定
		if (collider_res->distance_y > 0) {
			this->mb_next_is_ground = true;
			this->m_velocity.y = 0.0f;
		}
		if (collider_res->distance_x != 0.0f && !this->mb_is_ground) {
			this->m_velocity.x = 0.0f;
		}
		this->m_change.x += collider_res->distance_x;
		this->m_change.y += collider_res->distance_y;
		break;
	case TAG_TORNADO:
		this->mb_next_is_tornade = true;
		break;
	case TAG_LION_TOP:
		// 設置判定
		if (collider_res->distance_y > 0) {
			this->mb_next_is_ground = true;
			this->m_velocity.y = 0.0f;
		}
		else if (collider_res->distance_y < 0) {
			this->Damage();
		}
		if (collider_res->distance_x != 0.0f && !this->mb_is_ground) {
			this->m_velocity.x = 0.0f;
		}
		this->m_change.x += collider_res->distance_x;
		if (collider_res->distance_y > 0) this->m_change.y += collider_res->distance_y;
		else if (!this->mb_next_is_ground) this->m_change.y += collider_res->distance_y;
		break;
	case TAG_DHARMA:
		//this->Damage();
		break;
	case TAG_BALL:
		this->Damage();
		break;
	case TAG_PIECE_1:
	{
		game_manager->SetGetPiece(0, true);
		Collider collider = other->GetCollider();
		collider.is_active = false;
		other->SetCollider(&collider);
		obj_flg flg = 0;
		other->SetFlg(&flg);
		if (game_manager->CheckGetPiece(PIECE_NUM)) audio->Start(SOUND_LABEL_SE_COMPLETE_PIECE);
		else audio->Start(SOUND_LABEL_SE_GET_PIECE);
	}
	break;
	case TAG_PIECE_2:
	{
		game_manager->SetGetPiece(1, true);
		Collider collider = other->GetCollider();
		collider.is_active = false;
		other->SetCollider(&collider);
		obj_flg flg = 0;
		other->SetFlg(&flg);
		if (game_manager->CheckGetPiece(PIECE_NUM)) audio->Start(SOUND_LABEL_SE_COMPLETE_PIECE);
		else audio->Start(SOUND_LABEL_SE_GET_PIECE);
	}
	break;
	case TAG_PIECE_3:
	{
		game_manager->SetGetPiece(2, true);
		Collider collider = other->GetCollider();
		collider.is_active = false;
		other->SetCollider(&collider);
		obj_flg flg = 0;
		other->SetFlg(&flg);
		if (game_manager->CheckGetPiece(PIECE_NUM)) audio->Start(SOUND_LABEL_SE_COMPLETE_PIECE);
		else audio->Start(SOUND_LABEL_SE_GET_PIECE);
	}
	break;
	case TAG_GOAL:
		game_manager->SetGoal(true);
		break;
	default:
		break;
	}
}

void CPlayer::Animation()
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
	case PA_IDLE_00:
		// アニメーションが最後まで再生されました
		if (!is_hit) {

			// 瞬きセット
			this->SetAnimation(PA_IDLE_01, 2, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);

		}

		// 速度が0じゃない = 走るモーションへ
		if (this->m_velocity.x != 0.0f) {

			// 走り出しセット
			this->SetAnimation(PA_IDLETORUN, 0.0f, PLAYER_IDLE_TO_RUN_WIDTH, PLAYER_IDLE_TO_RUN_HEIGHT);
		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_IDLE_01:
		// アニメーションが最後まで再生されました
		if (!is_hit) {

			// 瞬き無しセット
			this->SetAnimation(PA_IDLE_00, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);

		}

		// 速度が0じゃない = 走るモーションへ
		if (this->m_velocity.x != 0.0f) {

			// 走り出しセット
			this->SetAnimation(PA_IDLETORUN, 0.0f, PLAYER_IDLE_TO_RUN_WIDTH, PLAYER_IDLE_TO_RUN_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_IDLETORUN:
		// アニメーションが最後まで再生されました
		if (!is_hit) {

			// 走りセット
			this->SetAnimation(PA_RUN00, tmp_time, PLAYER_RUN_WIDTH, PLAYER_RUN_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_RUN00:
		// アニメーションが最後まで再生されました
		if (!is_hit) {

			// 瞬きセット
			this->SetAnimation(PA_RUN01, 2, tmp_time, PLAYER_RUN_WIDTH, PLAYER_RUN_HEIGHT);

		}

		// 速度が0 = 止まるモーションへ
		if (this->m_velocity.x == 0.0f) {

			// 走り終わりセット
			this->SetAnimation(PA_RUNTOIDLE, 0.0f, PLAYER_RUN_TO_IDLE_WIDTH, PLAYER_RUN_TO_IDLE_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_RUN01:
		// アニメーションが最後まで再生されました
		if (!is_hit) {

			// 瞬き無しセット
			this->SetAnimation(PA_RUN00, tmp_time, PLAYER_RUN_WIDTH, PLAYER_RUN_HEIGHT);

		}

		// 速度が0 = 止まるモーションへ
		if (this->m_velocity.x == 0.0f) {

			// 走り終わりセット
			this->SetAnimation(PA_RUNTOIDLE, 0.0f, PLAYER_RUN_TO_IDLE_WIDTH, PLAYER_RUN_TO_IDLE_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_RUNTOIDLE:
		// アニメーションが最後まで再生されました
		if (!is_hit) {

			// 瞬き無しセット
			this->SetAnimation(PA_IDLE_00, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
		//case PA_JUMP_UP:
		//	// アニメーションが最後まで再生されました
		//	if (!is_hit) {
		//		this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		//	}

		//	if (this->m_anim.u == 2.0f && this->m_anim.v == 2.0f) {
		//		this->m_velocity.y = this->mf_vertical_jump_power;
		//		//this->m_velocity.x /= this->mf_horizontal_jump_power;
		//		this->mb_is_ground = false;
		//	}
		//	break;
	case PA_JUMP:
		// アニメーションが最後まで再生されました
		if (!is_hit) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		// 着地した
		if (this->mb_is_ground) {
			this->SetAnimation(PA_JUMP_DOWN, 0.0f, PLAYER_JUMP_DOWN_WIDTH, PLAYER_JUMP_DOWN_HEIGHT);
			CSingleton<CXAudio2>::GetInstance()->Start(SOUND_LABEL_SE_FALL);
		}
		break;
	case PA_JUMP_DOWN:
		// アニメーションが最後まで再生されました
		if (!is_hit) {
			this->SetAnimation(PA_IDLE_00, 0.0f, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);
		}
		break;
	case PA_HIT:
		// アニメーションが最後まで再生されました
		if (!is_hit) {
			if (this->mb_is_ground) {
				// 速度が0じゃない = 走るモーションへ
				if (this->m_velocity.x != 0.0f) {

					// 走り出しセット
					this->SetAnimation(PA_IDLETORUN, 0.0f, PLAYER_IDLE_TO_RUN_WIDTH, PLAYER_IDLE_TO_RUN_HEIGHT);
				}
				else {
					// 瞬き無しセット
					this->SetAnimation(PA_IDLE_00, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);
				}
			}
			else {
				this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
			}
		}
		break;
	case PA_OVER:
		// 無敵時間を切る
		this->mf_current_invincible_time = 0.0f;

		// アニメーションが最後まで再生されました
		if (!is_hit) {
			this->mf_anim_time = this->m_sprites[this->m_current_sprite].count_check * static_cast<float>(this->m_sprites[this->m_current_sprite].anim_max + 1);

			// アニメーションのコマ計算
			this->m_anim.u = static_cast<float>(this->m_sprites[this->m_current_sprite].slice_x - 1);
			this->m_anim.v = static_cast<float>(this->m_sprites[this->m_current_sprite].slice_y - 1);
		}
		break;
	default:
		break;
	}
}

void CPlayer::SetAnimation(PLAYER_ANIM anim_id, float set_time, float width, float height)
{
	// 経過時間を更新
	this->mf_anim_time = set_time;

	// アニメーション再生数をリセット
	this->mi_anim_count = 0;

	// 画像変更
	this->m_current_sprite = anim_id;

	// アニメーションのコマを0にする
	this->m_anim.u = 0.0f;
	this->m_anim.v = 0.0f;

	// 画像サイズ変更
	this->SetDefaultPosition(width, height);
}

void CPlayer::SetAnimation(PLAYER_ANIM anim_id, int count_max, float set_time, float width, float height)
{
	// 経過時間を更新
	this->mf_anim_time = set_time;

	// アニメーション再生数を増やす。
	this->mi_anim_count++;

	// count_max回やりました
	if (this->mi_anim_count >= count_max) {
		// アニメーション再生数をリセット
		this->mi_anim_count = 0;

		// 画像変更
		this->m_current_sprite = anim_id;
	}

	// アニメーションのコマを0にする
	this->m_anim.u = 0.0f;
	this->m_anim.v = 0.0f;

	// 画像サイズ変更
	this->SetDefaultPosition(width, height);
}

void CPlayer::Damage()
{
	if (this->mf_current_invincible_time <= 0.0f) {
		CSingleton<CXAudio2>::GetInstance()->Start(SOUND_LABEL_SE_DAMAGE);
		this->mf_current_invincible_time = this->mf_invincible_time;
		this->SetAnimation(PA_HIT, 0.0f, PLAYER_HIT_WIDTH, PLAYER_HIT_HEIGHT);
		CSingleton<CGameManager>::GetInstance()->PullHP();
	}
}

void CPlayer::StartGameOver()
{
	// 経過時間を更新
	this->mf_anim_time = 0.0f;

	// アニメーション再生数をリセット
	this->mi_anim_count = 0;

	// 画像変更
	this->m_current_sprite = PA_OVER;

	// アニメーションのコマを0にする
	this->m_anim.u = 0.0f;
	this->m_anim.v = 0.0f;

	// 画像サイズ変更
	this->SetDefaultPosition(PLAYER_OVER_WIDTH, PLAYER_OVER_HEIGHT);
}

bool CPlayer::CheckFinishGameOver()
{
	return this->mf_anim_time >= this->m_sprites[PA_OVER].count_check * static_cast<float>(this->m_sprites[PA_OVER].anim_max + 1);
}
