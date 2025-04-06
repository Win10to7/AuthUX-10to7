#pragma once

#include "pch.h"
#include "DirectUI/DirectUI.h"

class CDUIComboBox : public DirectUI::Combobox
{
public:
	CDUIComboBox();
	~CDUIComboBox() override;

	static DirectUI::IClassInfo* Class;
	DirectUI::IClassInfo* GetClassInfoW() override;
	static DirectUI::IClassInfo* GetClassInfoPtr();

	static HRESULT Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie, DirectUI::Element** ppElement);

	static HRESULT Register();


	HRESULT SetSelectionEx(int newSelection);
	int AddStringEx(const wchar_t* String);
	void OnHosted(DirectUI::Element* peNewHost) override;

	CCoSimpleArray<const wchar_t*> m_stringArray; //@mod, use CCoSimpleArray instead of hdpa
	int m_selection;


};
