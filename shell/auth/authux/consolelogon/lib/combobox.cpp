#include "pch.h"
#include "combobox.h"

DirectUI::IClassInfo* CDUIComboBox::Class = nullptr;

CDUIComboBox::CDUIComboBox() : m_selection(-1)
{

}

CDUIComboBox::~CDUIComboBox()
{
	for (size_t i = 0; i < m_stringArray.GetSize(); ++i)
	{
		DirectUI::HFree((void*)m_stringArray[i]);
	}

	DirectUI::Combobox::~Combobox();
}

DirectUI::IClassInfo* CDUIComboBox::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CDUIComboBox::GetClassInfoPtr()
{
	return Class;
}

HRESULT CDUIComboBox::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie, DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CDUIComboBox, int>(3, pParent, pdwDeferCookie, ppElement);
}

HRESULT CDUIComboBox::Register()
{
	return DirectUI::ClassInfo<CDUIComboBox, DirectUI::Combobox>::RegisterGlobal(HINST_THISCOMPONENT, L"DUIComboBox", nullptr, 0);
}

HRESULT CDUIComboBox::SetSelectionEx(int newSelection)
{
	if (GetHWND())
		return SetSelection(newSelection);

	m_selection = newSelection;

	return S_OK;
}

HRESULT StringStringAllocCopy(const wchar_t* Src, const wchar_t** a2)
{
	const wchar_t* newString = (const wchar_t*)DirectUI::HAlloc((wcslen(Src)+1)*2);
	memcpy((void*)newString,Src, (wcslen(Src) + 1) * 2);
	*a2 = newString;
	return S_OK;
}

int CDUIComboBox::AddStringEx(const wchar_t* String)
{
	if (GetHWND())
		return AddString((unsigned short const*)String);

	const wchar_t* newString = nullptr;
	if (SUCCEEDED(StringStringAllocCopy(String, &newString)))
	{
		if (FAILED(m_stringArray.Add(newString)))
			return -1;

		return m_stringArray.GetSize() - 1;
	}

	return -1;
}

void CDUIComboBox::OnHosted(DirectUI::Element* peNewHost)
{
	bool initiallyNullHWND = GetHWND() == nullptr;
	DirectUI::Combobox::OnHosted(peNewHost);

	if (initiallyNullHWND && GetHWND())
	{
		for (size_t i = 0; i < m_stringArray.GetSize(); ++i)
		{
			if (AddStringEx(m_stringArray[i]) == -1)
				return;
		}
	}

	m_stringArray.RemoveAll();

	if (m_selection != -1)
		SetSelectionEx(m_selection);
}
