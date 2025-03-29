#pragma once

#include "pch.h"

extern const __declspec(selectany) _Null_terminated_ WCHAR ConsoleLockAsyncAction[];

class ConsoleLockAction
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		,LC::IUnlockTrigger
		,LC::ILockInfo
		,WF::IAsyncAction
		,WI::AsyncBaseFTM<WF::IAsyncActionCompletedHandler
			, Microsoft::WRL::SingleResult
			, Microsoft::WRL::AsyncCausalityOptions<ConsoleLockAsyncAction,GUID_CAUSALITY_WINDOWS_PLATFORM_ID, WF::Diagnostics::CausalitySource_System>>
		>
{
public:
	ConsoleLockAction();
	~ConsoleLockAction() override;
	
	HRESULT STDMETHODCALLTYPE RuntimeClassInitialize(HSTRING domainName, HSTRING userName, HSTRING friendlyName);
    HRESULT STDMETHODCALLTYPE TriggerUnlock() override;
    HRESULT STDMETHODCALLTYPE SyncBackstop() override;
    HRESULT STDMETHODCALLTYPE CheckCompletion() override;
    HRESULT STDMETHODCALLTYPE get_VisualOwner(LC::LockDisplayOwner* value) override;
    HRESULT STDMETHODCALLTYPE get_DomainName(HSTRING* value) override;
    HRESULT STDMETHODCALLTYPE get_UserName(HSTRING* value) override;
    HRESULT STDMETHODCALLTYPE get_FriendlyName(HSTRING* value) override;
    HRESULT STDMETHODCALLTYPE get_RequireSecureGesture(bool* value) override;
    HRESULT STDMETHODCALLTYPE get_ShowSpeedBump(bool* value) override;
    HRESULT STDMETHODCALLTYPE get_RequireSecureGestureString(HSTRING* value) override;
    HRESULT STDMETHODCALLTYPE get_SpeedBumpString(HSTRING* value) override;
    HRESULT STDMETHODCALLTYPE add_UserActivity(WF::ITypedEventHandler<LC::ILockInfo *, LC::LockActivity>* handler, EventRegistrationToken* token) override;
    HRESULT STDMETHODCALLTYPE remove_UserActivity(EventRegistrationToken token) override;
    HRESULT STDMETHODCALLTYPE put_Completed(WF::IAsyncActionCompletedHandler* pRequestHandler) override;
    HRESULT STDMETHODCALLTYPE get_Completed(WF::IAsyncActionCompletedHandler** ppRequestHandler) override;
    HRESULT STDMETHODCALLTYPE GetResults() override;
    HRESULT OnStart() override;
    void OnClose() override;
    void OnCancel() override;
private:
	Microsoft::WRL::Wrappers::HString m_domainName;
    Microsoft::WRL::Wrappers::HString m_userName;
    Microsoft::WRL::Wrappers::HString m_friendlyName;
    Microsoft::WRL::EventSource<WF::ITypedEventHandler<LC::ILockInfo*, LC::LockActivity>> m_userActivityEvent;
};
