#include "pch.h"
#include "authuxlockaction.h"

const __declspec(selectany) _Null_terminated_ WCHAR ConsoleLockAsyncAction[] = L"Windows.Foundation.IAsyncAction Windows.Internal.UI.Logon.Controller.ConsoleLockAction";

AuthUXLockAction::AuthUXLockAction()
{
}

AuthUXLockAction::~AuthUXLockAction()
{
}

HRESULT AuthUXLockAction::RuntimeClassInitialize(HSTRING domainName, HSTRING userName, HSTRING friendlyName)
{
	RETURN_IF_FAILED(m_domainName.Set(domainName)); // 16
	RETURN_IF_FAILED(m_userName.Set(userName)); // 17
	RETURN_IF_FAILED(m_friendlyName.Set(friendlyName)); // 18
	RETURN_IF_FAILED(Start()); // 19
	//RETURN_IF_FAILED(TriggerUnlock()); // 19

	return S_OK;
}

HRESULT AuthUXLockAction::TriggerUnlock()
{
	RETURN_IF_FAILED(FireCompletion()); //26

	return S_OK;
}

HRESULT AuthUXLockAction::SyncBackstop()
{
	return S_OK;
}

HRESULT AuthUXLockAction::CheckCompletion()
{
	return S_OK;
}

HRESULT AuthUXLockAction::get_VisualOwner(LC::LockDisplayOwner* value)
{
	*value = LC::LockDisplayOwner_LogonUX;
	return S_OK;
}

HRESULT AuthUXLockAction::get_DomainName(HSTRING* value)
{
	*value = nullptr;
	if (m_domainName.IsEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_domainName.CopyTo(value)); // 47

	return S_OK;
}

HRESULT AuthUXLockAction::get_UserName(HSTRING* value)
{
	*value = nullptr;
	if (m_userName.IsEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_userName.CopyTo(value)); // 57

	return S_OK;
}

HRESULT AuthUXLockAction::get_FriendlyName(HSTRING* value)
{
	*value = nullptr;
	if (m_friendlyName.IsEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_friendlyName.CopyTo(value)); // 67

	return S_OK;
}

HRESULT AuthUXLockAction::get_RequireSecureGesture(bool* value)
{
	*value = true;
	return S_OK;
}

HRESULT AuthUXLockAction::get_ShowSpeedBump(bool* value)
{
	*value = false;
	return S_OK;
}

HRESULT AuthUXLockAction::get_RequireSecureGestureString(HSTRING* value)
{
	*value = nullptr;
	return S_OK;
}

HRESULT AuthUXLockAction::get_SpeedBumpString(HSTRING* value)
{
	*value = nullptr;
	return S_OK;
}

HRESULT AuthUXLockAction::get_IsLostMode(bool* value)
{
	*value = false;
	return S_OK;
}

HRESULT AuthUXLockAction::get_LostModeMessage(HSTRING* value)
{
	*value = nullptr;
	return S_OK;
}

HRESULT AuthUXLockAction::add_UserActivity(WF::ITypedEventHandler<LC::ILockInfo*, LC::LockActivity>* handler,
                                            EventRegistrationToken* token)
{
	token->value = 0;
	RETURN_IF_FAILED(m_userActivityEvent.Add(handler,token)); // 99

	return S_OK;
}

HRESULT AuthUXLockAction::remove_UserActivity(EventRegistrationToken token)
{
	RETURN_IF_FAILED(m_userActivityEvent.Remove(token)); // 105
	return S_OK;
}

HRESULT AuthUXLockAction::put_Completed(WF::IAsyncOperationCompletedHandler<HSTRING>* pRequestHandler)
{
	RETURN_IF_FAILED(PutOnComplete(pRequestHandler)); // 112
	return S_OK;
}

HRESULT AuthUXLockAction::get_Completed(WF::IAsyncOperationCompletedHandler<HSTRING>** ppRequestHandler)
{
	*ppRequestHandler = nullptr;
	RETURN_IF_FAILED(GetOnComplete(ppRequestHandler)); // 119

	return S_OK;
}

HRESULT AuthUXLockAction::GetResults(HSTRING* results)
{
	return S_OK;
}

HRESULT AuthUXLockAction::OnStart()
{
	return S_OK;
}

void AuthUXLockAction::OnClose()
{
}

void AuthUXLockAction::OnCancel()
{
	FireCompletion();
}
