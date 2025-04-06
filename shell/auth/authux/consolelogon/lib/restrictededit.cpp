#include "pch.h"
#include "restrictededit.h"
#include <windowsx.h>

static BYTE _uidCDUIRestrictedEditCapsLockWarning = 0;
UID CDUIRestrictedEdit::s_CapsLockWarning = UID(&_uidCDUIRestrictedEditCapsLockWarning);

DirectUI::IClassInfo* CDUIRestrictedEdit::Class = nullptr;

DirectUI::IClassInfo* CDUIRestrictedEdit::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CDUIRestrictedEdit::GetClassInfoPtr()
{
	return Class;
}

HRESULT CDUIRestrictedEdit::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie,
	DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CDUIRestrictedEdit, int>(3, pParent, pdwDeferCookie, ppElement);
}

HRESULT CDUIRestrictedEdit::Register()
{
	return DirectUI::ClassInfo<CDUIRestrictedEdit, DirectUI::Edit>::RegisterGlobal(HINST_THISCOMPONENT, L"RestrictedEdit", 0, 0);
}

void CDUIRestrictedEdit::OnPropertyChanged(const DirectUI::PropertyInfo* ppi, int iIndex, DirectUI::Value* pvOld,
	DirectUI::Value* pvNew)
{
	if (!GetVisible())
		return DirectUI::Edit::OnPropertyChanged(ppi,iIndex,pvOld,pvNew);

	if (ppi == DirectUI::Element::KeyWithinProp() && (ppi->fFlags & 3) == iIndex)
	{
		_CheckCapsLock();
	}
	else if (ppi == DirectUI::Element::ContentProp() && (ppi->fFlags & 3) == iIndex)
	{
		DirectUI::Value* contentValue;
		const wchar_t* content = GetContentString(&contentValue);
		if (content && *content)
			SetAccValue(content);
		else
		{
			RemoveLocalValue(DirectUI::Element::AccValueProp);
		}

		//@Mod, dont need this, this is actually for credui, not logonui
		/*if (m_scenario == LCPD::CredProvScenario_CredUI)
		{
			DirectUI::Value* ClassValue;
			const wchar_t* Class = GetClass(&ClassValue);
			if (Class && _wcsicmp(Class, L"PasswordEdit") == 0)
			{
				WCHAR buf[256];
				HRESULT hr = GetEncodedContentString(buf,256);
				if (SUCCEEDED(hr))
					content = buf;
			}
			ClassValue->Release();

			//here i would set the datasource's value to content
		}*/
		contentValue->Release();
	}
	return DirectUI::Edit::OnPropertyChanged(ppi, iIndex, pvOld, pvNew);
}

LRESULT CDUIRestrictedEdit::_sSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData)
{
	CDUIRestrictedEdit* edit = reinterpret_cast<CDUIRestrictedEdit*>(uIdSubclass);
	if (uMsg > WM_COPY)
	{
		if (uMsg == WM_PASTE)
		{
			if (edit->m_scenario == LCPD::CredProvScenario_CredUI)
				return DefSubclassProc(hWnd,uMsg,wParam,lParam);

			return 0;
		}

		if (uMsg > WM_UNDO && (uMsg <= EM_GETCUEBANNER || uMsg > EM_HIDEBALLOONTIP))
			return DefSubclassProc(hWnd, uMsg, wParam, lParam);

		return 0;
	}
	switch (uMsg)
	{
	case WM_DESTROY:
		RemoveWindowSubclass(hWnd, (SUBCLASSPROC)CDUIRestrictedEdit::_sSubclassProc, dwRefData);
		edit->m_hwnd = 0;
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	case WM_SETTEXT:
		//do set text in datasource here

			return DefSubclassProc(hWnd, uMsg, wParam, lParam);

	case WM_CONTEXTMENU:
		if (edit->m_scenario == LCPD::CredProvScenario_CredUI)
			return DefSubclassProc(hWnd, uMsg, wParam, lParam);

		return 0;
	}
	if (uMsg != WM_GETDLGCODE)
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);

	return DefSubclassProc(hWnd, WM_GETDLGCODE, wParam, lParam) | 2;
}

void CDUIRestrictedEdit::SetKeyFocus()
{
	DirectUI::Edit::SetKeyFocus();
	//TODO:
	if (false ) // CKeyboardNavigationTracker::s_bInKeyboardNavigation
		SendMessageW(m_hwnd, EM_SETSEL, 0, -1);
}

HWND CDUIRestrictedEdit::CreateHWND(HWND parentHwnd)
{
	HWND newHwnd = DirectUI::Edit::CreateHWND(parentHwnd, false);
	if (newHwnd)
	{
		if (m_hintText)
			Edit_SetCueBannerText(newHwnd,m_hintText);

		Edit_LimitText(newHwnd, m_maxTextLength);

		if (SetWindowSubclass(newHwnd, CDUIRestrictedEdit::_sSubclassProc, (UINT_PTR)this, 0))
			m_hwnd = newHwnd;

		bool bIsPasswordEdit = false;

		DirectUI::Value* classValue;
		const wchar_t* Class = GetClass(&classValue);
		if (Class && _wcsicmp(Class, L"PasswordEdit") == 0)
			bIsPasswordEdit = true;

		if (bIsPasswordEdit)
			SetPasswordCharacter(9679);

		classValue->Release();
	}
	return newHwnd;
}

void CDUIRestrictedEdit::OnInput(DirectUI::InputEvent* inputEvent)
{
	DirectUI::Edit::OnInput(inputEvent);

	if (inputEvent->nDevice == DirectUI::GINPUT_KEYBOARD)
	{
		auto keyboardEvent = reinterpret_cast<DirectUI::KeyboardEvent*>(inputEvent);
		if (keyboardEvent->nStage)
			return;

		if (keyboardEvent->nCode == DirectUI::GMOUSE_UP)
		{
			keyboardEvent->fHandled = keyboardEvent->ch == VK_RETURN;
			return;
		}

		WCHAR chr;

		if (keyboardEvent->nCode)
		{
			if (keyboardEvent->nCode != DirectUI::GMOUSE_DOWN)
				return;
			chr = VK_MENU;
		}
		else
		{
			chr = VK_CAPITAL;
		}
		if (chr == keyboardEvent->ch)
			_CheckCapsLock();
	}
	else if (inputEvent->nDevice == DirectUI::GINPUT_MOUSE)
	{
		auto mouseEvent = reinterpret_cast<DirectUI::MouseEvent*>(inputEvent);
		if (mouseEvent->bButton == 1 || mouseEvent->bButton == 2)
			SetKeyFocus();
	}
}

void CDUIRestrictedEdit::_CheckCapsLock()
{
	bool bIsPasswordEdit = false;

	DirectUI::Value* classValue;
	const wchar_t* Class = GetClass(&classValue);
	if (Class && wcscmp(Class, L"PasswordEdit") == 0)
		bIsPasswordEdit = true;

	//TODO!!!!!!
	//if (bIsPasswordEdit)
	//{
	//    DirectUI::Event event;
	//    event.uidType = CDUIRestrictedEdit::CapsLockWarning;
	//}

	classValue->Release();
}
