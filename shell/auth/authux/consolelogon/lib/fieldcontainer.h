#pragma once

#include "pch.h"
#include "DirectUI/DirectUI.h"
#include "logoninterfaces.h"

/*
: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
, ControlBase
, WF::ITypedEventHandler<LCPD::ICredentialField*, LCPD::CredentialFieldChangeKind>
*/

class CDUIFieldContainer : public DirectUI::Element
{
public:

	static DirectUI::IClassInfo* Class;
	DirectUI::IClassInfo* GetClassInfoW() override;
	static DirectUI::IClassInfo* GetClassInfoPtr();

	static HRESULT Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie, DirectUI::Element** ppElement);

	static HRESULT Register();

	HRESULT AddField(DirectUI::Element* field);
	HRESULT ShowSubmitButton(const wchar_t* accName, DirectUI::Button** OutSubmitButton);
	void HideSubmitButton();

	DirectUI::Button* m_SubmitButton;
};
