#include "pch.h"
#include "logonframe.h"

#include <WtsApi32.h>

#include "logoninterfaces.h"
#include "userlist.h"
#include "usertileelement.h"
#include "backgroundfetcher.h"
#include "duiutil.h"
#include "logonguids.h"
#include "slpublic.h"

using namespace Microsoft::WRL;

DirectUI::IClassInfo* CLogonFrame::Class = nullptr;
CLogonFrame* CLogonFrame::_pSingleton = nullptr;

CLogonFrame::~CLogonFrame()
{
	if (m_xmlParser)
		m_xmlParser->Destroy();

	if (m_nativeHost)
	{
		m_nativeHost->Destroy();
		DestroyWindow(m_nativeHost->GetHWND());
	}

	UnregisterClassW(L"AUTHUI.DLL: LogonUI MainFrame Timer Window", HINST_THISCOMPONENT);

	DirectUI::HWNDElement::~HWNDElement();
}

DirectUI::IClassInfo* CLogonFrame::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CLogonFrame::GetClassInfoPtr()
{
	return Class;
}

HRESULT CLogonFrame::Create(HWND hParent, bool fDblBuffer, UINT nCreate, Element* pParent, DWORD* pdwDeferCookie,
	DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CLogonFrame, HWND, bool, UINT>(hParent, fDblBuffer,nCreate, pParent, pdwDeferCookie,ppElement);
}

HRESULT CLogonFrame::Create(CLogonNativeHWNDHost* host)
{
	_pSingleton = DirectUI::HNew<CLogonFrame>();
	if (!_pSingleton)
	{
		DestroyWindow(host->GetHWND());
		return E_OUTOFMEMORY;
	}

	DWORD defer;
	HRESULT hr = _pSingleton->_Initialize(host, nullptr, &defer);
	_pSingleton->EndDefer(defer);
	return hr;
}

HRESULT CLogonFrame::Register()
{
	return DirectUI::ClassInfo<CLogonFrame, DirectUI::HWNDElement, DirectUI::EmptyCreator<CLogonFrame>>::RegisterGlobal(HINST_THISCOMPONENT, L"MainFrame", nullptr, 0);
}

CLogonFrame* CLogonFrame::GetSingleton()
{
	return _pSingleton;
}

HRESULT CLogonFrame::CreateStyleParser(DirectUI::DUIXmlParser** outParser)
{
	*outParser = nullptr;

	UINT buttonSet;
	RETURN_IF_FAILED(CBackground::GetButtonSet(&buttonSet));

	DirectUI::DUIXmlParser* parser = nullptr;
	RETURN_IF_FAILED(DirectUI::DUIXmlParser::Create(&parser, 0, 0, 0, 0));

	//parser->SetParseErrorCallback([](const WCHAR* pszError, const WCHAR* pszToken, int dLine, void* pContext) {
	//	MessageBox(nullptr, std::format(L"err: {}; {}; {}\n", pszError, pszToken, dLine).c_str(), L"Error while parsing DirectUI", 0);
	//	DebugBreak();
	//	}, nullptr);

	auto cleaner = wil::scope_exit([&] { parser->Destroy(); });

	RETURN_IF_FAILED(parser->SetXMLFromResource((UIFILE_LOGON + buttonSet),HINST_THISCOMPONENT,nullptr));

	cleaner.release();

	*outParser = parser;
	return S_OK;
}

