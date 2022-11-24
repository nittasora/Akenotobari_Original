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

	// 初期化処理
	virtual void Initilize() = 0;

	// 終了処理
	virtual void UnInitilize() = 0;

	// 更新処理
	virtual void Update();

	// 描画処理
	virtual void Draw();

	// 当たりへの反応
	virtual void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other);

	// 場所の更新
	virtual void UpdateTransform();

	virtual bool GetGroundFlg();

	// 各種情報の取得
	ObjectTag GetTag();
	obj_flg GetFlg();
	Transform GetTransform();
	Color GetColor();
	UV GetAnim();
	Collider GetCollider();
	Collider GetConvertedCollider();
	Velocity GetVelocity();

	// 各種情報の設定
	void SetTag(const ObjectTag* tag);
	void SetFlg(const obj_flg* flg);
	void SetTransform(const Transform* transform);
	void SetColor(const Color* color);
	void SetAnim(const UV* anim);
	void SetCollider(const Collider* collider);
	void SetVelocity(const Velocity* velocity);

	// 現在有効かどうか
	bool IsActive();

	// 有効かどうか設定
	void IsActive(bool is_active);

	// テクスチャ追加
	void AddTexture(int id);
	void ChangeTexture(int id);

	// テクスチャを取得・設定
	SpriteData GetTexture();
	void SetTexture(const SpriteData* sprite);

	// デフォルトサイズの設定
	void SetDefaultPosition(float width, float height);
	void SetDefaultPosAndColl(float width, float height);

	virtual void Damage();

protected:
	// 画像の形
	struct VerPos
	{
		float x, y, z;
		float u, v;
	} m_default_vertex[SQUARE_VERTEX_NUM];

	// 頂点バッファ
	ID3D11Buffer* m_vertex_buffer;

	// タグ
	ObjectTag m_tag;

	// フラグ
	obj_flg m_flg;

	// 画像
	std::vector<SpriteData> m_sprites;

	// 現在の画像
	size_t m_current_sprite;

	// トランスフォーム情報
	Transform m_transform;

	// トランスフォーム変更量
	Translate m_change;

	// 色
	Color m_color;

	// アニメーションのコマ
	UV m_anim;

	// アニメーションの経過時間
	float mf_anim_time;

	// コライダー
	Collider m_collider;

	// 速度
	Velocity m_velocity;
};

