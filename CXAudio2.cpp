#include "CXAudio2.h"

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

CXAudio2::CXAudio2()
{
	this->mf_common_volume = 0.5f;
}

CXAudio2::~CXAudio2()
{
	this->Release();
}

HRESULT CXAudio2::Initialize(void)
{
	HRESULT      hr;

	HANDLE               hFile;
	DWORD                dwChunkSize;
	DWORD                dwChunkPosition;
	DWORD                filetype;

	// �b�n�l�̏�����
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	/**** Create XAudio2 ****/
	hr = XAudio2Create(&this->mp_xaudio2, 0);		// �������ͤ����t���O �f�o�b�O���[�h�̎w��(���݂͖��g�p�Ȃ̂�0�ɂ���)
	//hr=XAudio2Create(&g_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);		// ��O�����́Awindows�ł͖���
	if (FAILED(hr)) {
		CoUninitialize();
		return -1;
	}

	/**** Create Mastering Voice ****/
	hr = this->mp_xaudio2->CreateMasteringVoice(&this->mp_mastering_voice);			// ����͂o�b�̃f�t�H���g�ݒ�ɔC���Ă���
	/*, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL*/		// �{���U�̈����������Ă���
	if (FAILED(hr)) {
		if (this->mp_xaudio2)	this->mp_xaudio2->Release();
		CoUninitialize();
		return -1;
	}

	/**** Initalize Sound ****/
	for (int i = 0; i < SOUND_LABEL_MAX; i++)
	{
		memset(&this->m_wfx[i], 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&this->m_buffer[i], 0, sizeof(XAUDIO2_BUFFER));

		hFile = CreateFileA(this->g_param[i].filename, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			return HRESULT_FROM_WIN32(GetLastError());
		}
		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		//check the file type, should be fourccWAVE or 'XWMA'
		this->FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
		this->ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
		if (filetype != fourccWAVE)		return S_FALSE;

		this->FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
		this->ReadChunkData(hFile, &this->m_wfx[i], dwChunkSize, dwChunkPosition);

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		this->FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
		this->m_data_buffer[i] = new BYTE[dwChunkSize];
		this->ReadChunkData(hFile, this->m_data_buffer[i], dwChunkSize, dwChunkPosition);

		CloseHandle(hFile);

		// 	�T�u�~�b�g�{�C�X�ŗ��p����T�u�~�b�g�o�b�t�@�̐ݒ�
		this->m_buffer[i].AudioBytes = dwChunkSize;
		this->m_buffer[i].pAudioData = this->m_data_buffer[i];
		this->m_buffer[i].Flags = XAUDIO2_END_OF_STREAM;
		if (this->g_param[i].bLoop)
			this->m_buffer[i].LoopCount = XAUDIO2_LOOP_INFINITE;
		else
			this->m_buffer[i].LoopCount = 0;

		this->mp_xaudio2->CreateSourceVoice(&this->mp_source_voice[i], &(this->m_wfx[i].Format));
		this->mp_source_voice[i]->SubmitSourceBuffer(&(this->m_buffer[i]));	// �{�C�X�L���[�ɐV�����I�[�f�B�I�o�b�t�@�[��ǉ�
	}

	//mv_music_condition.clear();

	this->mv_fade_func.clear();

	return hr;
}

void CXAudio2::Release(void)
{
	for (int i = 0; i < SOUND_LABEL_MAX; i++)
	{
		if (this->mp_source_voice[i])
		{
			this->mp_source_voice[i]->Stop(0);
			this->mp_source_voice[i]->FlushSourceBuffers();
			this->mp_source_voice[i]->DestroyVoice();			// �I�[�f�B�I�O���t����\�[�X�{�C�X���폜
			delete[]  this->m_data_buffer[i];
		}
	}

	this->mp_mastering_voice->DestroyVoice();

	if (this->mp_xaudio2) this->mp_xaudio2->Release();

	// �b�n�l�̔j��
	CoUninitialize();
}

void CXAudio2::Update()
{
	for (size_t i = 0; i < this->mv_fade_func.size(); i++)
	{
		SOUND_LABEL label = this->mv_fade_func[i].m_label;
		float volume = this->mv_fade_func[i].m_volume;
		float diff = this->mv_fade_func[i].m_diff;
		if ((this->*mv_fade_func[i].mp_fade_func)(label, volume, diff)) {
			this->mv_fade_func.erase(this->mv_fade_func.begin() + i);
			i--;
			continue;
		}
	}

	//for (int i = 0; i < this->mv_music_condition.size(); i++)
	//{
	//	float target_volume = this->mv_music_condition[i].volume;
	//	IXAudio2SourceVoice* source = this->mp_source_voice[this->mv_music_condition[i].label];
	//	float volume;
	//	source->GetVolume(&volume);
	//	if (target_volume > volume) {
	//		volume += this->mv_music_condition[i].diff;
	//		if (volume > target_volume)
	//		{
	//			volume = target_volume;
	//			this->mv_music_condition.erase(this->mv_music_condition.begin() + i);
	//			i--;
	//		}
	//		source->SetVolume(volume);
	//	}
	//	else {
	//		volume -= this->mv_music_condition[i].diff;
	//		if (volume < target_volume)
	//		{
	//			volume = target_volume;
	//			this->mv_music_condition.erase(this->mv_music_condition.begin() + i);
	//			i--;
	//		}
	//		source->SetVolume(volume);
	//	}
	//}
}

void CXAudio2::Start(SOUND_LABEL label)
{
	// ��U�~�߂�
	this->Stop(label);

	// �ŏ����痬��
	float volume = this->ma_default_volume[label] * this->mf_common_volume;
	this->mp_source_voice[(int)label]->SetVolume(volume);
	this->Play(label);
}