void CLogonFrame::OnEvent(DirectUI::Event* pEvent)
{
	//if (pEvent->peTarget == m_SwitchUser && pEvent->uidType == DirectUI::Button::Click())
	//{
	//	//m_activeUserList->UnzoomList(m_activeUserList->GetZoomedTile());
	//	//m_consoleUIManager->m_credProvDataModel->put_SelectedUserOrV1Credential(nullptr);
	//	return DirectUI::HWNDElement::OnEvent(pEvent);
	//}

	if (pEvent->uidType == DirectUI::Button::Click() && m_CurrentWindow == m_MessageFrame && pEvent->nStage == DirectUI::GMF_BUBBLED)
	{
		MessageOptionFlag flag;
		if (pEvent->peTarget == m_Ok)
			flag = MessageOptionFlag::Ok;
		else if (pEvent->peTarget == m_Cancel)
			flag = MessageOptionFlag::Cancel;
		else if (pEvent->peTarget == m_No)
			flag = MessageOptionFlag::No;
		else if (pEvent->peTarget == m_Yes)
			flag = MessageOptionFlag::Yes;
		else
			flag = MessageOptionFlag::None;

		if (flag != MessageOptionFlag::None)
		{
			if (m_MessageDisplayResultCompletion.get() != nullptr)
			{
				OnMessageOptionPressed(flag);
			}
			else
			{
				m_consoleUIManager->ShowCredentialView();
				//ComPtr<LogonViewManager> thisRef = m_consoleUIManager;
				//
				//HRESULT hr = BeginInvoke(m_consoleUIManager->m_Dispatcher.Get(), [=]() -> void
				//{
				//	thisRef->ShowCredentialView();
				//});
				//LOG_IF_FAILED(hr);
			}
		}
	}

	if (pEvent->uidType == DirectUI::Button::Click() && m_CurrentWindow == m_activeUserList && pEvent->nStage == DirectUI::GMF_BUBBLED)
	{
		if (IsElementOfClass(pEvent->peTarget,L"UserTile"))
		{
			CDUIUserTileElement* tile = static_cast<CDUIUserTileElement*>(pEvent->peTarget);
			if (tile->m_dataSourceCredential.Get())
			{
				m_consoleUIManager->m_credProvDataModel->put_SelectedUserOrV1Credential(tile->m_dataSourceUser.Get());
				//HRESULT hr = BeginInvoke(m_consoleUIManager->m_Dispatcher.Get(), [=]() -> void
				//{
				//	UNREFERENCED_PARAMETER(this);
				//	thisRef->m_credProvDataModel->put_SelectedUserOrV1Credential(tile->m_dataSourceCredential.Get());
				//});
			}
			else
			{
				LOG_HR_MSG(E_FAIL,"TILE HAD NO CREDENTIAL DATASOURCE");
			}
		}

		if (pEvent->peTarget == m_Cancel && m_currentReason == LC::LogonUIRequestReason_LogonUIChange)
		{
			LOG_HR_IF_NULL_MSG(E_FAIL,m_consoleUIManager.Get(),"m_consoleUIManager IS NULL!!!");
			if (m_consoleUIManager.Get() && m_consoleUIManager->m_requestCredentialsComplete.get())
			{
				ComPtr<LogonViewManager> thisRef = m_consoleUIManager;
				HRESULT hr = BeginInvoke(m_consoleUIManager->m_Dispatcher.Get(), [=]() -> void
				{
					UNREFERENCED_PARAMETER(this);
					thisRef->m_requestCredentialsComplete->Complete(HRESULT_FROM_WIN32(ERROR_CANCELLED));
					thisRef->ClearCredentialStateUIThread();
					thisRef->m_requestCredentialsComplete.reset();
				});

				//m_consoleUIManager->m_requestCredentialsComplete = nullptr;
			}
		}
		else if (pEvent->peTarget == m_SwitchUser)
		{
			if (!GetSystemMetrics(SM_REMOTESESSION))
			{
				ComPtr<LogonViewManager> thisRef = m_consoleUIManager;
				HRESULT hr = BeginInvoke(m_consoleUIManager->m_Dispatcher.Get(), [=]() -> void
				{
					UNREFERENCED_PARAMETER(this);
					if (m_currentReason == LC::LogonUIRequestReason_LogonUIUnlock)
					{
						//RETURN_IF_FAILED(thisRef->m_userSettingManager->put_IsLockScreenAllowed(FALSE)); // 281
						if (SUCCEEDED(thisRef->m_userSettingManager->put_IsLockScreenAllowed(FALSE)))
							WTSDisconnectSession(nullptr, WTS_CURRENT_SESSION, FALSE);
					}
					else if (m_currentReason == LC::LogonUIRequestReason_LogonUILogon)
					{
						//RETURN_IF_FAILED(thisRef->m_credProvDataModel->put_SelectedUserOrV1Credential(nullptr)); // 286
						thisRef->m_credProvDataModel->put_SelectedUserOrV1Credential(nullptr); // 286
					}
				});

			}
		}

		return DirectUI::HWNDElement::OnEvent(pEvent);
	}

	if (pEvent->uidType == DirectUI::Button::Click() && m_CurrentWindow == m_SecurityOptions && m_SecurityOptionsCompletion && pEvent->nStage == DirectUI::GMF_BUBBLED)
	{
		LC::LogonUISecurityOptions options;
		if (pEvent->peTarget->GetID() == DirectUI::StrToID(L"SecurityLock"))
			options = LC::LogonUISecurityOptions_Lock;
		else if (pEvent->peTarget->GetID() == DirectUI::StrToID(L"SecuritySwitchUser"))
			options = LC::LogonUISecurityOptions_SwitchUser;
		else if (pEvent->peTarget->GetID() == DirectUI::StrToID(L"SecurityLogOff"))
			options = LC::LogonUISecurityOptions_LogOff;
		else if (pEvent->peTarget->GetID() == DirectUI::StrToID(L"SecurityChange"))
			options = LC::LogonUISecurityOptions_ChangePassword;
		else if (pEvent->peTarget->GetID() == DirectUI::StrToID(L"SecurityTaskManager"))
			options = LC::LogonUISecurityOptions_TaskManager;
		else if (pEvent->peTarget->GetID() == DirectUI::StrToID(L"Cancel"))
			options = LC::LogonUISecurityOptions_Cancel;

		OnSecurityOptionSelected(options);
	}

	if (pEvent->uidType == DirectUI::Button::Click() && m_CurrentWindow == m_LogonUserList)
	{

	}

	return DirectUI::HWNDElement::OnEvent(pEvent);
}

