#include "pch.h"
#include "labeledcheckbox.h"

#include "duiutil.h"

static BYTE _uidCDUILabeledCheckboxToggled = 0;
UID CDUILabeledCheckbox::s_Toggled = UID(&_uidCDUILabeledCheckboxToggled);

DirectUI::IClassInfo* CDUILabeledCheckbox::Class = nullptr;

DirectUI::IClassInfo* CDUILabeledCheckbox::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CDUILabeledCheckbox::GetClassInfoPtr()
{
	return Class;
}

HRESULT CDUILabeledCheckbox::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie,
	DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CDUILabeledCheckbox, int>(1, pParent, pdwDeferCookie, ppElement);
}

HRESULT CDUILabeledCheckbox::Register()
{
	return DirectUI::ClassInfo<CDUILabeledCheckbox, DirectUI::Button>::RegisterGlobal(HINST_THISCOMPONENT, L"DUILabeledCheckbox", nullptr, 0);
}

HRESULT CDUILabeledCheckbox::Configure(bool isChecked, const wchar_t* labelText)
{
	m_checkbox = FindDescendent(DirectUI::StrToID(L"Checkbox"));
	m_label = FindDescendent(DirectUI::StrToID(L"Label"));

	DWORD cookie;
	StartDefer(&cookie);

	auto scopeExit = wil::scope_exit([&]() -> void {if (cookie) { EndDefer(cookie); }});

	RETURN_IF_FAILED(SetContentAndAcc(m_label,labelText));

	RETURN_IF_FAILED(m_label->SetClass(L"SmallText"));

	m_checkedBackgroundFill = DirectUI::Value::CreateDFCFill(4u, 0x400u);
	if (m_checkedBackgroundFill)
	{
		m_uncheckedBackgroundFill = DirectUI::Value::CreateDFCFill(4u, 0);

		if (m_uncheckedBackgroundFill)
		{
			SetChecked(isChecked);
		}
		else
		{
			m_checkedBackgroundFill->Release();
			m_checkedBackgroundFill = nullptr;
		}
	}

	if (m_checkedBackgroundFill && m_uncheckedBackgroundFill)
	{
		RETURN_IF_FAILED(m_checkbox->SetAccessible(true));

		RETURN_IF_FAILED(m_checkbox->SetAccRole(44));

		RETURN_IF_FAILED(DirectUI::SetDefAction(m_checkbox,(unsigned int)isChecked - 791));

		RETURN_IF_FAILED(m_checkbox->SetAccName(labelText));
	}

	return S_OK;
}

void CDUILabeledCheckbox::SetChecked(bool newIsChecked)
{
	m_isChecked = newIsChecked;

	DWORD cookie;
	StartDefer(&cookie);

	DirectUI::Value* fillToUse;
	if (m_isChecked)
		fillToUse = m_checkedBackgroundFill;
	else
		fillToUse = m_uncheckedBackgroundFill;

	m_checkbox->SetValue(BackgroundProp,1,fillToUse);

	if (cookie)
		EndDefer(cookie);

	int AccState = m_checkbox->GetAccState();

	int unCheckedAccstate = AccState & 0xFFFFFFEF;
	int checkedAccstate = AccState | 0x10;

	if (m_isChecked)
		m_checkbox->SetAccState(checkedAccstate);
	else
		m_checkbox->SetAccState(unCheckedAccstate);

	DirectUI::SetDefAction(m_checkbox, (unsigned int)(LOBYTE(m_isChecked) != 0) - 791);
}

void CDUILabeledCheckbox::OnInput(DirectUI::InputEvent* inputEvent)
{
	if (inputEvent->nStage > DirectUI::GMF_ROUTED)
		return DirectUI::Button::OnInput(inputEvent);

	if (inputEvent->nDevice == DirectUI::GINPUT_KEYBOARD)
	{
		auto keyboardEvent = reinterpret_cast<DirectUI::KeyboardEvent*>(inputEvent);
		if ((keyboardEvent->ch == 13 || keyboardEvent->ch == 32) && keyboardEvent->nCode)
		{
			if (inputEvent->nCode == DirectUI::GMOUSE_DOWN)
			{
				SetChecked(m_isChecked == 0);

				DirectUI::Event event;
				event.fHandled = m_isChecked;
				event.fUIAHandled = m_isChecked;
				event.peTarget = this;
				event.uidType = CDUILabeledCheckbox::s_Toggled;
				DirectUI::Element::FireEvent(&event, true, false);
			}
		}
	}
	else if (inputEvent->nDevice == DirectUI::GINPUT_MOUSE)
	{
		auto mouseEvent = reinterpret_cast<DirectUI::MouseEvent*>(inputEvent);
		if ((mouseEvent->bButton == 1 || mouseEvent->bButton == 2) && inputEvent->nCode != DirectUI::GMOUSE_DOWN)
		{
			if (inputEvent->nCode == DirectUI::GMOUSE_UP)
			{
				SetChecked(m_isChecked == 0);

				DirectUI::Event event;
				event.fHandled = m_isChecked;
				event.fUIAHandled = m_isChecked;
				event.peTarget = this;
				event.uidType = CDUILabeledCheckbox::s_Toggled;
				DirectUI::Element::FireEvent(&event, true, false);
			}
		}
	}

	return DirectUI::Button::OnInput(inputEvent);
}

void CDUILabeledCheckbox::SetKeyFocus()
{
	if (GetVisible() && GetEnabled()) // guess
		m_checkbox->SetKeyFocus();
}
