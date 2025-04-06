#pragma once

#include "pch.h"
#include "DirectUI/DirectUI.h"

class CDUIRestrictedEdit : public DirectUI::Edit
{
public:

	static DirectUI::IClassInfo* Class;
	DirectUI::IClassInfo* GetClassInfoW() override;
	static DirectUI::IClassInfo* GetClassInfoPtr();

	static HRESULT Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie, DirectUI::Element** ppElement);

	static HRESULT Register();

	void OnPropertyChanged(const DirectUI::PropertyInfo* ppi, int iIndex, DirectUI::Value* pvOld, DirectUI::Value* pvNew) override;

	static LRESULT CALLBACK _sSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	void SetKeyFocus() override;

	HWND CreateHWND(HWND parentHwnd) override;

	void OnInput(DirectUI::InputEvent* inputEvent) override;

	HWND m_hwnd;
	const wchar_t* m_hintText = L"Hint Text Test";
	DWORD m_maxTextLength;
	LCPD::CredProvScenario m_scenario;

	Microsoft::WRL::ComPtr<LCPD::ICredentialField> m_fieldData;

	static UID s_CapsLockWarning;

private:
	void _CheckCapsLock();
};