bool CLogonFrame::_ShowBackgroundBitmap()
{
	return GetSystemMetrics(SM_REMOTESESSION) != 0; //7 authui creates a com object of CSystemSettings and calls CSystemSettings::IsLogonWallpaperShown which does this.
}

BOOL SHWindowsPolicy(REFGUID rpolid)
{
	static BOOL(WINAPI* fSHWindowsPolicy)(REFGUID) = decltype(fSHWindowsPolicy)(GetProcAddress(LoadLibrary(L"SHLWAPI.dll"), MAKEINTRESOURCEA(618)));
	return fSHWindowsPolicy(rpolid);
}

bool CLogonFrame::_IsSwitchUserAllowed()
{
	if ( !GetSystemMetrics(SM_REMOTESESSION) )
	{
		DWORD allowMultipleSessions;
		if (SUCCEEDED(SLGetWindowsInformationDWORD(L"TerminalServices-RemoteConnectionManager-AllowMultipleSessions", &allowMultipleSessions)))
		{
			if ( allowMultipleSessions != 0 )
				return !SHWindowsPolicy(POLID_HideFastUserSwitching);
		}

	}
	return false;
}

void CLogonFrame::SetBackgroundGraphics()
{
	auto InsideFrameElement = FindDescendent(DirectUI::StrToID(L"InsideFrame"));

	HBITMAP bitmap = nullptr;
	CBackground::GetBackground(&bitmap);

	if (!bitmap)
		return;

	auto graphic = DirectUI::Value::CreateGraphic(bitmap, 4, 0xFFFFFFFF, false, false, false);
	if (!graphic)
	{
		DeleteObject(bitmap);
		return;
	}

	InsideFrameElement->SetValue(DirectUI::Element::BackgroundProp, 1, graphic);
	graphic->Release();
}

struct SecurityOptionsFlags
{
	LC::LogonUISecurityOptions flag;
	const wchar_t* id;
};

SecurityOptionsFlags secOptsFlags[] = { {LC::LogonUISecurityOptions_Lock,L"SecurityLock"},{LC::LogonUISecurityOptions_LogOff,L"SecurityLogOff"},{LC::LogonUISecurityOptions_ChangePassword,L"SecurityChange"},{LC::LogonUISecurityOptions_TaskManager,L"SecurityTaskManager"},{LC::LogonUISecurityOptions_SwitchUser,L"SecuritySwitchUser"}};

void CLogonFrame::ShowSecurityOptions(LC::LogonUISecurityOptions SecurityOptsFlag, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion)
{
	m_SecurityOptionsCompletion = wil::make_unique_nothrow<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>>(completion);
	if (m_SecurityOptionsCompletion.get() == nullptr)
		return;

	DWORD cookie;
	StartDefer(&cookie);

	DirectUI::Element* InsideWindow = m_Window->FindDescendent(DirectUI::StrToID(L"InsideWindow"));
	InsideWindow->Add(m_SecurityOptions);
	m_SecurityOptions->SetVisible(true);
	//if (IsHighContrastOn())
	//    _SetUIForHighContrast(1,InsideWindow);
	SetBackgroundGraphics();
	_SelectMode(m_SecurityOptions,true);

	//DWORD optsFlag = 32;
	//if ((SecurityOptsFlag & 0x1F0) != 0)
	//{
	//	//SecurityOptsFlag &= 0xFFFFFE0F;
	//	optsFlag = 96;
	//}
	SetOptions(MessageOptionFlag::Cancel | MessageOptionFlag::ShutDownFrame | MessageOptionFlag::Accessibility);
	//if (!_IsSwitchUserAllowed())
	//    SecurityOptsFlag &= ~0x200u;
	bool bPastFirst = false;
	for (int i = 0; i < ARRAYSIZE(secOptsFlags); ++i)
	{
		SecurityOptionsFlags& opt = secOptsFlags[i];
		DirectUI::Element* button = FindDescendent(DirectUI::StrToID(opt.id));
		if ((SecurityOptsFlag & opt.flag) != 0)
		{
			button->SetVisible(true);
			button->SetLayoutPos(-1);
			if (!bPastFirst)
			{
				bPastFirst = true;
				button->SetKeyFocus();
			}
		}
		else
		{
			button->SetVisible(false);
			button->SetLayoutPos(-3);
		}
	}
	if (!bPastFirst)
		m_Cancel->SetKeyFocus();

	if (cookie)
		EndDefer(cookie);
}

