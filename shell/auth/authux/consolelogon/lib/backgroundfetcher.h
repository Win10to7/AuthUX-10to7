#pragma once
#include "pch.h"

class CBackground
{
public:

	static HRESULT GetButtonSet(UINT* buttonSet);
	static HRESULT GetBackground(HBITMAP* bitmap);

private:
	static bool _UseOEMBackground();
};
