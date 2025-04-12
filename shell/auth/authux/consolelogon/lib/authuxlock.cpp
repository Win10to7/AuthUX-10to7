#include "pch.h"

#include "authuxlockaction.h"
#include "duiutil.h"
#include "logonframe.h"

using namespace Microsoft::WRL;

using namespace ABI::Windows::Foundation;
using namespace Windows::Internal::UI::Logon::Controller;
using namespace Windows::Internal::UI::Logon::CredProvData;

//extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleLockScreen[] = L"Windows.Internal.UI.Logon.Controller.ConsoleLockScreen";
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleLockScreen[] = L"Windows.Internal.UI.Logon.Controller.LockScreenHost";

class AuthUXLock final
	: public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>
		, ILockScreenHost
		, FtmBase
	>
{
	InspectableClass(RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleLockScreen, FullTrust);

public:
	AuthUXLock();
	~AuthUXLock() override;

	STDMETHODIMP ShowWebDialogAsync(HSTRING a1, void** a2) override;
	STDMETHODIMP LockAsync(LockOptions options, HSTRING domainName, HSTRING userName, HSTRING friendlyName, HSTRING unk, bool* setWin32kForegroundHardening, IUnlockTrigger** ppAction) override;
	STDMETHODIMP Reset() override;
	STDMETHODIMP PreShutdown() override;
};

AuthUXLock::AuthUXLock()
{
}

AuthUXLock::~AuthUXLock()
{
}

HRESULT AuthUXLock::ShowWebDialogAsync(HSTRING a1, void** a2)
{
	return S_OK;
}

HRESULT AuthUXLock::LockAsync(LockOptions options, HSTRING domainName, HSTRING userName, HSTRING friendlyName, HSTRING unk,
                               bool* setWin32kForegroundHardening, IUnlockTrigger** ppAction)
{
	*ppAction = nullptr;
	*setWin32kForegroundHardening = false;

	BOOL disableCAD = TRUE;
	RETURN_IF_FAILED(SHRegGetBOOLWithREGSAM(HKEY_LOCAL_MACHINE,L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",L"DisableCAD",0,&disableCAD));
	RETURN_HR_IF(E_NOTIMPL, disableCAD == TRUE);

	RETURN_HR_IF(E_NOTIMPL, (options & LockOptions_SecureDesktop) == 0);

	RETURN_IF_FAILED(MakeAndInitialize<AuthUXLockAction>(ppAction,domainName,userName,friendlyName)); // 36


	return S_OK;
}

HRESULT AuthUXLock::Reset()
{
	return S_OK;
}

HRESULT AuthUXLock::PreShutdown()
{
	return S_OK;
}

ActivatableClass(AuthUXLock);
