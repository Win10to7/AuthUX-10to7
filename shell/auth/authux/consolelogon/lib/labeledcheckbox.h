#pragma once

#include "pch.h"
#include "DirectUI/DirectUI.h"

class CDUILabeledCheckbox : public DirectUI::Button
{
public:

	static DirectUI::IClassInfo* Class;
	DirectUI::IClassInfo* GetClassInfoW() override;
	static DirectUI::IClassInfo* GetClassInfoPtr();

	static HRESULT Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie, DirectUI::Element** ppElement);

	static HRESULT Register();

	HRESULT Configure(bool isChecked, const wchar_t* labelText);
	void SetChecked(bool isChecked);
	void OnInput(DirectUI::InputEvent* inputEvent) override;
	void SetKeyFocus() override;

	static UID s_Toggled;

	DirectUI::Element* m_checkbox;
	DirectUI::Element* m_label;
	DirectUI::Value* m_checkedBackgroundFill;
	DirectUI::Value* m_uncheckedBackgroundFill;
	BOOL m_isChecked;
};