HRESULT CLogonFrame::OnSecurityOptionSelected(LC::LogonUISecurityOptions SecurityOpt)
{
	ComPtr<LC::ILogonUISecurityOptionsResultFactory> factory;
	RETURN_IF_FAILED(WF::GetActivationFactory(
		Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_LogonUISecurityOptionsResult).Get(), &factory)); // 101

	ComPtr<LC::ILogonUISecurityOptionsResult> optionResult;
	RETURN_IF_FAILED(factory->CreateSecurityOptionsResult(SecurityOpt, LC::LogonUIShutdownChoice_None, &optionResult)); // 104

	RETURN_IF_FAILED(m_SecurityOptionsCompletion->GetResult().Set(optionResult.Get())); // 106

	m_SecurityOptionsCompletion->Complete(S_OK);
	m_SecurityOptionsCompletion.reset();

	return S_OK;
}

void CLogonFrame::ShowStatusMessage(const wchar_t* message)
{
	return _DisplayStatusMessage(message, true);
}

HRESULT CLogonFrame::_Initialize(CLogonNativeHWNDHost* Host, DirectUI::Element* pParent, DWORD* DeferCookie)
{
    m_nativeHost = Host;

    RETURN_IF_FAILED(DirectUI::HWNDElement::Initialize(m_nativeHost->GetHWND(),1,4,nullptr, DeferCookie));

    RETURN_IF_FAILED(CreateStyleParser(&m_xmlParser));

    DWORD defer;
    StartDefer(&defer);

    auto deferCleaner = wil::scope_exit([&] { EndDefer(defer); });

    DirectUI::Layout* fillLayout;
    RETURN_IF_FAILED(DirectUI::FillLayout::Create(&fillLayout));
    auto layoutCleaner = wil::scope_exit([&] { fillLayout->Destroy(); });

    RETURN_IF_FAILED(SetLayout(fillLayout));

    DirectUI::Element* out;
    RETURN_IF_FAILED(m_xmlParser->CreateElement(L"MainFrame",this,0,0,&out));

    m_Status = FindDescendent(DirectUI::StrToID(L"Status"));
    m_StatusText = FindDescendent(DirectUI::StrToID(L"StatusText"));
    m_WaitAnimation = FindDescendent(DirectUI::StrToID(L"WaitAnimation"));
    m_Locked = FindDescendent(DirectUI::StrToID(L"Locked"));
    m_Window = FindDescendent(DirectUI::StrToID(L"Window"));
    m_MessageFrame = m_Window->FindDescendent(DirectUI::StrToID(L"MessageFrame"));
    m_FullMessageFrame = m_Window->FindDescendent(DirectUI::StrToID(L"FullMessageFrame"));
    m_ShortMessageFrame = m_Window->FindDescendent(DirectUI::StrToID(L"ShortMessageFrame"));
    m_ConnectMessageFrame = m_Window->FindDescendent(DirectUI::StrToID(L"ConnectMessageFrame"));
    m_SecurityOptions = m_Window->FindDescendent(DirectUI::StrToID(L"SecurityOptions"));
    m_SwitchUser = m_Window->FindDescendent(DirectUI::StrToID(L"SwitchUser"));
    m_OtherTiles = m_Window->FindDescendent(DirectUI::StrToID(L"OtherTiles"));
    m_Ok = m_Window->FindDescendent(DirectUI::StrToID(L"Ok"));
    m_Yes = m_Window->FindDescendent(DirectUI::StrToID(L"Yes"));
    m_No = m_Window->FindDescendent(DirectUI::StrToID(L"No"));
    m_Cancel = m_Window->FindDescendent(DirectUI::StrToID(L"Cancel"));
    m_Options = FindDescendent(DirectUI::StrToID(L"Options"));
    m_ShutDownFrame = m_Options->FindDescendent(DirectUI::StrToID(L"ShutDownFrame"));
    m_ShowPLAP = m_Options->FindDescendent(DirectUI::StrToID(L"ShowPLAP"));
    m_DisconnectPLAP = m_Options->FindDescendent(DirectUI::StrToID(L"DisconnectPLAP"));
    m_Accessibility = m_Options->FindDescendent(DirectUI::StrToID(L"Accessibility"));

    DirectUI::Element* InsideWindow = m_Window->FindDescendent(DirectUI::StrToID(L"InsideWindow"));
    InsideWindow->Remove(m_SecurityOptions);
    SetBackgroundColor(_ShowBackgroundBitmap() != false ? 0xFF000000 : 0xFF7A5F1D);

    SetBackgroundGraphics();
    _SetBrandingGraphic();
    SetVisible(true);
    SetActive(7);

    SetOptions(MessageOptionFlag::None);
    m_nativeHost->Host(this);

    RETURN_IF_FAILED(_InitializeUserLists());

    //todo: handle the other extra stuff like high contrast
    layoutCleaner.release();
    return S_OK;
}

