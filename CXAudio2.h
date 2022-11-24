#pragma once

#include <xaudio2.h>
#include <vector>
#include <array>

constexpr float DEFAULT_MAX_VOLUME = 1.0f;
constexpr float DEFAULT_DIFF_VOLUME = 0.1f;

// サウンドファイル
typedef enum
{
	SOUND_LABEL_BGM_TITLE = 0,
	SOUND_LABEL_BGM_SELECT,
	SOUND_LABEL_BGM_STAGE_1,
	SOUND_LABEL_BGM_STAGE_2,
	SOUND_LABEL_BGM_STAGE_3,
	SOUND_LABEL_BGM_RESULT,
	SOUND_LABEL_BGM_MAX,
	SOUND_LABEL_SE_CURSOR_MOVE = SOUND_LABEL_BGM_MAX,
	SOUND_LABEL_SE_CURSOR_DECISION,
	SOUND_LABEL_SE_START,
	SOUND_LABEL_SE_JUMP,
	SOUND_LABEL_SE_FOLD_1,
	SOUND_LABEL_SE_FOLD_2,
	SOUND_LABEL_SE_FOLD_2_RE,
	SOUND_LABEL_SE_DAMAGE,
	SOUND_LABEL_SE_FALL,
	SOUND_LABEL_SE_CANNON,
	SOUND_LABEL_SE_SWITCH_RB,
	SOUND_LABEL_SE_BACKSIDE,
	SOUND_LABEL_SE_GET_PIECE,
	SOUND_LABEL_SE_COMPLETE_PIECE,
	SOUND_LABEL_SE_CLEAR,
	SOUND_LABEL_SE_GAMEOVER,
	SOUND_LABEL_SE_MAX,
	SOUND_LABEL_MAX = SOUND_LABEL_SE_MAX,
} SOUND_LABEL;

struct MusicCondition
{
	SOUND_LABEL label;
	float volume;
	float diff;
};

class CXAudio2;

struct MusicConditionEX
{
	using Func = bool(CXAudio2::*)(SOUND_LABEL, float, float);

	SOUND_LABEL m_label;
	float m_volume;
	float m_diff;
	Func mp_fade_func;
};

class CXAudio2
{
public:
	CXAudio2();
	~CXAudio2();

	// XAudio2の初期化を担当する関数
	HRESULT Initialize(void);

	// XAudio2の解放を担当する関数
	void Release(void);

	// 更新処理
	void Update();

	// 音楽を始めから再生する
	void Start(SOUND_LABEL label);

	// 止まっている音楽を再生する
	void Play(SOUND_LABEL label);

	// 音楽を止める
	void Stop(SOUND_LABEL label);

	// 音楽を一時停止する
	void Pause(SOUND_LABEL label);

	// 音楽が止まっているかどうか
	bool CheckStop(SOUND_LABEL label);

	// 音楽のフェードインをセット
	void XA_SetFadein(SOUND_LABEL label, float target_volume, float diff);

	// 音楽のフェードアウトセット
	void XA_SetFadeout(SOUND_LABEL label, float target_volume, float diff);

	// 音量調整
	void XA_Setvolume(SOUND_LABEL label, float volume);

	// 共通の音量設定
	void XA_SetCommonVolume(float volume);
	float XA_GetCommonVolume();

private:
	//音楽をフェードイン
	bool XA_Fadein(SOUND_LABEL label, float target_volume, float diff);

	//音楽をフェードアウト
	bool XA_Fadeout(SOUND_LABEL label, float target_volume, float diff);

	// パラメータ構造体
	// ここに追加したい音楽ファイルへのパスなどを書くよ
	struct
	{
		LPCSTR filename;	// 音声ファイルまでのパスを設定
		bool bLoop;			// trueでループ。通常BGMはture、SEはfalse。
	} g_param[SOUND_LABEL_MAX] = { //RESULT_00
		{"assets/Game/Sound/BGM/TITLE_00.wav", true},
		{"assets/Game/Sound/BGM/SELECT_00.wav", true},
		{"assets/Game/Sound/BGM/STAGE_01.wav", true},
		{"assets/Game/Sound/BGM/STAGE_02.wav", true},
		{"assets/Game/Sound/BGM/STAGE_03.wav", true},
		{"assets/Game/Sound/BGM/RESULT_00.wav", true},
		{"assets/Game/Sound/SE/Cursor_Move.wav", false},
		{"assets/Game/Sound/SE/Cursor_Decision.wav", false},
		{"assets/Game/Sound/SE/Start.wav", false},
		{"assets/Game/Sound/SE/Jump.wav", false},
		{"assets/Game/Sound/SE/Fold01.wav", false},
		{"assets/Game/Sound/SE/Fold02.wav", false},
		{"assets/Game/Sound/SE/Fold02_reverse.wav", false},
		{"assets/Game/Sound/SE/Damage.wav", false},
		{"assets/Game/Sound/SE/Fall.wav", false},
		{"assets/Game/Sound/SE/Cannon.wav", false},
		{"assets/Game/Sound/SE/Switch_Red_Blue.wav", false},
		{"assets/Game/Sound/SE/BackSide.wav", false},
		{"assets/Game/Sound/SE/Get_RedSeal.wav", false},
		{"assets/Game/Sound/SE/Complete.wav", false},
		{"assets/Game/Sound/SE/Clear.wav", false},
		{"assets/Game/Sound/BGM/GAMEOVER_00.wav", false},
	};

	std::array<float, SOUND_LABEL_MAX> ma_default_volume = {
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
	};

	//std::vector<MusicCondition> mv_music_condition;

	std::vector<MusicConditionEX> mv_fade_func;

	// xaudio2を管理する
	IXAudio2               *mp_xaudio2 = NULL;

	// 再生時にミキシング後の状態を管理する
	IXAudio2MasteringVoice *mp_mastering_voice = NULL;

	// 音楽素材
	IXAudio2SourceVoice    *mp_source_voice[SOUND_LABEL_MAX];

	WAVEFORMATEXTENSIBLE	m_wfx[SOUND_LABEL_MAX];				// WAVフォーマット
	XAUDIO2_BUFFER			m_buffer[SOUND_LABEL_MAX];
	BYTE					*m_data_buffer[SOUND_LABEL_MAX];

	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);

	// 共通の音量
	float mf_common_volume;

};

