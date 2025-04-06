#pragma once

#include "pch.h"

#include "logonnativehwndhost.h"
#include "DirectUI/DirectUI.h"
#include "userlist.h"

class CLogonFrame : public DirectUI::HWNDElement
{
public:

	~CLogonFrame() override;

	static DirectUI::IClassInfo* Class;
	DirectUI::IClassInfo* GetClassInfoW() override;
	static DirectUI::IClassInfo* GetClassInfoPtr();

	static HRESULT Create(HWND hParent, bool fDblBuffer, UINT nCreate, Element* pParent, DWORD* pdwDeferCookie, DirectUI::Element** ppElement);

	static HRESULT Create(CLogonNativeHWNDHost* host);

	static HRESULT Register();

	static CLogonFrame* GetSingleton();

	HRESULT CreateStyleParser(DirectUI::DUIXmlParser** outParser) override;
	void OnEvent(DirectUI::Event* pEvent) override;

	void SetBackgroundGraphics();
	void ShowSecurityOptions(LC::LogonUISecurityOptions SecurityOptsFlag, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion);
	HRESULT OnSecurityOptionSelected(LC::LogonUISecurityOptions SecurityOpt);

	void ShowStatusMessage(const wchar_t* message);

	CLogonNativeHWNDHost* m_nativeHost;
	DirectUI::Element* m_CurrentWindow = 0;
	DirectUI::DUIXmlParser* m_xmlParser;

	DirectUI::Element* m_Window = 0;
	DirectUI::Element* m_Options = 0;
	DirectUI::Element* m_SwitchUser = 0;
	DirectUI::Element* m_OtherTiles = 0;
	DirectUI::Element* m_Ok = 0;
	DirectUI::Element* m_Yes = 0;
	DirectUI::Element* m_No = 0;
	DirectUI::Element* m_Cancel = 0;
	DirectUI::Element* m_ShutDownFrame = 0;
	DirectUI::Element* m_ShowPLAP = 0;
	DirectUI::Element* m_DisconnectPLAP = 0;
	DirectUI::Element* m_Accessibility = 0;
	DirectUI::Element* m_MessageFrame = 0;
	DirectUI::Element* m_FullMessageFrame = 0;
	DirectUI::Element* m_ShortMessageFrame = 0;
	DirectUI::Element* m_ConnectMessageFrame = 0;
	DirectUI::Element* m_Status = 0;
	DirectUI::Element* m_StatusText = 0;
	DirectUI::Element* m_SecurityOptions = 0;
	DirectUI::Element* m_Locked = 0;
	DirectUI::Element* m_WaitAnimation = 0;

	UserList* m_activeUserList;
	UserList* m_LogonUserList;
	UserList* m_PLAPUserList;

	bool isHighContrast = false;

private:
	HRESULT _Initialize(CLogonNativeHWNDHost* Host, DirectUI::Element* pParent, DWORD* DeferCookie);
	HRESULT _InitializeUserLists();
	bool _IsInstallUpdatesAndShutdownAllowed();
	bool _ShowBackgroundBitmap();
	void _SetSoftKeyboardAllowed(bool allowed);
	void _SetBrandingGraphic();
	void _SetOptions(int a2);
	void _SelectMode(DirectUI::Element* elementToHost, bool isVisible);
	void _ShowCursor(bool bShow);
	void _DisplayStatusMessage(const wchar_t* message, bool showSpinner);
	void _SwitchToUserList(class UserList* userList);
	void _DisplayLogonDialog(const wchar_t* messageCaptionContent, const wchar_t* messageContent, WORD flags);

	static CLogonFrame* _pSingleton;

	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>> m_SecurityOptionsCompletion;
};
