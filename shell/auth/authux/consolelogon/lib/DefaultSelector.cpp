#include "pch.h"

#include "DefaultSelector.h"

#include <windows.foundation.collections.h>

#include "duiutil.h"
#include "logonguids.h"
#include "windowscollections.h"

using namespace Microsoft::WRL;

HRESULT DefaultSelector::RuntimeClassInitialize(LC::IUserSettingManager* userSettingManager, LC::LogonUIRequestReason reason)
{
	m_userSettingManager = userSettingManager;
	m_reason = reason;

	ComPtr<Windows::Foundation::Collections::Internal::AgileVector<GUID>> preferredProviders;
	RETURN_IF_FAILED(Windows::Foundation::Collections::Internal::AgileVector<GUID>::Make(&preferredProviders)); // 29
	RETURN_IF_FAILED(preferredProviders->Vector::Append(CLSID_PasswordCredentialProvider)); // 30
	RETURN_IF_FAILED(preferredProviders->Vector::Append(CLSID_CWLIDCredentialProvider)); // 31
	RETURN_IF_FAILED(preferredProviders->Vector::GetView(&m_preferredProviders)); // 32

	ComPtr<Windows::Foundation::Collections::Internal::AgileVector<GUID>> excludedProviders;
	RETURN_IF_FAILED(Windows::Foundation::Collections::Internal::AgileVector<GUID>::Make(&excludedProviders)); // 35
	RETURN_IF_FAILED(excludedProviders->Vector::Append(CLSID_PicturePasswordCredentialProvider)); // 36
	RETURN_IF_FAILED(excludedProviders->Vector::GetView(&m_excludedProviders)); // 37

	return S_OK;
}

HRESULT DefaultSelector::get_UseLastLoggedOnProvider(BOOLEAN* value)
{
	*value = TRUE;
	return S_OK;
}

HRESULT DefaultSelector::get_PreferredProvidersDup(WFC::IVectorView<GUID>** ppValue)
{
	RETURN_IF_FAILED(m_preferredProviders.CopyTo(ppValue)); // 51
	return S_OK;
}

HRESULT DefaultSelector::get_PreferredProviders(WFC::IVectorView<GUID>** ppValue)
{
	RETURN_IF_FAILED(m_preferredProviders.CopyTo(ppValue)); // 51
	return S_OK;
}

HRESULT DefaultSelector::get_ExcludedProviders(WFC::IVectorView<GUID>** ppValue)
{
	RETURN_IF_FAILED(m_excludedProviders.CopyTo(ppValue)); // 58
	return S_OK;
}

HRESULT DefaultSelector::get_DefaultUserSid(HSTRING* value)
{
	RETURN_IF_FAILED(m_userSettingManager->get_UserSid(value)); // 64
	return S_OK;
}

HRESULT DefaultSelector::AllowAutoSubmitOnSelection(LCPD::IUser* user, BOOLEAN* value)
{
	*value = TRUE;

	BOOL forceAutoLogon = FALSE;
	bool isForceAutoLogon = SUCCEEDED(SHRegGetBOOLWithREGSAM(
		HKEY_LOCAL_MACHINE,
		L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
		L"ForceAutoLogon", 0, &forceAutoLogon)) && forceAutoLogon;

	bool isChangePassword = m_reason == LC::LogonUIRequestReason_LogonUIChange;

	BOOLEAN isFirstLogonAfterSignOutOrSwitchUser = FALSE;
	m_userSettingManager->get_IsFirstLogonAfterSignOutOrSwitchUser(&isFirstLogonAfterSignOutOrSwitchUser);

	BOOLEAN isUserLoggedOn = 0;
	RETURN_IF_FAILED(user->get_IsLoggedIn(&isUserLoggedOn)); // 77

	bool isLoggedOnUserPresentUnlockOrLogon =
		(m_reason == LC::LogonUIRequestReason_LogonUIUnlock || m_reason == LC::LogonUIRequestReason_LogonUILogon) && isUserLoggedOn;
	bool isRemoteSessionUserNotLoggedOn = GetSystemMetrics(SM_REMOTESESSION) && !isUserLoggedOn;

	*value = isForceAutoLogon
		|| isChangePassword
		|| (!isFirstLogonAfterSignOutOrSwitchUser && !isLoggedOnUserPresentUnlockOrLogon)
		|| isRemoteSessionUserNotLoggedOn;
	return S_OK;
}
