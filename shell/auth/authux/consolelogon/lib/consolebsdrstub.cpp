#include "pch.h"

using namespace Microsoft::WRL;

using namespace ABI::Windows::Foundation;
using namespace Windows::Internal::UI::Logon::Controller;
using namespace Windows::Internal::UI::Logon::CredProvData;

//extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleBlockedShutdownResolver[] = L"Windows.Internal.UI.Logon.Controller.ConsoleBlockedShutdownResolver";
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleBlockedShutdownResolver[] = L"Windows.Internal.UI.Logon.Controller.BlockedShutdownResolverUX";

class ConsoleBSDRStub
	: public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>
		, IBlockedShutdownResolverUX
		, FtmBase
	>
{
	InspectableClass(RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleBlockedShutdownResolver, FullTrust);

public:
	ConsoleBSDRStub();
	~ConsoleBSDRStub() override;

	HRESULT STDMETHODCALLTYPE Start(IUserSettingManager* settingsManager, ILogonUIStateInfo* stateInfo) override;
	HRESULT STDMETHODCALLTYPE get_ScaleFactor(UINT* value) override;
	HRESULT STDMETHODCALLTYPE get_WasClicked(bool* value) override;
	HRESULT STDMETHODCALLTYPE AddApplication(IShutdownBlockingApp* blockingApp) override;
	HRESULT STDMETHODCALLTYPE RemoveApplication(UINT appid) override;
	HRESULT STDMETHODCALLTYPE add_Resolved(ITypedEventHandler<IBlockedShutdownResolverUX*, BlockedShutdownResolution>* handler, EventRegistrationToken* token) override;
	HRESULT STDMETHODCALLTYPE remove_Resolved(EventRegistrationToken token) override;
	HRESULT STDMETHODCALLTYPE Hide() override;
	HRESULT STDMETHODCALLTYPE Stop() override;
private:
	EventSource<ITypedEventHandler<IBlockedShutdownResolverUX*, BlockedShutdownResolution>> _Resolved;

};

ConsoleBSDRStub::ConsoleBSDRStub()
{
}

ConsoleBSDRStub::~ConsoleBSDRStub()
{
}

HRESULT ConsoleBSDRStub::Start(IUserSettingManager* settingsManager, ILogonUIStateInfo* stateInfo)
{
	return S_OK;
}

HRESULT ConsoleBSDRStub::get_ScaleFactor(UINT* value)
{
	*value = 100;
	return S_OK;
}

HRESULT ConsoleBSDRStub::get_WasClicked(bool* value)
{
	*value = false;
	return S_OK;
}

HRESULT ConsoleBSDRStub::AddApplication(IShutdownBlockingApp* blockingApp)
{
	return _Resolved.InvokeAll(this, BlockedShutdownResolution_Force);
}

HRESULT ConsoleBSDRStub::RemoveApplication(UINT appid)
{
	return _Resolved.InvokeAll(this, BlockedShutdownResolution_Force);
}

HRESULT ConsoleBSDRStub::add_Resolved(
	ITypedEventHandler<IBlockedShutdownResolverUX*, BlockedShutdownResolution>* handler, EventRegistrationToken* token)
{
	token->value = 0;

	return _Resolved.Add(handler,token);
}

HRESULT ConsoleBSDRStub::remove_Resolved(EventRegistrationToken token)
{
	return _Resolved.Remove(token);
}

HRESULT ConsoleBSDRStub::Hide()
{
	return S_OK;
}

HRESULT ConsoleBSDRStub::Stop()
{
	return S_OK;
}

ActivatableClass(ConsoleBSDRStub);
