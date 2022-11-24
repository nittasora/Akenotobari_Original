#pragma once

#include "CSpriteFactory.h"
#include "ModelData.h"
#include "Collider.h"
#include "ObjectData.h"

class CGameObject
{
public:
	CGameObject();
	~CGameObject();

	// ����������
	virtual void Initilize() = 0;

	// �I������
	virtual void UnInitilize() = 0;

	// �X�V����
	virtual void Update();

	// �`�揈��
	virtual void Draw();

	// ������ւ̔���
	virtual void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other);

	// �ꏊ�̍X�V
	virtual void UpdateTransform();

	virtual bool GetGroundFlg();

	// �e����̎擾
	ObjectTag GetTag();
	obj_flg GetFlg();
	Transform GetTransform();
	Color GetColor();
	UV GetAnim();
	Collider GetCollider();
	Collider GetConvertedCollider();
	Velocity GetVelocity();

	// �e����̐ݒ�
	void SetTag(const ObjectTag* tag);
	void SetFlg(const obj_flg* flg);
	void SetTransform(const Transform* transform);
	void SetColor(const Color* color);
	void SetAnim(const UV* anim);
	void SetCollider(const Collider* collider);
	void SetVelocity(const Velocity* velocity);

	// ���ݗL�����ǂ���
	bool IsActive();

	// �L�����ǂ����ݒ�
	void IsActive(bool is_active);

	// �e�N�X�`���ǉ�
	void AddTexture(int id);
	void ChangeTexture(int id);

	// �e�N�X�`�����擾�E�ݒ�
	SpriteData GetTexture();
	void SetTexture(const SpriteData* sprite);

	// �f�t�H���g�T�C�Y�̐ݒ�
	void SetDefaultPosition(float width, float height);
	void SetDefaultPosAndColl(float width, float height);

	virtual void Damage();

protected:
	// �摜�̌`
	struct VerPos
	{
		float x, y, z;
		float u, v;
	} m_default_vertex[SQUARE_VERTEX_NUM];

	// ���_�o�b�t�@
	ID3D11Buffer* m_vertex_buffer;

	// �^�O
	ObjectTag m_tag;

	// �t���O
	obj_flg m_flg;

	// �摜
	std::vector<SpriteData> m_sprites;

	// ���݂̉摜
	size_t m_current_sprite;

	// �g�����X�t�H�[�����
	Transform m_transform;

	// �g�����X�t�H�[���ύX��
	Translate m_change;

	// �F
	Color m_color;

	// �A�j���[�V�����̃R�}
	UV m_anim;

	// �A�j���[�V�����̌o�ߎ���
	float mf_anim_time;

	// �R���C�_�[
	Collider m_collider;

	// ���x
	Velocity m_velocity;
};

