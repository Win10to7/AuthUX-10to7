#include "pch.h"
#include <windowsx.h>
#include <CommCtrl.h>
#include <string>
#include <winerror.h>
#include <wincodec.h>
#include <robuffer.h>
#include <wrl/wrappers/corewrappers.h>
#include <vector>

#pragma	comment(lib, "msimg32.lib")
#pragma comment(lib, "windowscodecs.lib")

#define WM_ADD_APP (WM_USER + 1)
#define WM_REMOVE_APP (WM_USER + 2)

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

	//~ Begin IBlockedShutdownResolverUX Interface
	STDMETHODIMP Start(IUserSettingManager* settingsManager, ILogonUIStateInfo* stateInfo) override;
	STDMETHODIMP get_ScaleFactor(UINT* value) override;
	STDMETHODIMP get_WasClicked(BOOLEAN* value) override;
	STDMETHODIMP AddApplication(IShutdownBlockingApp* blockingApp) override;
	STDMETHODIMP RemoveApplication(UINT appid) override;
	STDMETHODIMP add_Resolved(ITypedEventHandler<IBlockedShutdownResolverUX*, BlockedShutdownResolution>* handler, EventRegistrationToken* token) override;
	STDMETHODIMP remove_Resolved(EventRegistrationToken token) override;
	STDMETHODIMP Hide() override;
	STDMETHODIMP Stop() override;
	//~ End IBlockedShutdownResolverUX Interface
private:
	static EventSource<ITypedEventHandler<IBlockedShutdownResolverUX*, BlockedShutdownResolution>> _Resolved;
	
	static void LogToFile(const wchar_t* message, ...);
	static void CenterWindow(HWND hWnd);
	static bool IsHighContrast();
	static HBITMAP LoadAlphaBitmap(UINT resourceId);
	static HBITMAP StreamToHBITMAP(ABI::Windows::Storage::Streams::IRandomAccessStream* stream);
	static void DrawButton(HDC hdc, LPDRAWITEMSTRUCT pDIS);
	static void CreateAppTileControls(IShutdownBlockingApp* blockingApp);
	static void RemoveAppTileControls(UINT appId);
	static void UpdateAppListLayout();
	static LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK AppListSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static HANDLE hThread;
	static HWND hDlg;
	static HWND hBgWnd;
	static HWND hAppListContainer;
	static HWND hTitleText;
	static HWND hTopSeparator;
	static HWND hAppList;
	static HWND hScrollBar;
	static HWND hBottomSeparator;
	static HWND hWarningText;
	static HWND hForceButton;
	static HWND hCancelButton;
	static HWND hDescText;
	static HWND hYesButton;
	static HWND hNoButton;
	static HWND hHoverButton;
	static HFONT hTitleFont;
	static HFONT hDescFont;
	static INT_PTR CALLBACK DlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
	static HBITMAP bgBitmap;
	static HBITMAP separatorBitmap;
	static HBITMAP btnNormalBitmap;
	static HBITMAP btnHoverBitmap;
	static HBITMAP btnPressedBitmap;
	static HBITMAP btnSelectedBitmap;
	static HBITMAP btnSelectedHoverBitmap;
	static int bgOffsetX;
	static int bgOffsetY;
	static LogonUIState logonUIState;
	static int scrollPos;
	static BOOLEAN wasClicked;

	struct AppTile
	{
		UINT appId;
		BOOLEAN isBlocking;
		HWND hIcon;
		HWND hTitle;
		HWND hBlockReason;
		HBITMAP hIconBitmap;
		ComPtr<IShutdownBlockingApp> app;
	};
	static std::vector<AppTile> appTiles;
	static std::vector<ComPtr<IShutdownBlockingApp>> pendingApps;
	static std::vector<ComPtr<IShutdownBlockingApp>> addQueue;
};

HANDLE ConsoleBSDRStub::hThread = nullptr;
HWND ConsoleBSDRStub::hDlg = nullptr;
HWND ConsoleBSDRStub::hBgWnd = nullptr;
HWND ConsoleBSDRStub::hAppListContainer = nullptr;
HWND ConsoleBSDRStub::hHoverButton = nullptr;
HWND ConsoleBSDRStub::hTitleText = nullptr;
HWND ConsoleBSDRStub::hTopSeparator = nullptr;
HWND ConsoleBSDRStub::hAppList = nullptr;
HWND ConsoleBSDRStub::hScrollBar = nullptr;
HWND ConsoleBSDRStub::hBottomSeparator = nullptr;
HWND ConsoleBSDRStub::hWarningText = nullptr;
HWND ConsoleBSDRStub::hForceButton = nullptr;
HWND ConsoleBSDRStub::hCancelButton = nullptr;
HWND ConsoleBSDRStub::hDescText = nullptr;
HWND ConsoleBSDRStub::hYesButton = nullptr;
HWND ConsoleBSDRStub::hNoButton = nullptr;
HFONT ConsoleBSDRStub::hTitleFont = nullptr;
HFONT ConsoleBSDRStub::hDescFont = nullptr;
HBITMAP ConsoleBSDRStub::bgBitmap = nullptr;
HBITMAP ConsoleBSDRStub::separatorBitmap = nullptr;
HBITMAP ConsoleBSDRStub::btnNormalBitmap = nullptr;
HBITMAP ConsoleBSDRStub::btnHoverBitmap = nullptr;
HBITMAP ConsoleBSDRStub::btnPressedBitmap = nullptr;
HBITMAP ConsoleBSDRStub::btnSelectedBitmap = nullptr;
HBITMAP ConsoleBSDRStub::btnSelectedHoverBitmap = nullptr;
LogonUIState ConsoleBSDRStub::logonUIState = LogonUIState_ShuttingDown;
int ConsoleBSDRStub::bgOffsetX = 0;
int ConsoleBSDRStub::bgOffsetY = 0;
int ConsoleBSDRStub::scrollPos = 0;
BOOLEAN ConsoleBSDRStub::wasClicked = FALSE;
std::vector<ConsoleBSDRStub::AppTile> ConsoleBSDRStub::appTiles;
std::vector<ComPtr<IShutdownBlockingApp>> ConsoleBSDRStub::pendingApps;
std::vector<ComPtr<IShutdownBlockingApp>> ConsoleBSDRStub::addQueue;
EventSource<ITypedEventHandler<IBlockedShutdownResolverUX*, BlockedShutdownResolution>> ConsoleBSDRStub::_Resolved = {};

ConsoleBSDRStub::ConsoleBSDRStub()
{
}

ConsoleBSDRStub::~ConsoleBSDRStub()
{
}

void ConsoleBSDRStub::LogToFile(const wchar_t* message, ...)
{
	FILE* file;
	_wfopen_s(&file, L"C:\\test\\CustomBSDR.log", L"a");
	if (file)
	{
		va_list args;
		va_start(args, message);
		vfwprintf(file, message, args);
		va_end(args);
		fwprintf(file, L"\n");
		fclose(file);
	}
}

void ConsoleBSDRStub::CenterWindow(HWND hWnd)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);
	int windowWidth = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;
	SetWindowPos(hWnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

bool ConsoleBSDRStub::IsHighContrast()
{
	HIGHCONTRASTW highContrast = {};
	highContrast.cbSize = sizeof(HIGHCONTRASTW);
	if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, 0))
	{
		return (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
	}
	return false;
}