HRESULT CLogonFrame::_InitializeUserLists()
{
	m_LogonUserList = (UserList*)FindDescendent(DirectUI::StrToID(L"LogonUserList"));
	m_PLAPUserList = (UserList*)FindDescendent(DirectUI::StrToID(L"PLAPUserList"));

	RETURN_IF_FAILED(m_LogonUserList->Configure(m_xmlParser));
	RETURN_IF_FAILED(m_PLAPUserList->Configure(m_xmlParser));

	m_PLAPUserList->m_scenario = m_nativeHost->m_scenario; // no plap scenario here, oh well;
	m_LogonUserList->m_scenario = m_nativeHost->m_scenario;

	m_activeUserList = m_LogonUserList;

	return S_OK;
}

//TODO:
bool CLogonFrame::_IsInstallUpdatesAndShutdownAllowed()
{
	return true;
}

//TODO:
void CLogonFrame::_SetSoftKeyboardAllowed(bool allowed)
{
}

static HBITMAP BrandingLoadImage(const wchar_t* a1, __int64 a2, UINT a3, int a4, int a5, UINT a6)
{
	static auto fBrandingLoadImage = reinterpret_cast<HBITMAP(__fastcall*)(const wchar_t* a1, __int64 a2, UINT a3, int a4, int a5, UINT a6)>(GetProcAddress(LoadLibrary(L"winbrand.dll"), "BrandingLoadImage"));
	if (fBrandingLoadImage)
		return fBrandingLoadImage(a1, a2, a3, a4, a5, a6);

	return 0;
}

void CLogonFrame::_SetBrandingGraphic()
{
	auto brandingElement = FindDescendent(DirectUI::StrToID(L"Branding"));
	if (!brandingElement) return;

	const int brandingSizes[3][2] = { {122,350}, {1122,438} ,{2122,525} };

	int residToUse = 122;
	int lastdist = 9999999;
	int DPI = GetDpiForSystem();

	int scalecompare = MulDiv(350, DPI, 96);/* 350 * (DPI / 96);*/
	for (int i = 0; i < 3; ++i)
	{
		auto pair = brandingSizes[i];
		int resid = pair[0];
		int reso = pair[1];

		int dist = abs(reso - scalecompare);
		if (dist < lastdist)
		{
			lastdist = dist;
			residToUse = resid;
		}
	}

	HBITMAP bitmap = BrandingLoadImage(L"Basebrd", 122, 0, 0, 0, 0); //hardcode to 122, seems like brandingloadimage has stuff to handle dpi on its own
	if (!bitmap)
	{
		bitmap = LoadBitmapW(HINST_THISCOMPONENT, MAKEINTRESOURCEW(residToUse));
	}

	auto graphic = DirectUI::Value::CreateGraphic(bitmap, (unsigned char)2, (unsigned int)0xFFFFFFFF, false, false, false);
	if (!graphic)
	{
		bitmap = LoadBitmapW(HINST_THISCOMPONENT, MAKEINTRESOURCEW(residToUse));
		graphic = DirectUI::Value::CreateGraphic(bitmap, (unsigned char)2, (unsigned int)0xFFFFFFFF, false, false, false);
	}
	if (!graphic)
	{
		DeleteObject(bitmap);
		return;
	}

	brandingElement->SetValue(DirectUI::Element::ContentProp, 1, graphic);
	graphic->Release();
}

