#include "pch.h"
#include "zoomableelement.h"

DirectUI::IClassInfo* CDUIZoomableElement::Class = nullptr;

DirectUI::IClassInfo* CDUIZoomableElement::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CDUIZoomableElement::GetClassInfoPtr()
{
	return Class;
}

HRESULT CDUIZoomableElement::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie,
	DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CDUIZoomableElement, int>(0, pParent, pdwDeferCookie, ppElement);
}

static DirectUI::PropertyInfoData dataimpElementZoomedProp {};

static const int vvimpElementZoomedProp[] = { 2, (int)0x0FFFFFFFF };

static const DirectUI::PropertyInfo impElementZoomedProp =
{
	.pszName = L"ElementZoomed",
	.fFlags = 0xA,
	.fGroups = 0,
	.pValidValues = vvimpElementZoomedProp,
	.DefaultProc = DirectUI::Value::GetBoolFalse,
	.pData = &dataimpElementZoomedProp
};

const DirectUI::PropertyInfo* CDUIZoomableElement::ElementZoomedProp()
{
	return &impElementZoomedProp;
}

bool CDUIZoomableElement::GetElementZoomed()
{
	DirectUI::Value* pv = GetValue(ElementZoomedProp, 0, nullptr);
	bool v = pv->GetBool();
	pv->Release();
	return v;
}

HRESULT CDUIZoomableElement::SetElementZoomed(bool v)
{
	DirectUI::Value* pv = DirectUI::Value::CreateBool(v);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = SetValue(ElementZoomedProp, 0, pv);
		pv->Release();
	}

	return hr;
}

HRESULT CDUIZoomableElement::Register()
{
	static const DirectUI::PropertyInfo* const s_rgProperties[] =
{
		&impElementZoomedProp
	};
	return DirectUI::ClassInfo<CDUIZoomableElement, DirectUI::Element>::RegisterGlobal(HINST_THISCOMPONENT, L"ZoomableElement", s_rgProperties, ARRAYSIZE(s_rgProperties));
}
