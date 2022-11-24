#include "Collider.h"
#include <cmath>

COLLIDER_RESULT Collider_Test(Collider * pThis, Collider * pOther)
{
	COLLIDER_RESULT res = { false };

	if (pThis->is_active == false || pOther->is_active == false) {
		return res;
	}

	// 中心点同士の距離を計算（絶対値）
	float distX = std::fabsf(pOther->center_x - pThis->center_x);
	float distY = std::fabsf(pOther->center_y - pThis->center_y);
	// ２つの四角形の縦横の長さを足して÷２した値を計算
	float halfTotalLenX = (pThis->width + pOther->width) / 2.0f;
	float halfTotalLenY = (pThis->height + pOther->height) / 2.0f;

	// 下の２つの条件が両方trueなら衝突している
	res.is_hit = distX < halfTotalLenX && distY < halfTotalLenY;

	// dy1のみ改変。なぜか下から上でもバグが出ない。
	float dx1 = (pOther->center_x - pOther->width / 2.0f) - (pThis->center_x + pThis->width / 2.0f);
	float dx2 = (pOther->center_x + pOther->width / 2.0f) - (pThis->center_x - pThis->width / 2.0f);
	float dy1 = (pOther->center_y - pOther->height / 2.0f) - (pThis->center_y + pThis->height / 2.0f);
	float dy2 = (pOther->center_y + pOther->height / 2.0f) - (pThis->center_y - pThis->height / 2.0f);

	float dx = fabsf(dx1) < fabsf(dx2) ? dx1 : dx2;
	float dy = fabsf(dy1) < fabsf(dy2) ? dy1 : dy2;

	if (fabsf(dx) < fabsf(dy)) {
		res.distance_x = dx;
		res.distance_y = 0.0f;
	}
	else {
		res.distance_x = 0.0f;
		res.distance_y = dy;
	}

	return res;
}

COLLIDER_RESULT Collider_Test_Point(Collider * pThis, Collider * pOther)
{
	COLLIDER_RESULT res = { false };

	if (pThis->is_active == false || pOther->is_active == false) {
		return res;
	}

	if (pThis->center_x < pOther->center_x - pOther->width / 2.0f) return res;
	if (pThis->center_x > pOther->center_x + pOther->width / 2.0f) return res;
	if (pThis->center_y < pOther->center_y - pOther->height / 2.0f) return res;
	if (pThis->center_y > pOther->center_y + pOther->height / 2.0f) return res;

	res.is_hit = true;
	res.distance_x = pOther->center_x - pThis->center_x;
	res.distance_y = pOther->center_y - pThis->center_y;

	return res;
}
