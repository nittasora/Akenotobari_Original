#pragma once

struct Transform;
struct Scale;
struct Matrix4_4;

class CCamera
{
public:
	// 初期化
	void Initilize();

	// 更新
	void Update();

	// 終了
	void Finalize();

	// Transform取得
	Transform GetTransform();

	// Transform設定
	void SetTransform(const Transform* transform);

	// スクリーンサイズ設定
	void SetScreenScale(float x, float y);

	// 2Dカメラ用行列の取得
	Matrix4_4 Get2DMatrix();

private:
	Transform* m_transform;
	Scale* m_screen_scale;
	Matrix4_4* m_2d_matrix;
};