HBITMAP ConsoleBSDRStub::LoadAlphaBitmap(UINT resourceId)
{
	HRSRC hResource = FindResourceW(HINST_THISCOMPONENT, MAKEINTRESOURCEW(resourceId), RT_BITMAP);
	if (!hResource)
		return nullptr;

	HGLOBAL hGlobal = LoadResource(HINST_THISCOMPONENT, hResource);
	if (!hGlobal)
		return nullptr;

	void* pResourceData = LockResource(hGlobal);
	if (!pResourceData)
		return nullptr;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)pResourceData;
	BITMAPINFOHEADER* pHeader = &pBitmapInfo->bmiHeader;

	if (pHeader->biBitCount != 32)
	{
		return LoadBitmapW(HINST_THISCOMPONENT, MAKEINTRESOURCEW(resourceId));
	}

	int width = pHeader->biWidth;
	int height = abs(pHeader->biHeight);
	bool isTopDown = pHeader->biHeight < 0;

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hdc = GetDC(nullptr);
	void* pBits = nullptr;
	HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
	ReleaseDC(nullptr, hdc);

	if (hBitmap && pBits)
	{
		DWORD dwBitsOffset = pHeader->biSize;
		if (pHeader->biCompression == BI_BITFIELDS)
		{
			dwBitsOffset += 12;
		}

		unsigned char* pSrc = (unsigned char*)pResourceData + dwBitsOffset;
		DWORD dwSize = width * height * 4;

		if (isTopDown)
		{
			memcpy(pBits, pSrc, dwSize);
		}
		else
		{
			for (int y = 0; y < height; y++)
			{
				memcpy((unsigned char*)pBits + y * width * 4,
					pSrc + (height - 1 - y) * width * 4,
					width * 4);
			}
		}

		unsigned char* pPixels = (unsigned char*)pBits;
		for (int i = 0; i < width * height; i++)
		{
			unsigned char b = pPixels[i * 4 + 0];
			unsigned char g = pPixels[i * 4 + 1];
			unsigned char r = pPixels[i * 4 + 2];
			unsigned char a = pPixels[i * 4 + 3];

			pPixels[i * 4 + 0] = (b * a) / 255;
			pPixels[i * 4 + 1] = (g * a) / 255;
			pPixels[i * 4 + 2] = (r * a) / 255;
		}
	}

	return hBitmap;
}

HBITMAP g_testiconbmp = nullptr;

