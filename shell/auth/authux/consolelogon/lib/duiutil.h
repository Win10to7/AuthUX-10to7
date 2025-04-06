#pragma once
#include "pch.h"
#include "DirectUI/DirectUI.h"

static HRESULT SetContentAndAcc(DirectUI::Element* element, const wchar_t* content)
{
	HRESULT result = S_OK;

	if (content)
	{
		result = element->SetContentString(content);
		if (SUCCEEDED(result))
			return element->SetAccName(content);
	}
	else
	{
		result = element->RemoveLocalValue(DirectUI::Element::ContentProp);
		if (SUCCEEDED(result))
			return element->RemoveLocalValue(DirectUI::Element::AccNameProp);
	}
	return result;
}