void CLogonFrame::SetOptions(MessageOptionFlag optionsFlag)
{
	struct OptionFlags
	{
		MessageOptionFlag flag;
		DirectUI::Element* Option;
	};

	DWORD cookie;
	StartDefer(&cookie);

	bool bAllowSwitchUser = _IsSwitchUserAllowed();

	//OptionFlags opts[] = { {1,m_SwitchUser},{2,m_OtherTiles},{4,m_Ok},{8,m_Yes},{16,m_No},{32,m_Cancel},{64,m_ShutDownFrame},{128,m_ShowPLAP},{256,m_Accessibility},{512,m_DisconnectPLAP}};
	OptionFlags opts[] =
		{
			{MessageOptionFlag::SwitchUser,m_SwitchUser},
			{MessageOptionFlag::OtherTiles,m_OtherTiles},
			{MessageOptionFlag::Ok,m_Ok},
			{MessageOptionFlag::Yes,m_Yes},
			{MessageOptionFlag::No,m_No},
			{MessageOptionFlag::Cancel,m_Cancel},
			{MessageOptionFlag::ShutDownFrame,m_ShutDownFrame},
			{MessageOptionFlag::ShowPLAP,m_ShowPLAP},
			{MessageOptionFlag::Accessibility,m_Accessibility},
			{MessageOptionFlag::DisconnectPLAP,m_DisconnectPLAP}
		};
	for (int i = 0; i < 10; ++i)
	{
		OptionFlags& opt = opts[i];

		if (opt.Option == m_SwitchUser && bAllowSwitchUser == false)
			continue;

		bool Enable = (optionsFlag & opt.flag) != MessageOptionFlag::None;
		opt.Option->SetVisible(Enable);
		opt.Option->SetEnabled(Enable);
	}
	if ((optionsFlag & MessageOptionFlag::ShutDownFrame) != MessageOptionFlag::None && m_ShutDownFrame)
	{
		m_ShutDownFrame->FindDescendent(DirectUI::StrToID(L"ShutDown"))->SetSelected(_IsInstallUpdatesAndShutdownAllowed());

	}
	if (cookie)
		EndDefer(cookie);
}

void CLogonFrame::_SelectMode(DirectUI::Element* elementToHost, bool isVisible)
{
	DWORD cookie = 0;
	StartDefer(&cookie);

	DirectUI::Element* currentWindow = m_CurrentWindow;
	if (currentWindow)
	{
		currentWindow->SetVisible(false);
		m_CurrentWindow->SetEnabled(false);
	}

	m_CurrentWindow = elementToHost;
	elementToHost->SetVisible(true);
	m_CurrentWindow->SetEnabled(true);

	HWND hwnd = GetHWND();
	bool shouldEnable = false;
	if (isVisible)
	{
		EnableWindow(hwnd, true);
		SetActive(7);

		m_Window->SetVisible(true);
		shouldEnable = true;
	}
	else
	{
		EnableWindow(hwnd, false);
		EndMenu();
		SetActive(0);

		m_Window->SetVisible(false);
		shouldEnable = false;
	}

	m_Window->SetEnabled(shouldEnable);
	_SetSoftKeyboardAllowed(false);

	if (cookie)
		EndDefer(cookie);
}

static bool g_ShowCursor = true;

void CLogonFrame::_ShowCursor(bool bShow)
{
	if ( bShow != g_ShowCursor )
	{
		g_ShowCursor = bShow;
		if ( GetSystemMetrics(SM_MOUSEPRESENT) )
		{
			if ( !GetSystemMetrics(SM_REMOTESESSION) )
				ShowCursor(bShow);
		}
	}
}

void CLogonFrame::_DisplayStatusMessage(const wchar_t* message, bool showSpinner)
{
	DWORD cookie;
	StartDefer(&cookie);

	_SelectMode(m_Status, false);
	//_ShowCursor(!showSpinner);
	m_WaitAnimation->SetVisible(showSpinner);

	SetContentAndAcc(m_StatusText, message);
	SetOptions(MessageOptionFlag::None);

	if (cookie)
		EndDefer(cookie);
}

