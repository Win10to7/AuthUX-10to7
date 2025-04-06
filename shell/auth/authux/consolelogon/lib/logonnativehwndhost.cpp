#include "pch.h"
#include "logonnativehwndhost.h"

CLogonNativeHWNDHost::CLogonNativeHWNDHost()
	: m_scenario(LCPD::CredProvScenario_Logon)
{

}

HRESULT HRESULTFromLastErrorError()
{
	DWORD lastErrorDWORD = GetLastError();
	if (!lastErrorDWORD)
		lastErrorDWORD = 1;
	HRESULT result = static_cast<WORD>(lastErrorDWORD) | 0x80070000;
	if (lastErrorDWORD <= 0)
		return static_cast<HRESULT>(lastErrorDWORD);
	return result;
}

HRESULT CLogonNativeHWNDHost::Create(int dx, int dy, int dwidth, int dheight, CLogonNativeHWNDHost** out)
{
	CLogonNativeHWNDHost* host = DirectUI::HNewAndZero<CLogonNativeHWNDHost>();
	if (!host)
		return E_OUTOFMEMORY;

	HRESULT er = S_OK;

	WNDCLASSEXW wndClass = {};

	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.style = CS_GLOBALCLASS;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wndClass.lpfnWndProc = DefWindowProcW;
	wndClass.lpszClassName = L"AUTHUI.DLL: LogonUI Logon Window";
	if (!RegisterClassExW(&wndClass))
	{
		er = HRESULTFromLastErrorError();
		if (FAILED(er))
		{
			host->Destroy();
			return er;
		}
	}

	er = host->Initialize(L"AUTHUI.DLL: LogonUI Logon Window", L"Windows Logon",nullptr,nullptr,dx,dy,dwidth,dheight, 0, 0x80000000 | WS_VISIBLE,nullptr,0);
	if (SUCCEEDED(er))
	{
		*out = host;
	}
	else
	{
		*out = nullptr;
		host->Destroy();
	}
	return er;
}
