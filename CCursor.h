#pragma once

#include "CImage.h"

class CCursor : public CImage
{
public:
	// ‰Šú‰»ˆ—
	void Initilize() override;

	// I—¹ˆ—
	void UnInitilize() override;

	// XVˆ—
	void Update() override;
};