HBITMAP ConsoleBSDRStub::StreamToHBITMAP(ABI::Windows::Storage::Streams::IRandomAccessStream* stream)
{
	if (!stream)
	{
		LogToFile(L"StreamToHBITMAP: stream is null");
		return nullptr;
	}

	ComPtr<ABI::Windows::Storage::Streams::IInputStream> inputStream;
	if (FAILED(stream->QueryInterface(IID_PPV_ARGS(&inputStream))))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get IInputStream");
		return nullptr;
	}

	UINT64 size;
	if (FAILED(stream->get_Size(&size)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get stream size");
		return nullptr;
	}
	LogToFile(L"StreamToHBITMAP: Stream size=%lld", size);

	ComPtr<ABI::Windows::Storage::Streams::IBufferFactory> bufferFactory;
	HRESULT hr = RoGetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_Storage_Streams_Buffer).Get(), IID_PPV_ARGS(&bufferFactory));
	if (FAILED(hr))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get buffer factory, hr=0x%08X", hr);
		return nullptr;
	}

	ComPtr<ABI::Windows::Storage::Streams::IBuffer> buffer;
	if (FAILED(bufferFactory->Create((UINT32)size, &buffer)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to create buffer");
		return nullptr;
	}

	ComPtr<ABI::Windows::Foundation::IAsyncOperationWithProgress<ABI::Windows::Storage::Streams::IBuffer*, UINT32>> readOperation;
	if (FAILED(inputStream->ReadAsync(buffer.Get(), (UINT32)size, ABI::Windows::Storage::Streams::InputStreamOptions_None, &readOperation)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to start ReadAsync");
		return nullptr;
	}

	ComPtr<ABI::Windows::Foundation::IAsyncInfo> asyncInfo;
	readOperation.As(&asyncInfo);

	AsyncStatus status;
	while (SUCCEEDED(asyncInfo->get_Status(&status)) && status == AsyncStatus::Started)
	{
		Sleep(10);
	}

	if (status != AsyncStatus::Completed)
	{
		LogToFile(L"StreamToHBITMAP: Async operation failed, status=%d", (int)status);
		return nullptr;
	}

	ComPtr<ABI::Windows::Storage::Streams::IBuffer> resultBuffer;
	if (FAILED(readOperation->GetResults(&resultBuffer)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get results");
		return nullptr;
	}

	ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
	if (FAILED(resultBuffer.As(&bufferByteAccess)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get IBufferByteAccess");
		return nullptr;
	}

	BYTE* data = nullptr;
	if (FAILED(bufferByteAccess->Buffer(&data)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get buffer data");
		return nullptr;
	}

	UINT32 dataLength;
	if (FAILED(resultBuffer->get_Length(&dataLength)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get buffer length");
		return nullptr;
	}
	LogToFile(L"StreamToHBITMAP: Got %d bytes of image data", dataLength);

	ComPtr<IWICImagingFactory> wicFactory;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory))))
	{
		LogToFile(L"StreamToHBITMAP: Failed to create WIC factory");
		return nullptr;
	}

	ComPtr<IWICStream> wicStream;
	if (FAILED(wicFactory->CreateStream(&wicStream)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to create WIC stream");
		return nullptr;
	}

	if (FAILED(wicStream->InitializeFromMemory(data, dataLength)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to initialize WIC stream from memory");
		return nullptr;
	}

	ComPtr<IWICBitmapDecoder> decoder;
	if (FAILED(wicFactory->CreateDecoderFromStream(wicStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &decoder)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to create decoder");
		return nullptr;
	}

	ComPtr<IWICBitmapFrameDecode> frame;
	if (FAILED(decoder->GetFrame(0, &frame)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get frame");
		return nullptr;
	}

	ComPtr<IWICFormatConverter> converter;
	if (FAILED(wicFactory->CreateFormatConverter(&converter)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to create format converter");
		return nullptr;
	}

	if (FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to initialize converter");
		return nullptr;
	}

	UINT width, height;
	if (FAILED(converter->GetSize(&width, &height)))
	{
		LogToFile(L"StreamToHBITMAP: Failed to get size");
		return nullptr;
	}
	LogToFile(L"StreamToHBITMAP: Image size=%dx%d", width, height);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -(int)height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;
	HDC hdc = GetDC(nullptr);
	HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
	ReleaseDC(nullptr, hdc);

	if (hBitmap && pBits)
	{
		UINT stride = width * 4;
		UINT bufferSize = stride * height;
		if (SUCCEEDED(converter->CopyPixels(nullptr, stride, bufferSize, (BYTE*)pBits)))
		{
			LogToFile(L"StreamToHBITMAP: Successfully created bitmap %p", hBitmap);
			return hBitmap;
		}
		LogToFile(L"StreamToHBITMAP: Failed to copy pixels");
		DeleteObject(hBitmap);
	}
	else
	{
		LogToFile(L"StreamToHBITMAP: Failed to create DIB section");
	}

	return nullptr;
}

void ConsoleBSDRStub::DrawButton(HDC hdc, LPDRAWITEMSTRUCT pDIS)
{
	bool isPressed = (pDIS->itemState & ODS_SELECTED);
	bool isFocused = (pDIS->itemState & ODS_FOCUS);
	bool isHighContrast = IsHighContrast();

	RECT rcButton = pDIS->rcItem;

	if (isHighContrast)
	{
		UINT uState = DFCS_BUTTONPUSH;
		if (isPressed)
			uState |= DFCS_PUSHED;
		DrawFrameControl(hdc, &rcButton, DFC_BUTTON, uState);
	}
	else
	{
		HBITMAP hBitmap = nullptr;
		bool isHover = (hHoverButton == pDIS->hwndItem);

		if (isPressed)
		{
			hBitmap = btnPressedBitmap;
		}
		else if (isFocused && isHover)
		{
			hBitmap = btnSelectedHoverBitmap;
		}
		else if (isFocused)
		{
			hBitmap = btnSelectedBitmap;
		}
		else if (isHover)
		{
			hBitmap = btnHoverBitmap;
		}
		else
		{
			hBitmap = btnNormalBitmap;
		}

		if (hBitmap)
		{
			BITMAP bm;
			GetObject(hBitmap, sizeof(bm), &bm);

			int width = rcButton.right - rcButton.left;
			int height = rcButton.bottom - rcButton.top;

			const int borderSize = MulDiv(4, GetDpiForWindow(hDlg), 96);

			int srcW = bm.bmWidth;
			int srcH = bm.bmHeight;
			int dstW = width;
			int dstH = height;

			int left = rcButton.left;
			int top = rcButton.top;

			if (bm.bmBitsPixel == 32)
			{
				BITMAPINFO bmi = {};
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = dstW;
				bmi.bmiHeader.biHeight = -dstH;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;

				void* pBits = nullptr;
				HBITMAP hOffscreenBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
				if (hOffscreenBmp)
				{
					HDC hdcOffscreen = CreateCompatibleDC(hdc);
					HBITMAP hOldOffscreen = (HBITMAP)SelectObject(hdcOffscreen, hOffscreenBmp);

					HWND hParent = GetParent(pDIS->hwndItem);
					HWND hGrandParent = GetParent(hParent);
					if (hGrandParent && bgBitmap)
					{
						POINT pt = { rcButton.left, rcButton.top };
						MapWindowPoints(pDIS->hwndItem, hGrandParent, &pt, 1);
						pt.x -= bgOffsetX;
						pt.y -= bgOffsetY;

						HDC hdcBg = CreateCompatibleDC(hdc);
						HBITMAP hOldBg = (HBITMAP)SelectObject(hdcBg, bgBitmap);
						BitBlt(hdcOffscreen, 0, 0, dstW, dstH, hdcBg, pt.x, pt.y, SRCCOPY);
						SelectObject(hdcBg, hOldBg);
						DeleteDC(hdcBg);
					}

					HDC hdcSrc = CreateCompatibleDC(hdc);
					HBITMAP hOldSrc = (HBITMAP)SelectObject(hdcSrc, hBitmap);

					BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

					AlphaBlend(hdcOffscreen, 0, 0, borderSize, borderSize, hdcSrc, 0, 0, borderSize, borderSize, bf);
					AlphaBlend(hdcOffscreen, dstW - borderSize, 0, borderSize, borderSize, hdcSrc, srcW - borderSize, 0, borderSize, borderSize, bf);
					AlphaBlend(hdcOffscreen, 0, dstH - borderSize, borderSize, borderSize, hdcSrc, 0, srcH - borderSize, borderSize, borderSize, bf);
					AlphaBlend(hdcOffscreen, dstW - borderSize, dstH - borderSize, borderSize, borderSize, hdcSrc, srcW - borderSize, srcH - borderSize, borderSize, borderSize, bf);

					AlphaBlend(hdcOffscreen, borderSize, 0, dstW - 2 * borderSize, borderSize, hdcSrc, borderSize, 0, srcW - 2 * borderSize, borderSize, bf);
					AlphaBlend(hdcOffscreen, borderSize, dstH - borderSize, dstW - 2 * borderSize, borderSize, hdcSrc, borderSize, srcH - borderSize, srcW - 2 * borderSize, borderSize, bf);
					AlphaBlend(hdcOffscreen, 0, borderSize, borderSize, dstH - 2 * borderSize, hdcSrc, 0, borderSize, borderSize, srcH - 2 * borderSize, bf);
					AlphaBlend(hdcOffscreen, dstW - borderSize, borderSize, borderSize, dstH - 2 * borderSize, hdcSrc, srcW - borderSize, borderSize, borderSize, srcH - 2 * borderSize, bf);

					AlphaBlend(hdcOffscreen, borderSize, borderSize, dstW - 2 * borderSize, dstH - 2 * borderSize, hdcSrc, borderSize, borderSize, srcW - 2 * borderSize, srcH - 2 * borderSize, bf);

					SelectObject(hdcSrc, hOldSrc);
					DeleteDC(hdcSrc);

					BitBlt(hdc, left, top, dstW, dstH, hdcOffscreen, 0, 0, SRCCOPY);

					SelectObject(hdcOffscreen, hOldOffscreen);
					DeleteDC(hdcOffscreen);
					DeleteObject(hOffscreenBmp);
				}
			}
			else
			{
				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
				StretchBlt(hdc, left, top, dstW, dstH, hdcMem, 0, 0, srcW, srcH, SRCCOPY);
				SelectObject(hdcMem, hOldBitmap);
				DeleteDC(hdcMem);
			}
		}
	}

	wchar_t buttonText[256];
	GetWindowTextW(pDIS->hwndItem, buttonText, _countof(buttonText));

	if (wcslen(buttonText) > 0)
	{
		if (!isHighContrast)
		{
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255, 255, 255));
		}
		else
		{
			SetBkMode(hdc, OPAQUE);
			SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
		}
		DrawTextW(hdc, buttonText, -1, &rcButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	if (isHighContrast && isFocused)
	{
		int cxEdge = 2 * GetSystemMetrics(SM_CXEDGE);
		int cxBorder = GetSystemMetrics(SM_CXBORDER) + cxEdge;
		int cyEdge = 2 * GetSystemMetrics(SM_CYEDGE);
		int cyBorder = GetSystemMetrics(SM_CYBORDER) + cyEdge;

		InflateRect(&rcButton, -cxBorder, -cyBorder);
		DrawFocusRect(hdc, &rcButton);
	}
}

LRESULT CALLBACK ConsoleBSDRStub::ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (hHoverButton != hWnd)
		{
			if (hHoverButton)
			{
				RedrawWindow(hHoverButton, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
			}
			hHoverButton = hWnd;
			RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
		}

		TRACKMOUSEEVENT tme = { sizeof(tme) };
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd;
		TrackMouseEvent(&tme);
		break;
	}
	case WM_MOUSELEAVE:
	{
		if (hHoverButton == hWnd)
		{
			hHoverButton = nullptr;
			RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		break;
	}
	case WM_NCDESTROY:
	{
		RemoveWindowSubclass(hWnd, ButtonSubclassProc, uIdSubclass);
		break;
	}
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ConsoleBSDRStub::AppListSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC:
	{
		if (!IsHighContrast())
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			SetStretchBltMode((HDC)wParam, HALFTONE);

			HWND hControl = (HWND)lParam;
			bool isBlockReason = false;
			for (auto& tile : appTiles)
			{
				if (tile.hBlockReason == hControl)
				{
					isBlockReason = true;
					break;
				}
			}

			if (isBlockReason)
			{
				SetTextColor((HDC)wParam, RGB(255, 204, 153));
			}
			else
			{
				SetTextColor((HDC)wParam, RGB(255, 255, 255));
			}

			return (INT_PTR)GetStockObject(NULL_BRUSH);
		}
		break;
	}
	case WM_ERASEBKGND:
	{
		if (!IsHighContrast())
		{
			return TRUE;
		}
		break;
	}
	case WM_NCDESTROY:
	{
		RemoveWindowSubclass(hWnd, AppListSubclassProc, uIdSubclass);
		break;
	}
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void ConsoleBSDRStub::CreateAppTileControls(IShutdownBlockingApp* blockingApp)
{
	if (!hDlg || !blockingApp)
	{
		return;
	}

	int dpi = GetDpiForWindow(hDlg);
	LogToFile(L"DPI: %d", dpi);

	AppTile tile = {};
	blockingApp->get_Id(&tile.appId);
	blockingApp->get_IsBlocking(&tile.isBlocking);
	tile.app = blockingApp;
	tile.hIconBitmap = nullptr;

	LogToFile(L"Creating tile for app %d, isBlocking=%d", tile.appId, tile.isBlocking);

	HSTRING caption, blockReason;
	std::wstring titleText;
	std::wstring blockReasonText;
	bool hasCustomReason = false;

	if (SUCCEEDED(blockingApp->get_Caption(&caption)))
	{
		const wchar_t* captionStr = WindowsGetStringRawBuffer(caption, nullptr);
		if (tile.isBlocking)
		{
			wchar_t waitingFor[256];
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_WAITINGFOR, waitingFor, _countof(waitingFor));
			titleText = std::wstring(waitingFor) + L" " + captionStr;
		}
		else
		{
			titleText = captionStr;
		}
		LogToFile(L"Title text: %s", titleText.c_str());
		WindowsDeleteString(caption);
	}

	if (SUCCEEDED(blockingApp->get_BlockReason(&blockReason)))
	{
		const wchar_t* reasonStr = WindowsGetStringRawBuffer(blockReason, nullptr);
		if (reasonStr && wcslen(reasonStr) > 0)
		{
			blockReasonText = reasonStr;
			hasCustomReason = true;
		}
		WindowsDeleteString(blockReason);
	}

	if (blockReasonText.empty() && tile.isBlocking)
	{
		UINT stringId = IDS_BSDR_BLOCKINGAPP_SHUTDOWN;
		if (logonUIState == LogonUIState_LoggingOff)
			stringId = IDS_BSDR_BLOCKINGAPP_LOGOFF;
		else if (logonUIState == LogonUIState_Restarting)
			stringId = IDS_BSDR_BLOCKINGAPP_RESTART;

		wchar_t defaultReason[256];
		LoadStringW(HINST_THISCOMPONENT, stringId, defaultReason, _countof(defaultReason));
		blockReasonText = defaultReason;
	}
	LogToFile(L"Block reason text: %s, showReason=%d", blockReasonText.c_str(), !blockReasonText.empty());

	int iconSize = MulDiv(32, dpi, 96);

	HWND hParent = hAppListContainer ? hAppListContainer : hDlg;

	tile.hIcon = CreateWindowExW(0, L"Static", nullptr, WS_CHILD | WS_VISIBLE | SS_BITMAP, 0, 0, iconSize, iconSize, hParent, nullptr, HINST_THISCOMPONENT, nullptr);
	tile.hTitle = CreateWindowExW(0, L"Static", titleText.c_str(), WS_CHILD | WS_VISIBLE, 0, 0, 100, 20, hParent, nullptr, HINST_THISCOMPONENT, nullptr);

	if (!blockReasonText.empty())
	{
		tile.hBlockReason = CreateWindowExW(0, L"Static", blockReasonText.c_str(), WS_CHILD | WS_VISIBLE | SS_EDITCONTROL, 0, 0, 100, 40, hParent, nullptr, HINST_THISCOMPONENT, nullptr);
	}
	else
	{
		tile.hBlockReason = nullptr;
	}

	LogToFile(L"Created controls: hIcon=%p, hTitle=%p, hBlockReason=%p", tile.hIcon, tile.hTitle, tile.hBlockReason);

	HFONT hFont = (HFONT)SendMessageW(hDlg, WM_GETFONT, 0, 0);
	if (tile.hTitle)
		SendMessageW(tile.hTitle, WM_SETFONT, (WPARAM)hFont, FALSE);
	if (tile.hBlockReason)
		SendMessageW(tile.hBlockReason, WM_SETFONT, (WPARAM)hFont, FALSE);

	ABI::Windows::Storage::Streams::IRandomAccessStream* iconStream = nullptr;
	if (SUCCEEDED(blockingApp->get_Icon(&iconStream)) && iconStream)
	{
		LogToFile(L"Got icon stream, converting to HBITMAP");
		tile.hIconBitmap = StreamToHBITMAP(iconStream);
		if (tile.hIcon && tile.hIconBitmap)
		{
			SendMessageW(tile.hIcon, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)tile.hIconBitmap);
			LogToFile(L"Icon bitmap set: %p", tile.hIconBitmap);
		}
		else
		{
			LogToFile(L"Failed to set icon: hIcon=%p, hIconBitmap=%p", tile.hIcon, tile.hIconBitmap);
		}
		iconStream->Release();
	}
	else
	{
		LogToFile(L"No icon stream available");
	}

	if (tile.isBlocking)
	{
		appTiles.insert(appTiles.begin(), tile);
		LogToFile(L"Inserted blocking app at beginning, total apps: %d", (int)appTiles.size());
	}
	else
	{
		appTiles.push_back(tile);
		LogToFile(L"Added non-blocking app at end, total apps: %d", (int)appTiles.size());
	}

	UpdateAppListLayout();
}

void ConsoleBSDRStub::RemoveAppTileControls(UINT appId)
{
	LogToFile(L"RemoveAppTileControls: Removing app %d on thread %d", appId, GetCurrentThreadId());

	for (auto it = appTiles.begin(); it != appTiles.end(); ++it)
	{
		if (it->appId == appId)
		{
			LogToFile(L"Found app %d to remove: hIcon=%p, hTitle=%p, hBlockReason=%p", 
				appId, it->hIcon, it->hTitle, it->hBlockReason);

			if (it->hIcon)
			{
				BOOL result = DestroyWindow(it->hIcon);
				LogToFile(L"DestroyWindow(hIcon) returned %d, LastError=%d", result, GetLastError());
			}
			if (it->hTitle)
			{
				BOOL result = DestroyWindow(it->hTitle);
				LogToFile(L"DestroyWindow(hTitle) returned %d, LastError=%d", result, GetLastError());
			}
			if (it->hBlockReason)
			{
				BOOL result = DestroyWindow(it->hBlockReason);
				LogToFile(L"DestroyWindow(hBlockReason) returned %d, LastError=%d", result, GetLastError());
			}
			if (it->hIconBitmap)
			{
				DeleteObject(it->hIconBitmap);
			}

			appTiles.erase(it);
			LogToFile(L"Erased app from vector, new count: %d", (int)appTiles.size());

			UpdateAppListLayout();
			return;
		}
	}
	LogToFile(L"App %d not found in appTiles", appId);
}

void ConsoleBSDRStub::UpdateAppListLayout()
{
	if (!hDlg)
	{
		LogToFile(L"UpdateAppListLayout: hDlg is null");
		return;
	}

	int dpi = GetDpiForWindow(hDlg);
	LogToFile(L"UpdateAppListLayout: Updating layout for %d apps, scrollPos=%d, DPI=%d", (int)appTiles.size(), scrollPos, dpi);

	const int iconSize = MulDiv(32, dpi, 96);
	const int leftMargin = 0;
	const int topMargin = MulDiv(8, dpi, 96);
	const int itemHeight = MulDiv(73, dpi, 96);
	const int itemHeightNoReason = MulDiv(54, dpi, 96);
	const int iconTextGap = MulDiv(8, dpi, 96);
	const int textHeight = MulDiv(20, dpi, 96);
	const int maxWidth = MulDiv(700, dpi, 96);

	int visibleHeight = MulDiv(300, dpi, 96);
	if (hAppListContainer)
	{
		RECT rcContainer;
		GetClientRect(hAppListContainer, &rcContainer);
		visibleHeight = rcContainer.bottom - rcContainer.top;
		LogToFile(L"Using actual container height: %d", visibleHeight);
	}

	int totalContentHeight = 0;
	for (auto& tile : appTiles)
	{
		totalContentHeight += (tile.hBlockReason != nullptr) ? itemHeight : itemHeightNoReason;
	}

	int maxScroll = totalContentHeight - visibleHeight;
	if (maxScroll < 0) maxScroll = 0;

	if (scrollPos > maxScroll)
	{
		scrollPos = maxScroll;
	}

	if (!hScrollBar)
	{
		hScrollBar = GetDlgItem(hDlg, IDC_BSDR_SCROLLBAR);
	}

	if (hScrollBar)
	{
		if (maxScroll > 0)
		{
			ShowWindow(hScrollBar, SW_SHOW);

			SCROLLINFO si = {};
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			si.nMin = 0;
			si.nMax = totalContentHeight;
			si.nPage = visibleHeight;
			si.nPos = scrollPos;
			SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

			LogToFile(L"Scrollbar shown: totalHeight=%d, visibleHeight=%d, maxScroll=%d", 
				totalContentHeight, visibleHeight, maxScroll);
		}
		else
		{
			ShowWindow(hScrollBar, SW_HIDE);
			scrollPos = 0;
			LogToFile(L"Scrollbar hidden: no scrolling needed");
		}
	}

	int yPos = topMargin - scrollPos;

	int index = 0;
	for (auto& tile : appTiles)
	{
		bool hasBlockReason = (tile.hBlockReason != nullptr);
		int titleYOffset = 0;
		int height = hasBlockReason ? itemHeight : itemHeightNoReason;

		if (!hasBlockReason)
		{
			titleYOffset = 10;
		}

		LogToFile(L"App %d (id=%d): yPos=%d, height=%d", index, tile.appId, yPos, height);

		if (tile.hIcon)
		{
			SetWindowPos(tile.hIcon, nullptr, leftMargin, yPos + MulDiv(4, dpi, 96), iconSize, iconSize, SWP_NOZORDER | SWP_SHOWWINDOW);
		}

		if (tile.hTitle)
		{
			SetWindowPos(tile.hTitle, nullptr, leftMargin + iconSize + iconTextGap, yPos + titleYOffset, maxWidth, textHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
		}

		if (tile.hBlockReason)
		{
			SetWindowPos(tile.hBlockReason, nullptr, leftMargin + iconSize + iconTextGap, yPos + MulDiv(24, dpi, 96), maxWidth, textHeight * 2, SWP_NOZORDER | SWP_SHOWWINDOW);
		}

		yPos += height;
		index++;
	}

	if (hTitleText)
	{
		wchar_t titleFormat[256];
		wchar_t titleText[256];
		if (appTiles.size() == 1)
		{
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_BLOCKINGAPPCOUNT_SINGLE, titleFormat, _countof(titleFormat));
		}
		else
		{
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_BLOCKINGAPPCOUNT_MULTI, titleFormat, _countof(titleFormat));
		}
		swprintf_s(titleText, _countof(titleText), titleFormat, (int)appTiles.size());
		SetWindowTextW(hTitleText, titleText);
		LogToFile(L"Updated title: %s", titleText);
	}
}

INT_PTR CALLBACK ConsoleBSDRStub::DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		LogToFile(L"Dialog initialized, hDlg parameter=%p, static hDlg=%p", hDlg, ConsoleBSDRStub::hDlg);
		ConsoleBSDRStub::hDlg = hDlg;

		SetWindowLong(hDlg, GWL_STYLE, GetWindowLong(hDlg, GWL_STYLE) & ~WS_CAPTION);
		hTitleText = GetDlgItem(hDlg, IDC_BSDR_TITLE);
		hWarningText = GetDlgItem(hDlg, IDC_BSDR_WARNING);
		hForceButton = GetDlgItem(hDlg, IDC_BSDR_FORCE_BTN);
		hCancelButton = GetDlgItem(hDlg, IDCANCEL);
		hDescText = GetDlgItem(hDlg, IDC_BSDR_DESC);
		hYesButton = GetDlgItem(hDlg, IDYES);
		hNoButton = GetDlgItem(hDlg, IDNO);
		ShowWindow(hWarningText, SW_HIDE);
		ShowWindow(hYesButton, SW_HIDE);
		ShowWindow(hNoButton, SW_HIDE);

		hAppListContainer = GetDlgItem(hDlg, IDC_BSDR_APPLIST);
		LogToFile(L"Got app list container: %p", hAppListContainer);

		if (hAppListContainer)
		{
			SetWindowSubclass(hAppListContainer, AppListSubclassProc, 0, 0);

			int dpi = GetDpiForWindow(hDlg);

			int itemHeight = MulDiv(73, dpi, 96);
			int itemHeightNoReason = MulDiv(54, dpi, 96);
			int topMargin = MulDiv(8, dpi, 96);

			int totalItemsHeight = topMargin;
			for (auto& app : pendingApps)
			{
				BOOLEAN isBlocking = FALSE;
				if (SUCCEEDED(app->get_IsBlocking(&isBlocking)) && isBlocking)
				{
					totalItemsHeight += itemHeight;
				}
				else
				{
					totalItemsHeight += itemHeightNoReason;
				}
			}

			int screenHeight = GetSystemMetrics(SM_CYSCREEN);
			int dialogTopMargin = MulDiv(119, dpi, 96);
			int dialogBottomReserved = MulDiv(119, dpi, 96);
			int minHeight = MulDiv(150, dpi, 96);
			int maxHeight = screenHeight - dialogTopMargin - dialogBottomReserved - MulDiv(100, dpi, 96);

			if (maxHeight < minHeight)
				maxHeight = minHeight;

			int newHeight = totalItemsHeight;
			if (newHeight < minHeight)
				newHeight = minHeight;
			if (newHeight > maxHeight)
				newHeight = maxHeight;

			RECT rcAppList;
			GetWindowRect(hAppListContainer, &rcAppList);
			MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcAppList, 2);
			int currentWidth = rcAppList.right - rcAppList.left;
			int appListTop = rcAppList.top;

			SetWindowPos(hAppListContainer, nullptr, 0, 0, currentWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
			LogToFile(L"Resized app list container: items=%d, totalHeight=%d, newHeight=%d (screen=%d)", 
				(int)pendingApps.size(), totalItemsHeight, newHeight, screenHeight);

			int bottomSeparatorNewY = appListTop + newHeight;

			HWND hBottomSeparator = GetDlgItem(hDlg, IDC_BSDR_SEPARATOR_BOTTOM);
			HWND hScrollBarCtrl = GetDlgItem(hDlg, IDC_BSDR_SCROLLBAR);

			if (hBottomSeparator)
			{
				RECT rcSep;
				GetWindowRect(hBottomSeparator, &rcSep);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcSep, 2);
				int sepHeight = rcSep.bottom - rcSep.top;
				SetWindowPos(hBottomSeparator, nullptr, rcSep.left, bottomSeparatorNewY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				bottomSeparatorNewY += sepHeight;
			}

			if (hScrollBarCtrl)
			{
				RECT rcScroll;
				GetWindowRect(hScrollBarCtrl, &rcScroll);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcScroll, 2);
				int scrollWidth = rcScroll.right - rcScroll.left;
				SetWindowPos(hScrollBarCtrl, nullptr, rcScroll.left, appListTop, scrollWidth, newHeight, SWP_NOZORDER);
				LogToFile(L"Resized scrollbar: x=%d, y=%d, width=%d, height=%d", rcScroll.left, appListTop, scrollWidth, newHeight);
			}

			int controlsStartY = bottomSeparatorNewY + MulDiv(24, dpi, 96);

			if (hDescText)
			{
				RECT rcDesc;
				GetWindowRect(hDescText, &rcDesc);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcDesc, 2);
				SetWindowPos(hDescText, nullptr, rcDesc.left, controlsStartY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

			if (hWarningText)
			{
				RECT rcWarn;
				GetWindowRect(hWarningText, &rcWarn);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcWarn, 2);
				SetWindowPos(hWarningText, nullptr, rcWarn.left, controlsStartY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

			int buttonsY = controlsStartY + MulDiv(60, dpi, 96);

			if (hForceButton)
			{
				RECT rcBtn;
				GetWindowRect(hForceButton, &rcBtn);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcBtn, 2);
				SetWindowPos(hForceButton, nullptr, rcBtn.left, buttonsY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

			if (hCancelButton)
			{
				RECT rcBtn;
				GetWindowRect(hCancelButton, &rcBtn);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcBtn, 2);
				SetWindowPos(hCancelButton, nullptr, rcBtn.left, buttonsY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

			if (hYesButton)
			{
				RECT rcBtn;
				GetWindowRect(hYesButton, &rcBtn);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcBtn, 2);
				SetWindowPos(hYesButton, nullptr, rcBtn.left, buttonsY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

			if (hNoButton)
			{
				RECT rcBtn;
				GetWindowRect(hNoButton, &rcBtn);
				MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcBtn, 2);
				SetWindowPos(hNoButton, nullptr, rcBtn.left, buttonsY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

			RECT rcDialog;
			GetWindowRect(hDlg, &rcDialog);
			int dialogWidth = rcDialog.right - rcDialog.left;
			int newDialogHeight = buttonsY + MulDiv(80, dpi, 96);

			SetWindowPos(hDlg, nullptr, 0, 0, dialogWidth, newDialogHeight, SWP_NOMOVE | SWP_NOZORDER);
			LogToFile(L"Resized dialog to height: %d", newDialogHeight);
		}

		SetWindowSubclass(hForceButton, ButtonSubclassProc, 0, 0);
		SetWindowSubclass(hCancelButton, ButtonSubclassProc, 0, 0);
		SetWindowSubclass(hYesButton, ButtonSubclassProc, 0, 0);
		SetWindowSubclass(hNoButton, ButtonSubclassProc, 0, 0);

		HFONT hDialogFont = (HFONT)SendMessageW(hDlg, WM_GETFONT, 0, 0);
		LOGFONTW lf = {};
		if (hDialogFont && GetObjectW(hDialogFont, sizeof(lf), &lf))
		{
			int originalHeight = lf.lfHeight;

			lf.lfHeight = MulDiv(originalHeight, 160, 100);
			hTitleFont = CreateFontIndirectW(&lf);
			if (hTitleFont && hTitleText)
			{
				SendMessageW(hTitleText, WM_SETFONT, (WPARAM)hTitleFont, FALSE);
			}

			lf.lfHeight = MulDiv(originalHeight, 120, 100);
			hDescFont = CreateFontIndirectW(&lf);
			if (hDescFont)
			{
				if (hWarningText)
					SendMessageW(hWarningText, WM_SETFONT, (WPARAM)hDescFont, FALSE);
				if (hDescText)
					SendMessageW(hDescText, WM_SETFONT, (WPARAM)hDescFont, FALSE);
			}
		}

		LogToFile(L"Processing %d pending apps", (int)pendingApps.size());
		for (auto& app : pendingApps)
		{
			CreateAppTileControls(app.Get());
		}
		pendingApps.clear();

		wchar_t desc[256], warning[256], btnText[256];
		switch (logonUIState)
		{
		case LogonUIState_LoggingOff:
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_DESC_LOGOFF, desc, _countof(desc));
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_WARNING_LOGOFF, warning, _countof(warning));
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_BTN_LOGOFF, btnText, _countof(btnText));
			break;
		case LogonUIState_Restarting:
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_DESC_RESTART, desc, _countof(desc));
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_WARNING_RESTART, warning, _countof(warning));
			LoadStringW(HINST_THISCOMPONENT, IDS_BSDR_BTN_RESTART, btnText, _countof(btnText));
			break;
		}
		if (logonUIState != LogonUIState_ShuttingDown)
		{
			SetWindowTextW(hDescText, desc);
			SetWindowTextW(hWarningText, warning);
			SetWindowTextW(hForceButton, btnText);
		}

		CenterWindow(hDlg);
		return TRUE;
	}
	case WM_CTLCOLORDLG:
	{
		if (!IsHighContrast())
		{
			return (INT_PTR)GetStockObject(NULL_BRUSH);
		}
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		if (!IsHighContrast())
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			SetStretchBltMode((HDC)wParam, HALFTONE);

			HWND hControl = (HWND)lParam;
			bool isBlockReason = false;
			for (auto& tile : appTiles)
			{
				if (tile.hBlockReason == hControl)
				{
					isBlockReason = true;
					break;
				}
			}

			if (isBlockReason)
			{
				SetTextColor((HDC)wParam, RGB(255, 204, 153));
			}
			else
			{
				SetTextColor((HDC)wParam, RGB(255, 255, 255));
			}

			return (INT_PTR)GetStockObject(NULL_BRUSH);
		}
		break;
	}
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
		if (pDIS->CtlID == IDC_BSDR_SEPARATOR_TOP || pDIS->CtlID == IDC_BSDR_SEPARATOR_BOTTOM)
		{
			if (separatorBitmap && !IsHighContrast())
			{
				BITMAP bm;
				GetObject(separatorBitmap, sizeof(bm), &bm);

				int width = pDIS->rcItem.right - pDIS->rcItem.left;
				int height = pDIS->rcItem.bottom - pDIS->rcItem.top;

				BITMAPINFO bmi = {};
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = width;
				bmi.bmiHeader.biHeight = -height;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;

				void* pBits = nullptr;
				HBITMAP hOffscreenBmp = CreateDIBSection(pDIS->hDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
				if (hOffscreenBmp)
				{
					HDC hdcOffscreen = CreateCompatibleDC(pDIS->hDC);
					HBITMAP hOldOffscreen = (HBITMAP)SelectObject(hdcOffscreen, hOffscreenBmp);

					HWND hParent = GetParent(pDIS->hwndItem);
					HWND hGrandParent = GetParent(hParent);
					if (hGrandParent && bgBitmap)
					{
						POINT pt = { pDIS->rcItem.left, pDIS->rcItem.top };
						MapWindowPoints(pDIS->hwndItem, hGrandParent, &pt, 1);
						pt.x -= bgOffsetX;
						pt.y -= bgOffsetY;

						BITMAP bmInfo;
						GetObject(bgBitmap, sizeof(bmInfo), &bmInfo);

						if (pt.x >= 0 && pt.y >= 0 && pt.x + width <= bmInfo.bmWidth && pt.y + height <= bmInfo.bmHeight)
						{
							HDC hdcBg = CreateCompatibleDC(pDIS->hDC);
							HBITMAP hOldBg = (HBITMAP)SelectObject(hdcBg, bgBitmap);
							BitBlt(hdcOffscreen, 0, 0, width, height, hdcBg, pt.x, pt.y, SRCCOPY);
							SelectObject(hdcBg, hOldBg);
							DeleteDC(hdcBg);
						}
					}

					HDC hdcSep = CreateCompatibleDC(pDIS->hDC);
					HBITMAP hOldSep = (HBITMAP)SelectObject(hdcSep, separatorBitmap);

					BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

					const int srcLeftSkip = 4;
					const int srcLeftGradient = 19;
					const int srcCenter = 8;
					const int srcRightGradient = 19;

					const int dstLeftGradient = 6;
					const int dstRightGradient = 6;
					const int dstCenter = width - dstLeftGradient - dstRightGradient;

					AlphaBlend(hdcOffscreen, 0, 0, dstLeftGradient, 1, 
						hdcSep, srcLeftSkip, 1, srcLeftGradient, 1, bf);

					AlphaBlend(hdcOffscreen, dstLeftGradient, 0, dstCenter, 1, 
						hdcSep, srcLeftSkip + srcLeftGradient, 1, srcCenter, 1, bf);

					AlphaBlend(hdcOffscreen, dstLeftGradient + dstCenter, 0, dstRightGradient, 1, 
						hdcSep, srcLeftSkip + srcLeftGradient + srcCenter, 1, srcRightGradient, 1, bf);

					SelectObject(hdcSep, hOldSep);
					DeleteDC(hdcSep);

					BitBlt(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, width, height, hdcOffscreen, 0, 0, SRCCOPY);

					SelectObject(hdcOffscreen, hOldOffscreen);
					DeleteDC(hdcOffscreen);
					DeleteObject(hOffscreenBmp);
				}
				return TRUE;
			}
			else
			{
				HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));
				FillRect(pDIS->hDC, &pDIS->rcItem, hBrush);
				DeleteObject(hBrush);
				return TRUE;
			}
		}
		else if (pDIS->CtlType == ODT_BUTTON)
		{
			DrawButton(pDIS->hDC, pDIS);
			return TRUE;
		}
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
		{
			LogToFile(L"Cancel button clicked - resolving with Cancel");
			wasClicked = TRUE;
			_Resolved.InvokeAll(nullptr, BlockedShutdownResolution_Cancel);
			EndDialog(hDlg, 0);
			DestroyWindow(hBgWnd);
			return TRUE;
		}
		case IDC_BSDR_FORCE_BTN:
		{
			LogToFile(L"Force button clicked");
			ShowWindow(hWarningText, SW_SHOW);
			ShowWindow(hYesButton, SW_SHOW);
			ShowWindow(hNoButton, SW_SHOW);
			ShowWindow(hDescText, SW_HIDE);
			ShowWindow(hCancelButton, SW_HIDE);
			ShowWindow(hForceButton, SW_HIDE);
			return TRUE;
		}
		case IDYES:
		{
			LogToFile(L"Yes button clicked - resolving with Force");
			wasClicked = TRUE;
			_Resolved.InvokeAll(nullptr, BlockedShutdownResolution_Force);
			EndDialog(hDlg, 0);
			DestroyWindow(hBgWnd);
			return TRUE;
		}
		case IDNO:
		{
			LogToFile(L"No button clicked");
			ShowWindow(hWarningText, SW_HIDE);
			ShowWindow(hYesButton, SW_HIDE);
			ShowWindow(hNoButton, SW_HIDE);
			ShowWindow(hDescText, SW_SHOW);
			ShowWindow(hCancelButton, SW_SHOW);
			ShowWindow(hForceButton, SW_SHOW);
			return TRUE;
		}
		}
		break;
	}
	case WM_ADD_APP:
	{
		LogToFile(L"WM_ADD_APP received on dialog thread=%d, queue size=%d", GetCurrentThreadId(), (int)addQueue.size());
		if (!addQueue.empty())
		{
			for (auto& app : addQueue)
			{
				CreateAppTileControls(app.Get());
			}
			addQueue.clear();
		}
		return TRUE;
	}
	case WM_REMOVE_APP:
	{
		UINT appId = (UINT)wParam;
		LogToFile(L"WM_REMOVE_APP received for AppId=%d on dialog thread=%d", appId, GetCurrentThreadId());
		RemoveAppTileControls(appId);
		return TRUE;
	}
	case WM_MOUSEWHEEL:
	{
		int dpi = GetDpiForWindow(hDlg);
		int itemHeight = MulDiv(60, dpi, 96);
		int visibleHeight = MulDiv(300, dpi, 96);

		if (hAppListContainer)
		{
			RECT rcContainer;
			GetClientRect(hAppListContainer, &rcContainer);
			visibleHeight = rcContainer.bottom - rcContainer.top;
		}

		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		scrollPos -= delta / 4;
		if (scrollPos < 0) scrollPos = 0;

		int maxScroll = (int)appTiles.size() * itemHeight - visibleHeight;
		if (maxScroll < 0) maxScroll = 0;
		if (scrollPos > maxScroll) scrollPos = maxScroll;

		UpdateAppListLayout();
		return TRUE;
	}
	case WM_VSCROLL:
	{
		int dpi = GetDpiForWindow(hDlg);
		int itemHeight = MulDiv(60, dpi, 96);
		int visibleHeight = MulDiv(300, dpi, 96);

		if (hAppListContainer)
		{
			RECT rcContainer;
			GetClientRect(hAppListContainer, &rcContainer);
			visibleHeight = rcContainer.bottom - rcContainer.top;
		}

		int oldPos = scrollPos;
		switch (LOWORD(wParam))
		{
		case SB_LINEUP: scrollPos -= MulDiv(20, dpi, 96); break;
		case SB_LINEDOWN: scrollPos += MulDiv(20, dpi, 96); break;
		case SB_PAGEUP: scrollPos -= MulDiv(100, dpi, 96); break;
		case SB_PAGEDOWN: scrollPos += MulDiv(100, dpi, 96); break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION: scrollPos = HIWORD(wParam); break;
		}

		if (scrollPos < 0) scrollPos = 0;
		int maxScroll = (int)appTiles.size() * itemHeight - visibleHeight;
		if (maxScroll < 0) maxScroll = 0;
		if (scrollPos > maxScroll) scrollPos = maxScroll;

		if (oldPos != scrollPos)
		{
			UpdateAppListLayout();
		}
		return TRUE;
	}
	}
	return FALSE;
}

