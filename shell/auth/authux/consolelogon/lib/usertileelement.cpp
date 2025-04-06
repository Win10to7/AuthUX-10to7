#include "pch.h"
#include "usertileelement.h"

DirectUI::IClassInfo* CDUIUserTileElement::Class = nullptr;

DirectUI::IClassInfo* CDUIUserTileElement::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CDUIUserTileElement::GetClassInfoPtr()
{
	return Class;
}

HRESULT CDUIUserTileElement::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie,
	DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CDUIUserTileElement, int>(3, pParent, pdwDeferCookie, ppElement);
}

static DirectUI::PropertyInfoData dataimpTileZoomedProp;
static const int vvimpTileZoomedProp[] = { 2, (int)0x0FFFFFFFF }; //idfk, this is some weirdass shit
static const DirectUI::PropertyInfo impTileZoomedProp =
{
	.pszName = L"TileZoomed",
	.fFlags = 0xA,
	.fGroups = 0,
	.pValidValues = vvimpTileZoomedProp,
	.DefaultProc = DirectUI::Value::GetBoolFalse,
	.pData = &dataimpTileZoomedProp
};

const DirectUI::PropertyInfo* CDUIUserTileElement::TileZoomedProp()
{
	return &impTileZoomedProp;
}

HRESULT CDUIUserTileElement::Register()
{
	static const DirectUI::PropertyInfo* const s_rgProperties[] =
{
		&impTileZoomedProp
	};
	return DirectUI::ClassInfo<CDUIUserTileElement, DirectUI::Button>::RegisterGlobal(HINST_THISCOMPONENT, L"UserTile", s_rgProperties, ARRAYSIZE(s_rgProperties));
}

bool CDUIUserTileElement::GetTileZoomed()
{
	DirectUI::Value* pv = GetValue(TileZoomedProp, 0, nullptr);
	bool v = pv->GetBool();
	pv->Release();
	return v;
}

HRESULT CDUIUserTileElement::SetTileZoomed(bool v)
{
	DirectUI::Value* pv = DirectUI::Value::CreateBool(v);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = SetValue(TileZoomedProp, 0, pv);
		pv->Release();
	}

	return hr;
}

void CDUIUserTileElement::OnEvent(DirectUI::Event* pEvent)
{
	Button::OnEvent(pEvent);
}

HRESULT CDUIUserTileElement::_SetFieldInitialVisibility(DirectUI::Element* field,
	Microsoft::WRL::ComPtr<LCPD::ICredentialField> fieldData)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateElementArrays()
{
	return S_OK;
}

UINT CDUIUserTileElement::_FindFieldInsertionIndex(int fieldIndex)
{
	return 0;
}

HRESULT CDUIUserTileElement::_AddFieldContainer(int index, DirectUI::Element* Parent, CDUIFieldContainer** a5)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_AddField(DirectUI::Element* a2, int index, DirectUI::Element* Parent,
	CDUIFieldContainer** OutContainer)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateStringField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateEditField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateCommandLinkField(int index, DirectUI::Element* Parent,
	DirectUI::Element** outElement, CDUIFieldContainer** OutContainer)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateTileImageField(const wchar_t* pszLabel, HBITMAP bitmap,
	DirectUI::Element** OutContainer)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateSubmitButton(int index, DirectUI::Button** outButton,
	DirectUI::Element** outElement)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateCheckboxField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateComboBoxField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateFieldByIndex(int index)
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateFieldsForDeselected()
{
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateFieldsForSelected()
{
	return S_OK;
}