void CLogonFrame::SwitchToUserList(class UserList* userList)
{
	DWORD cookie;
	StartDefer(&cookie);

	m_activeUserList->HideAllTiles();

	m_activeUserList = userList;

	_SelectMode(userList, true);
	userList->m_bIsActive = true;

	//userList->AddTestTiles();
	userList->_ShowEnumeratedTilesWorker(-1);

	userList->FindAndSetKeyFocus();
	userList->EnableList();

	userList->SetActive(7);

	SetOptions(MessageOptionFlag::Accessibility | MessageOptionFlag::ShutDownFrame);

	//ShowPLAP->SetVisible(userList == LogonUserList);

	userList->SetVisible(true);
	userList->_SetUnzoomedWidth();

	if (cookie)
		EndDefer(cookie);
}

void CLogonFrame::DisplayLogonDialog(const wchar_t* messageCaptionContent, const wchar_t* messageContent, WORD flags,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
	m_MessageDisplayResultCompletion = wil::make_unique_nothrow<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>>(completion);
	if (m_MessageDisplayResultCompletion.get() == nullptr)
		return;

	_DisplayLogonDialog(messageCaptionContent, messageContent, flags);
}

void CLogonFrame::DisplayLogonDialog(const wchar_t* messageCaptionContent, const wchar_t* messageContent, WORD flags)
{
	_DisplayLogonDialog(messageCaptionContent, messageContent, flags);

}

HRESULT CLogonFrame::OnMessageOptionPressed(MessageOptionFlag flag)
{
	UINT messageCode;
	switch (flag)
	{
	case MessageOptionFlag::Ok:
		messageCode = 1;
		break;
	case MessageOptionFlag::Cancel:
		messageCode = 2;
		break;
	case MessageOptionFlag::Yes:
		messageCode = 6;
		break;
	case MessageOptionFlag::No:
		messageCode = 7;
		break;

	default:
		RETURN_HR(E_INVALIDARG); // 111
	}

	ComPtr<LC::IMessageDisplayResultFactory> factory;
	RETURN_IF_FAILED(WF::GetActivationFactory(
		Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_MessageDisplayResult).Get(), &factory));

	ComPtr<LC::IMessageDisplayResult> messageResult;
	RETURN_IF_FAILED(factory->CreateMessageDisplayResult(messageCode, &messageResult));

	RETURN_IF_FAILED(m_MessageDisplayResultCompletion->GetResult().Set(messageResult.Get()));

	m_MessageDisplayResultCompletion->Complete(S_OK);
	m_MessageDisplayResultCompletion.reset();

	return S_OK;
}

void CLogonFrame::ShowLockedScreen()
{
	DWORD cookie;
	StartDefer(&cookie);

	SetBackgroundGraphics();
	_SelectMode(m_Locked, true);
	SetOptions(MessageOptionFlag::Accessibility);

	HWND hwnd = GetHWND();
	EnableWindow(hwnd, true);

	auto LockedMessage = m_Locked->FindDescendent(DirectUI::StrToID(L"LockedMessage"));
	auto LockedSubMessage = m_Locked->FindDescendent(DirectUI::StrToID(L"LockedSubMessage"));

	SetContentAndAccFromResources(LockedMessage,12007,12007);
	SetContentAndAcc(LockedSubMessage, L"");
	SetActive(3);

	if (cookie)
		EndDefer(cookie);
}

static bool LoadIconAsContent(DirectUI::Element* elm, const wchar_t* lpIconName)
{
	HICON IconW = LoadIconW(nullptr, lpIconName);
	if (IconW)
	{
		DirectUI::Value* Graphic = DirectUI::Value::CreateGraphic(IconW, 0, 0, 0);
		if (Graphic)
		{
			elm->SetValue(DirectUI::Element::ContentProp, 1, Graphic);
			elm->SetVisible(true);

			Graphic->Release();
			return true;
		}
	}
	return false;
}

