#pragma once

struct COLLIDER_RESULT {

	bool is_hit;
	float distance_x, distance_y;
};

struct Collider
{
	float center_x;
	float center_y;
	float width;
	float height;
	bool is_active;
};

COLLIDER_RESULT Collider_Test(Collider* pThis, Collider* pOther);
COLLIDER_RESULT Collider_Test_Point(Collider* pThis, Collider* pOther);