void CXAudio2::Play(SOUND_LABEL label)
{
	// �~�܂��Ă���Ȃ痬��
	if (!this->CheckStop(label)) {
		float volume = this->ma_default_volume[label] * this->mf_common_volume;
		this->mp_source_voice[(int)label]->SetVolume(volume);
		this->mp_source_voice[(int)label]->Start(0);
	}
}

void CXAudio2::Stop(SOUND_LABEL label)
{
	// �Đ����~�߂�
	this->mp_source_voice[(int)label]->Stop(0);

	// ���S�Ɏ~�߂����̂ŏ��������Ă�����
	this->mp_source_voice[(int)label]->FlushSourceBuffers();
	this->mp_source_voice[(int)label]->SubmitSourceBuffer(&(this->m_buffer[(int)label]));
}

void CXAudio2::Pause(SOUND_LABEL label)
{
	// �Đ����~�߂�
	if (!this->CheckStop(label)) {
		this->mp_source_voice[(int)label]->Stop(0);
	}
}

bool CXAudio2::CheckStop(SOUND_LABEL label)
{
	// �����̏��
	XAUDIO2_VOICE_STATE xa2state;

	// �����̏�Ԏ擾
	this->mp_source_voice[(int)label]->GetState(&xa2state);

	// �������Đ����Ă��邩
	return (xa2state.BuffersQueued == 0);
}

void CXAudio2::XA_SetFadein(SOUND_LABEL label, float target_volume, float diff)
{
	for (size_t i = 0; i < this->mv_fade_func.size(); i++)
	{
		if (this->mv_fade_func[i].m_label == label) {
			this->mv_fade_func[i].mp_fade_func = &CXAudio2::XA_Fadein;
			this->mv_fade_func[i].m_label = label;
			this->mv_fade_func[i].m_volume = target_volume;
			this->mv_fade_func[i].m_diff = diff;
			return;
		}
	}

	MusicConditionEX new_condition;
	new_condition.mp_fade_func = &CXAudio2::XA_Fadein;
	new_condition.m_label = label;
	new_condition.m_volume = target_volume;
	new_condition.m_diff = diff;
	this->mv_fade_func.emplace_back(new_condition);
}

void CXAudio2::XA_SetFadeout(SOUND_LABEL label, float target_volume, float diff)
{
	for (size_t i = 0; i < this->mv_fade_func.size(); i++)
	{
		if (this->mv_fade_func[i].m_label == label) {
			this->mv_fade_func[i].mp_fade_func = &CXAudio2::XA_Fadeout;
			this->mv_fade_func[i].m_label = label;
			this->mv_fade_func[i].m_volume = target_volume;
			this->mv_fade_func[i].m_diff = diff;
			return;
		}
	}

	MusicConditionEX new_condition;
	new_condition.mp_fade_func = &CXAudio2::XA_Fadeout;
	new_condition.m_label = label;
	new_condition.m_volume = target_volume;
	new_condition.m_diff = diff;
	this->mv_fade_func.emplace_back(new_condition);
}

bool CXAudio2::XA_Fadein(SOUND_LABEL label, float target_volume, float diff)
{
	bool result = false;

	//�t�F�[�h�C��
	if (mp_source_voice[(int)label] == nullptr) return true;//���y�t�@�C���̏��������Ă��郂�m

	float volume;
	mp_source_voice[(int)label]->GetVolume(&volume);//�����Ɍ��݂̉��ʂ����Ă����֐�
	volume += diff;

	float target = target_volume * this->mf_common_volume * this->ma_default_volume[label];
	if (volume >= target) {
		volume = target;
		result = true;
	}

	mp_source_voice[(int)label]->SetVolume(volume);//�����̒l�ɉ��ʂ�ς���֐�

	return result;
}

bool CXAudio2::XA_Fadeout(SOUND_LABEL label, float target_volume, float diff)
{
	bool result = false;

	// �t�F�[�h�A�E�g
	if (mp_source_voice[(int)label] == nullptr) return true;//���y�t�@�C���̏��������Ă��郂�m

	float volume;
	mp_source_voice[(int)label]->GetVolume(&volume);//�����Ɍ��݂̉��ʂ����Ă����֐�
	volume -= diff;

	float target = target_volume * this->mf_common_volume * this->ma_default_volume[label];
	if (volume <= target) {
		volume = target;
		if (volume <= 0.0f) {
			this->Stop(label);
		}
		result = true;
	}

	mp_source_voice[(int)label]->SetVolume(volume);//�����̒l�ɉ��ʂ�ς���֐�

	return result;
}

void CXAudio2::XA_Setvolume(SOUND_LABEL label, float volume)
{
	//���ʒ���
	if (mp_source_voice[(int)label] == nullptr) return;//���y�t�@�C���̏��������Ă��郂�m

	mp_source_voice[(int)label]->SetVolume(volume);//�����̒l�ɉ��ʂ�ς���֐�
}

void CXAudio2::XA_SetCommonVolume(float volume)
{
	this->mf_common_volume = volume;
	for (size_t i = 0; i < SOUND_LABEL_MAX; i++)
	{
		this->XA_Setvolume(static_cast<SOUND_LABEL>(i), this->mf_common_volume * this->ma_default_volume[i]);
	}
}

float CXAudio2::XA_GetCommonVolume()
{
	return this->mf_common_volume;
}

HRESULT CXAudio2::FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;
	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());
		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());
		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}
		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}
		dwOffset += dwChunkDataSize;
		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}
	return S_OK;
}

HRESULT CXAudio2::ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}