LRESULT CALLBACK ConsoleBSDRStub::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			LogToFile(L"Window created");
			hDlg = CreateDialogParamW(HINST_THISCOMPONENT, MAKEINTRESOURCEW(IDD_BSDR_DLG), hWnd, DlgProc, lParam);
			if (hDlg)
			{
				ShowWindow(hDlg, SW_SHOW);
			}
			else
			{
				LogToFile(L"Failed to create dialog: %d", GetLastError());
			}
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			if (bgBitmap)
			{
				HDC memDC = CreateCompatibleDC(hdc);
				HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bgBitmap);
				BitBlt(hdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), memDC, 0, 0, SRCCOPY);
				SelectObject(memDC, oldBitmap);
				DeleteDC(memDC);
			}
			if (g_testiconbmp)
			{
				BITMAP bm;
				GetObject(g_testiconbmp, sizeof(bm), &bm);

				HDC memDC = CreateCompatibleDC(hdc);
				HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, g_testiconbmp);

				BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
				AlphaBlend(hdc, 100, 100, bm.bmWidth, bm.bmHeight, memDC, 0, 0, bm.bmWidth, bm.bmHeight, bf);

				SelectObject(memDC, oldBitmap);
				DeleteDC(memDC);
				LogToFile(L"Test icon drawn at (100,100), size: %dx%d", bm.bmWidth, bm.bmHeight);
			}
			EndPaint(hWnd, &ps);
		}
		break;
		//case WM_LBUTTONDOWN:
		//	LogToFile(L"Left button down - resolving with Force");
		//	_Resolved.InvokeAll(nullptr, BlockedShutdownResolution_Force);
		//	DestroyWindow(hWnd);
		//	return 0;
		//case WM_RBUTTONDOWN:
		//{
		//	LogToFile(L"Right button down - resolving with Cancel");
		//	// for some reason this results in lock screen
		//	_Resolved.InvokeAll(nullptr, BlockedShutdownResolution_Cancel);
		//	DestroyWindow(hWnd);
		//	return 0;
		//}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