void CLogonFrame::_DisplayLogonDialog(const wchar_t* messageCaptionContent, const wchar_t* messageContent, WORD flags)
{
	LPWSTR iconId;
    int iconFlags = flags & 0xF0;
	if (iconFlags == 16)
	{
		iconId = IDI_ERROR;
	}
	else if (iconFlags == 48)
	{
		iconId = IDI_WARNING;
	}
	else
	{
		iconId = IDI_INFORMATION;
		if (iconFlags != 64)
			iconId = 0;
	}

    DirectUI::Element* ButtonToFocus;

    MessageOptionFlag optsFlags;
	switch (flags & 0xF)
	{
	case 1:
		optsFlags = MessageOptionFlag::Ok | MessageOptionFlag::Cancel;
		if ((flags & 0x100) == 0)
		{
			ButtonToFocus = m_Ok;
			break;
		}
		ButtonToFocus = m_Cancel;
		break;
	case 3:
		optsFlags = MessageOptionFlag::Cancel | MessageOptionFlag::Yes | MessageOptionFlag::No;
		if ((flags & 0x100) != 0)
		{
			ButtonToFocus = m_No;
			break;
		}
		if ((flags & 0x200) == 0)
		{
			ButtonToFocus = m_Yes;
			break;
		}
		ButtonToFocus = m_Cancel;
		break;
	case 4:
		optsFlags = MessageOptionFlag::Yes | MessageOptionFlag::No;
		if ((flags & 0x100) != 0)
		{
			ButtonToFocus = m_No;
			break;
		}
		ButtonToFocus = m_Yes;
		break;
	case 6:
		optsFlags = MessageOptionFlag::Cancel;
		ButtonToFocus = m_Cancel;
		break;
	default:
		optsFlags = MessageOptionFlag::Ok;
		ButtonToFocus = m_Ok;
		break;
	}

    DWORD cookie;
    StartDefer(&cookie);
    _SelectMode(m_MessageFrame,true);

	SetForegroundWindow(m_nativeHost->GetHWND());
    m_ConnectMessageFrame->SetLayoutPos(-3);

    DirectUI::Element* element;

	if (!messageCaptionContent || !*messageCaptionContent)
	{
        m_FullMessageFrame->SetLayoutPos (-3);
        m_ShortMessageFrame->SetLayoutPos( -1);
        m_ShortMessageFrame->SetVisible  ( 1);
		element = m_ShortMessageFrame->FindDescendent(DirectUI::StrToID(L"ShortMessage"));
		if (!messageContent)
		{
			SetOptions(optsFlags);
			ButtonToFocus->SetKeyFocus();
			if (cookie)
				EndDefer(cookie);
			return;
		}

        SetContentAndAcc(element, messageContent);
		DirectUI::Element* ShortIcon = m_ShortMessageFrame->FindDescendent(DirectUI::StrToID(L"ShortIcon"));
		if (iconId == 0)
			iconId = IDI_INFORMATION;
		bool IconAsContent = LoadIconAsContent(ShortIcon, iconId);
		ShortIcon->SetLayoutPos(IconAsContent != 0 ? -1 : -3);

		SetOptions(optsFlags);
		ButtonToFocus->SetKeyFocus();
		if (cookie)
			EndDefer(cookie);

		return;
	}
	if (!messageContent || !*messageContent)
	{
		m_FullMessageFrame->SetLayoutPos(-3);
		m_ShortMessageFrame->SetLayoutPos( -1);
		m_ShortMessageFrame->SetVisible( 1);
		element = m_ShortMessageFrame->FindDescendent(DirectUI::StrToID(L"ShortMessage"));

		SetContentAndAcc(element, messageCaptionContent);
		DirectUI::Element* ShortIcon = m_ShortMessageFrame->FindDescendent(DirectUI::StrToID(L"ShortIcon"));
		if (iconId == 0)
			iconId = IDI_INFORMATION;
		bool IconAsContent = LoadIconAsContent(ShortIcon, iconId);
        ShortIcon->SetLayoutPos(IconAsContent != 0 ? -1 : -3);

		SetOptions(optsFlags);
		ButtonToFocus->SetKeyFocus();
		if (cookie)
			EndDefer(cookie);

		return;
	}
    m_FullMessageFrame->SetLayoutPos(-1);
    m_ShortMessageFrame->SetLayoutPos(-3);
    m_FullMessageFrame->SetVisible(true);

	DirectUI::Element* FullIcon = m_FullMessageFrame->FindDescendent(DirectUI::StrToID(L"FullIcon"));
    int layoutPos;
	if (LoadIconAsContent(FullIcon, iconId))
		layoutPos = -1;
	else
		layoutPos = -3;

    FullIcon->SetLayoutPos(layoutPos);

	DirectUI::Element* MessageCaption = m_FullMessageFrame->FindDescendent(DirectUI::StrToID(L"MessageCaption"));
	SetContentAndAcc(MessageCaption, messageCaptionContent);

	DirectUI::Element* Message = m_FullMessageFrame->FindDescendent(DirectUI::StrToID(L"Message"));
	SetContentAndAcc(Message, messageContent);

	SetOptions(optsFlags);
    ButtonToFocus->SetKeyFocus();
	if (cookie)
		EndDefer(cookie);
}
