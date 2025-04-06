#include "pch.h"
#include "fieldcontainer.h"

DirectUI::IClassInfo* CDUIFieldContainer::Class = nullptr;

DirectUI::IClassInfo* CDUIFieldContainer::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CDUIFieldContainer::GetClassInfoPtr()
{
	return Class;
}

HRESULT CDUIFieldContainer::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie,
	DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CDUIFieldContainer, int>(0, pParent, pdwDeferCookie, ppElement);
}

HRESULT CDUIFieldContainer::Register()
{
	return DirectUI::ClassInfo<CDUIFieldContainer, DirectUI::Element>::RegisterGlobal(HINST_THISCOMPONENT, L"FieldContainer", nullptr, 0);
}

HRESULT CDUIFieldContainer::AddField(DirectUI::Element* field)
{
	return FindDescendent(DirectUI::StrToID(L"FieldParent"))->Add(field);
}

HRESULT CDUIFieldContainer::ShowSubmitButton(const wchar_t* accName, DirectUI::Button** OutSubmitButton)
{
	DWORD cookie;
	StartDefer(&cookie);
	if (!m_SubmitButton)
	{
		m_SubmitButton = (DirectUI::Button*)FindDescendent(DirectUI::StrToID(L"Submit"));
		m_SubmitButton->SetAccName(accName);
	}
	DirectUI::Element* ForceCenter = FindDescendent(DirectUI::StrToID(L"ForceCenter"));
	if (ForceCenter)
		ForceCenter->SetLayoutPos(-1);

	m_SubmitButton->SetLayoutPos(-1);
	m_SubmitButton->SetVisible(true);
	m_SubmitButton->SetEnabled(true);

	if (OutSubmitButton)
		*OutSubmitButton = m_SubmitButton;

	if (cookie)
		EndDefer(cookie);

	return S_OK;
}

void CDUIFieldContainer::HideSubmitButton()
{
	DWORD cookie = 0;
	StartDefer(&cookie);
	if (m_SubmitButton)
	{
		auto ForceCenter = FindDescendent(DirectUI::StrToID(L"ForceCenter"));
		if (ForceCenter)
			ForceCenter->SetLayoutPos(-3);

		m_SubmitButton->SetLayoutPos(-3);
		m_SubmitButton->SetVisible(false);
		m_SubmitButton->SetEnabled(false);
	}
	if (cookie)
		EndDefer(cookie);
}
