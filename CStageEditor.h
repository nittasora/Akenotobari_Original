#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// �X�e�[�W�G�f�B�^�\

#include "CScene.h"

#include "StageData.h"

class CImage;
class CButton;
class CCursor;
class CPage;
class CPlayer;
class CEditorText;

class CStageEditor : public CScene
{
public:
	// �R���X�g���N�^
	CStageEditor();

	// �f�X�g���N�^
	~CStageEditor();

	// ����������
	// �V�[���J�n���̏�����
	void Initialize() override final;

	// �I������
	// �V�[���I�����̏�����
	void Finalize() override final;

	// �X�V����
	void Update() override final;

	// �`�揈��
	void Draw() override final;

	// �|�[�Y���X�V����
	void PauseUpdate() override final;

	// �|�[�Y���`�揈��
	void PauseDraw() override final;

	// �����蔻��
	void ColliderTest();

private:
	// �X�e�[�W�̓ǂݍ���
	void LoadStage();

	// �X�e�[�W�̕ۑ�
	void SaveStage();

	// �X�e�[�W�̍X�V
	void UpdateStage();

	// �摜�I�u�W�F�N�g�̐���
	void CreateImage();

	// �摜�̒ǉ�
	void AddSprite();

	// �摜�̕ύX
	void ChangeSprite(int id);

	// �v���C���[�̈ړ�
	void MovePlayer();

	// �M�~�b�N�̍X�V
	void UpdateGoal(std::vector<GoalData> data);
	void UpdatePiece(std::vector<PieceData> data);
	void UpdateTornade(std::vector<TornadeData> data);
	void UpdateDharma(std::vector<DharmaData> data);
	void UpdateCannon(std::vector<CannonData> data);
	void UpdateLion(std::vector<LionData> data);

	// �M�~�b�N�̏��̓���
	void InputDharmaData(DharmaData* data);
	void InputCannonData(CannonData* data);
	void InputLionTopData(LionData* data);

	// �ʏ펞�̍X�V�E�`�揈��
	void NormalUpdate();
	void NormalColliderTest();
	void NormalDraw();

	// �摜�ݒ�ύX�̍X�V�E�`�揈��
	void SpriteOptionUpdate();
	void SpriteOptionColliderTest();
	void SpriteOptionDraw();

	// �}�b�v�`�b�v�I���̍X�V�E�`�揈��
	void MapChipSelectUpdate();
	void MapChipSelectColliderTest();

	// �}�b�v�`�b�v�ݒ�̍X�V�E�`�揈��
	void MapChipUpdate();
	void MapChipColliderTest();

	// �w�i�ݒ�̍X�V�E�`�揈��
	void BackSelectUpdate();
	void BackSelectColliderTest();

	// �M�~�b�N�I���̍X�V�E�`�揈��
	void GimmikSelectUpdate();
	void GimmikSelectColliderTest();
	void GimmikSelectDraw();

	// �M�~�b�N�ݒ�̍X�V�E�`�揈��
	void GimmikUpdate();
	void GimmikColliderTest();

	// �摜
	std::vector<CImage*> mvp_images;

	// �}�b�v�`�b�v�̕\���p�ƑI��p
	std::vector<CImage*> mvp_mapchip;
	std::vector<CImage*> mvp_mapchip_image;

	// ����
	std::vector<CEditorText*> mvp_text;

	// ����
	std::vector<CEditorText*> mvp_value_text;

	// �X�e�[�W�̔w�i
	std::vector<CPage*> mvp_pages;

	// �v���C���[
	std::vector<CPlayer*> mvp_player;

	// ���C����ʂ̃{�^��
	std::vector<CButton*> mvp_main_buttons;

	// �I����ʂ̃{�^��
	std::vector<CButton*> mvp_select_buttons;

	// �J�[�\���̉摜
	std::vector<CCursor*> mvp_cursor_image;

	// �M�~�b�N�̕\���p�ƑI��p
	std::vector<CImage*> mvp_gimmik_image;
	std::vector<CImage*> mvp_goal;
	std::vector<CImage*> mvp_piece;
	std::vector<CImage*> mvp_tornado;
	std::vector<CImage*> mvp_dharma;
	std::vector<CImage*> mvp_cannon;
	std::vector<CImage*> mvp_lion;

	// �J�[�\��
	CCursor* mp_cursor;
};

