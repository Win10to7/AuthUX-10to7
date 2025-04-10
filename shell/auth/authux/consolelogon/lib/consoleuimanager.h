#pragma once

#include "pch.h"

#include "NotificationDispatcher.h"

class ConsoleUIManager
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
		, IUnknown
	>
{
public:
	ConsoleUIManager();

	HRESULT Initialize();
	HRESULT StartUI();
	HRESULT StopUI();

protected:
	~ConsoleUIManager() = default;

	HRESULT EnsureUIStarted();

	Microsoft::WRL::ComPtr<INotificationDispatcher> m_Dispatcher;

private:

	static DWORD WINAPI s_UIThreadHostStartThreadProc(void* parameter);
	HRESULT UIThreadHostStartThreadProc();

	static DWORD WINAPI s_UIThreadHostThreadProc(void* parameter);
	DWORD UIThreadHostThreadProc();

	Microsoft::WRL::Wrappers::SRWLock m_lock;

	wil::unique_handle m_UIThreadHandle;
	HRESULT m_UIThreadInitResult;
	wil::unique_handle m_UIThreadQuitEvent;
};
