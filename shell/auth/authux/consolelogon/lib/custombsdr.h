#pragma once

#include "pch.h"
#include <vector>

using namespace Windows::Internal::UI::Logon::Controller;

typedef void(*Resolve_t)(BlockedShutdownResolution);

class CustomBSDR
{
public:
	static void Start(Resolve_t resolve, LogonUIState state);
	static void Hide();
	static void Stop();
	static void AddApplication(IShutdownBlockingApp* app);
	static void RemoveApplication(UINT appId);

private:
	static Resolve_t _resolve;
	static LogonUIState _logonUIState;

	// handles
	static HANDLE hThread;
	static HWND hDlg;
	static HWND hBgWnd;
	static HWND hTitleText;
	static HWND hAppList;
	static HWND hAppListScroll;
	static HWND hScrollBar;
	static HWND hWarningText;
	static HWND hForceButton;
	static HWND hCancelButton;
	static HWND hDescText;
	static HWND hYesButton;
	static HWND hNoButton;
	static HWND hHoverButton;
	static HFONT hTitleFont;
	static HFONT hDescFont;

	// functions
	static void CenterWindow(HWND hWnd);
	static bool IsHighContrast();
	static bool UseClassicScrollbar();
	static HBITMAP LoadAlphaBitmap(UINT resourceId);
	static void DrawSeparator(HDC hdc, LPDRAWITEMSTRUCT pDIS);
	static void DrawButton(HDC hdc, LPDRAWITEMSTRUCT pDIS);
	static void CreateAppTileControls(IShutdownBlockingApp* blockingApp);
	static void RemoveAppTileControls(UINT appId);
	static void UpdateAppListLayout();
	static LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK AppListSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static INT_PTR CALLBACK DlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);

	// bitmaps
	static HBITMAP bgBitmap;
	static HBITMAP separatorBitmap;
	static HBITMAP btnNormalBitmap;
	static HBITMAP btnHoverBitmap;
	static HBITMAP btnPressedBitmap;
	static HBITMAP btnSelectedBitmap;
	static HBITMAP btnSelectedHoverBitmap;

	// variables
	static int bgOffsetX;
	static int bgOffsetY;
	static int bgWidth;
	static int bgHeight;
	static int scrollPos;
	static int totalContentHeight;
	static bool isOnSecureDesktop;

	// app list data stuff
	struct AppTile
	{
		UINT appId;
		BOOLEAN isBlocking;
		HWND hIcon;
		HWND hTitle;
		HWND hBlockReason;
		HBITMAP hIconBitmap;
		Microsoft::WRL::ComPtr<IShutdownBlockingApp> app;
	};
	static std::vector<AppTile> appTiles;
	static std::vector<Microsoft::WRL::ComPtr<IShutdownBlockingApp>> pendingApps;
	static std::vector<Microsoft::WRL::ComPtr<IShutdownBlockingApp>> addQueue;
};
