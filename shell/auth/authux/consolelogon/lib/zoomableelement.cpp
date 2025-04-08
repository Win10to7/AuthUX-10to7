#include "pch.h"
#include "zoomableelement.h"

#include "usertileelement.h"

DirectUI::IClassInfo* CDUIZoomableElement::Class = nullptr;

CDUIZoomableElement::CDUIZoomableElement() : m_index(-1)
	, m_owningElement(nullptr)
	, m_token(0)
{
}

CDUIZoomableElement::~CDUIZoomableElement()
{
}

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

HRESULT CDUIZoomableElement::Advise(LCPD::ICredentialField* dataSource)
{
	m_FieldInfo = dataSource;

	RETURN_IF_FAILED(m_FieldInfo->add_FieldChanged(this, &m_token)); // 19
	return S_OK;
}

HRESULT CDUIZoomableElement::UnAdvise()
{
	if (m_FieldInfo)
	{
		RETURN_IF_FAILED(m_FieldInfo->remove_FieldChanged(m_token)); // 25

		m_FieldInfo.Reset();
	}

	return S_OK;
}

void CDUIZoomableElement::OnDestroy()
{
	Element::OnDestroy();
	UnAdvise();
}

HRESULT CDUIZoomableElement::Invoke(LCPD::ICredentialField* sender, LCPD::CredentialFieldChangeKind args)
{
	LOG_HR_MSG(E_FAIL,"CDUIZoomableElement::Invoke\n");
	if (m_owningElement && m_owningElement->m_containersArray[m_index])
	{
		CFieldWrapper* fieldData;
		m_owningElement->fieldsArray.GetAt(m_index,fieldData);
		m_owningElement->_SetFieldInitialVisibility(m_owningElement->m_containersArray[m_index],fieldData);
		LOG_HR_MSG(E_FAIL,"CDUIZoomableElement::Invoke _SetFieldInitialVisibility\n");
	}

	return S_OK;
}

HRESULT CDUIZoomableElement::Register()
{
	static const DirectUI::PropertyInfo* const s_rgProperties[] =
	{
		&impElementZoomedProp
	};
	return DirectUI::ClassInfo<CDUIZoomableElement, DirectUI::Element>::RegisterGlobal(HINST_THISCOMPONENT, L"ZoomableElement", s_rgProperties, ARRAYSIZE(s_rgProperties));
}