DWORD WINAPI ConsoleBSDRStub::ThreadProc(LPVOID lpParameter)
{
	LogToFile(L"ConsoleBSDRStub::ThreadProc started");

	HDESK hDesktop = OpenInputDesktop(0, FALSE, DESKTOP_CREATEWINDOW | DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS);
	if (hDesktop)
	{
		if (!SetThreadDesktop(hDesktop))
		{
			LogToFile(L"Failed to set thread desktop: %d", GetLastError());
			CloseDesktop(hDesktop);
			return GetLastError();
		}
		CloseDesktop(hDesktop);
	}
	else
	{
		LogToFile(L"Failed to open input desktop: %d", GetLastError());
		return GetLastError();
	}

	WNDCLASSEXW wndClass = {};

	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.style = CS_GLOBALCLASS;
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOWFRAME;
	wndClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = L"BlockedShutdownResolver";

	if (RegisterClassExW(&wndClass) == 0)
	{
		LogToFile(L"Failed to register window class: %d", GetLastError());
		return GetLastError();
	}

	int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	LogToFile(L"Virtual screen: x=%d, y=%d, cx=%d, cy=%d", x, y, cx, cy);

	bgOffsetX = x;
	bgOffsetY = y;

	if (!IsHighContrast())
	{
		HDC hDC = GetDC(nullptr);
		HDC hMemDC = CreateCompatibleDC(hDC);
		bgBitmap = CreateCompatibleBitmap(hDC, cx, cy);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hMemDC, bgBitmap);

		LogToFile(L"Created bgBitmap: %p, size: %dx%d", bgBitmap, cx, cy);

		RECT rect = { 0, 0, cx, cy };
		HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		FillRect(hMemDC, &rect, hBrush);

		AlphaBlend(hMemDC, 0, 0, cx, cy, hDC, x, y, cx, cy, { AC_SRC_OVER, 0, 31, 0 });

		SelectObject(hMemDC, oldBitmap);
		DeleteDC(hMemDC);
		ReleaseDC(nullptr, hDC);

		separatorBitmap = LoadAlphaBitmap(IDB_BSDR_SEPARATOR);
		btnNormalBitmap = LoadAlphaBitmap(IDB_BSDR_BTN_NORMAL);
		btnHoverBitmap = LoadAlphaBitmap(IDB_BSDR_BTN_HOVER);
		btnPressedBitmap = LoadAlphaBitmap(IDB_BSDR_BTN_PRESSED);
		btnSelectedBitmap = LoadAlphaBitmap(IDB_BSDR_BTN_SELECTED);
		btnSelectedHoverBitmap = LoadAlphaBitmap(IDB_BSDR_BTN_SELECTED_HOVER);
	}

	hBgWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_COMPOSITED, wndClass.lpszClassName, NULL, WS_POPUP | WS_VISIBLE, x, y, cx, cy, NULL, NULL, NULL, NULL);

	if (!hBgWnd)
	{
		int gle = GetLastError();
		LogToFile(L"Failed to create window: %d", gle);
		return gle;
	}

	//wchar_t desktopName[256], userName[UNLEN + 1], procName[MAX_PATH];
	//DWORD userNameSize = _countof(userName);
	//GetUserNameW(userName, &userNameSize);
	//GetThreadDesktop(GetCurrentThreadId());
	//GetUserObjectInformationW(GetThreadDesktop(GetCurrentThreadId()), UOI_NAME, desktopName, sizeof(desktopName), nullptr);
	//GetModuleFileNameW(nullptr, procName, _countof(procName));
	//LogToFile(L"Current user: %s, Current desktop: %s, Current process: %s", userName, desktopName, procName);

	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0) > 0)
	{
		if (!IsDialogMessageW(hDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return 0;
}

HRESULT ConsoleBSDRStub::Start(IUserSettingManager* settingsManager, ILogonUIStateInfo* stateInfo)
{
	LogToFile(L"===============================");
	stateInfo->get_CurrentLogonUIState(&logonUIState);
	LogToFile(L"ConsoleBSDRStub::Start called, Current LogonUIState: %d", logonUIState);
	hThread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
	return S_OK;
}

HRESULT ConsoleBSDRStub::get_ScaleFactor(UINT* value)
{
	*value = 100;
	return S_OK;
}

HRESULT ConsoleBSDRStub::get_WasClicked(BOOLEAN* value)
{
	LogToFile(L"ConsoleBSDRStub::get_WasClicked called, returning %s", wasClicked ? L"true" : L"false");
	*value = wasClicked;
	return S_OK;
}

HRESULT ConsoleBSDRStub::AddApplication(IShutdownBlockingApp* blockingApp)
{
	LogToFile(L"=== AddApplication called, hDlg=%p, Current thread=%d ===", hDlg, GetCurrentThreadId());
	std::wstring logMessage = L"ConsoleBSDRStub::AddApplication, AppId=";
	UINT appId = 0;
	if (SUCCEEDED(blockingApp->get_Id(&appId))) {
		logMessage += std::to_wstring(appId);
	}
	BOOLEAN isBlocking = FALSE;
	if (SUCCEEDED(blockingApp->get_IsBlocking(&isBlocking))) {
		logMessage += L", IsBlocking=" + std::wstring(isBlocking ? L"true" : L"false");
	}
	HSTRING caption, blockReason;
	if (SUCCEEDED(blockingApp->get_Caption(&caption))) {
		logMessage += L", Caption=" + std::wstring(WindowsGetStringRawBuffer(caption, NULL));
		WindowsDeleteString(caption);
	}
	if (SUCCEEDED(blockingApp->get_BlockReason(&blockReason))) {
		logMessage += L", BlockReason=" + std::wstring(WindowsGetStringRawBuffer(blockReason, NULL));
		WindowsDeleteString(blockReason);
	}
	LogToFile(logMessage.c_str());

	if (hDlg && IsWindow(hDlg))
	{
		LogToFile(L"Dialog exists, posting WM_ADD_APP message");
		addQueue.push_back(blockingApp);
		PostMessageW(hDlg, WM_ADD_APP, 0, 0);
	}
	else
	{
		LogToFile(L"Dialog not yet created, adding to pending list");
		pendingApps.push_back(blockingApp);
	}

	return S_OK;
}

HRESULT ConsoleBSDRStub::RemoveApplication(UINT appid)
{
	LogToFile(L"=== RemoveApplication called, AppId=%d, Current thread=%d ===", appid, GetCurrentThreadId());

	if (hDlg && IsWindow(hDlg))
	{
		LogToFile(L"Posting WM_REMOVE_APP message to dialog thread");
		PostMessageW(hDlg, WM_REMOVE_APP, (WPARAM)appid, 0);
	}
	else
	{
		LogToFile(L"WARNING: hDlg is null or invalid, cannot remove app");
	}

	return S_OK;
}

HRESULT ConsoleBSDRStub::add_Resolved(
	ITypedEventHandler<IBlockedShutdownResolverUX*, BlockedShutdownResolution>* handler, EventRegistrationToken* token)
{
	token->value = 0;

	return _Resolved.Add(handler, token);
}

HRESULT ConsoleBSDRStub::remove_Resolved(EventRegistrationToken token)
{
	return _Resolved.Remove(token);
}

HRESULT ConsoleBSDRStub::Hide()
{
	LogToFile(L"ConsoleBSDRStub::Hide called, hiding dialog");
	if (hDlg)
	{
		ShowWindow(hDlg, SW_HIDE);
	}
	return S_OK;
}

HRESULT ConsoleBSDRStub::Stop()
{
	LogToFile(L"ConsoleBSDRStub::Stop called, cleaning up resources");

	for (auto& tile : appTiles)
	{
		if (tile.hIcon) DestroyWindow(tile.hIcon);
		if (tile.hTitle) DestroyWindow(tile.hTitle);
		if (tile.hBlockReason) DestroyWindow(tile.hBlockReason);
		if (tile.hIconBitmap) DeleteObject(tile.hIconBitmap);
	}
	appTiles.clear();

	hAppListContainer = nullptr;

	if (hDlg)
	{
		LogToFile(L"Destroying dialog window");
		DestroyWindow(hDlg);
		hDlg = nullptr;
	}

	if (hBgWnd)
	{
		LogToFile(L"Destroying background window");
		DestroyWindow(hBgWnd);
		hBgWnd = nullptr;
	}

	if (bgBitmap)
	{
		DeleteObject(bgBitmap);
		bgBitmap = nullptr;
	}
	if (separatorBitmap)
	{
		DeleteObject(separatorBitmap);
		separatorBitmap = nullptr;
	}
	if (btnNormalBitmap)
	{
		DeleteObject(btnNormalBitmap);
		btnNormalBitmap = nullptr;
	}
	if (btnHoverBitmap)
	{
		DeleteObject(btnHoverBitmap);
		btnHoverBitmap = nullptr;
	}
	if (btnPressedBitmap)
	{
		DeleteObject(btnPressedBitmap);
		btnPressedBitmap = nullptr;
	}
	if (btnSelectedBitmap)
	{
		DeleteObject(btnSelectedBitmap);
		btnSelectedBitmap = nullptr;
	}
	if (btnSelectedHoverBitmap)
	{
		DeleteObject(btnSelectedHoverBitmap);
		btnSelectedHoverBitmap = nullptr;
	}
	if (g_testiconbmp)
	{
		DeleteObject(g_testiconbmp);
		g_testiconbmp = nullptr;
	}
	if (hTitleFont)
	{
		DeleteObject(hTitleFont);
		hTitleFont = nullptr;
	}
	if (hDescFont)
	{
		DeleteObject(hDescFont);
		hDescFont = nullptr;
	}

	LogToFile(L"Stop completed");
	return S_OK;
}

ActivatableClass(ConsoleBSDRStub);
