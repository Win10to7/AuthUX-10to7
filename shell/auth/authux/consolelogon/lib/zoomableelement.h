#pragma once

#include "pch.h"
#include "DirectUI/DirectUI.h"

class CDUIZoomableElement : public DirectUI::Element
{
public:

	static DirectUI::IClassInfo* Class;
	DirectUI::IClassInfo* GetClassInfoW() override;
	static DirectUI::IClassInfo* GetClassInfoPtr();

	static HRESULT Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie, DirectUI::Element** ppElement);

	static const DirectUI::PropertyInfo* ElementZoomedProp();
	bool GetElementZoomed();
	HRESULT SetElementZoomed(bool v);

	static HRESULT Register();
};
