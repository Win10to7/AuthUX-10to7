#include "pch.h"
#include "backgroundfetcher.h"
#include <Gdiplus.h>
#include <gdiplusheaders.h>
#include <gdiplusinit.h>

struct ImageData
{
    DWORD w;
    DWORD h;
    unsigned long long resIdConsumer;
    unsigned long long resIdServer;
    const wchar_t* OEMPath;
};

struct ratioItem
{
    float ratio;
    int numberOfImages;
    ImageData* RatioImageDatas;
};

ImageData imageDatas125[] = { {1280, 1024, 5031, 5044, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1280x1024.jpg"} };
ImageData imageDatas1333333[] =
{
    {1280, 960, 5032, 5045, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1280x960.jpg"},
    {1024, 768, 5033, 5046, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1024x768.jpg"},
    {1600, 1200, 5034, 5047, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1600x1200.jpg"}
};
ImageData imageDatas16[] =
{
    {1440, 900, 5035, 5048, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1440x900.jpg"},
    {1920, 1200, 5036, 5049, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1920x1200.jpg"}
};
ImageData imageData166666[] = { {1280, 768, 5037, 5050, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1280x768.jpg"} };
ImageData imageData177[] = { {1360, 768, 5038, 5051, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1360x768.jpg"} };
ImageData imageData08[] = { {1024, 1280, 5039, 5052, L"%windir%\\system32\\oobe\\info\\backgrounds\\background1024x1280.jpg"} };
ImageData imageData075[] = { {960, 1280, 5040, 5053, L"%windir%\\system32\\oobe\\info\\backgrounds\\background960x1280.jpg"} };
ImageData imageData0625[] = { {900, 1440, 5041, 5054, L"%windir%\\system32\\oobe\\info\\backgrounds\\background900x1440.jpg"} };
ImageData imageData06000[] = { {768, 1280, 5042, 5055, L"%windir%\\system32\\oobe\\info\\backgrounds\\background768x1280.jpg"} };
ImageData imageData0564[] = { {768, 1360, 5043, 5056, L"%windir%\\system32\\oobe\\info\\backgrounds\\background768x1360.jpg"} };

ratioItem ratioItems[10] =
	{
	{1.25, 1, imageDatas125 },
    {1.3333334,3,imageDatas1333333},
    {1.6, 2, imageDatas16},
    {1.6666666, 1,imageData166666},
    {1.7708334, 1, imageData177},
    {0.80000001, 1, imageData08},
    {0.75, 1, imageData075},
    {0.625, 1, imageData0625},
    {0.60000002, 1, imageData06000},
    {0.56470591, 1, imageData0564}
	};

static HRESULT SHRegGetDWORDW(HKEY hkey, const WCHAR* pszSubKey, const WCHAR* pszValue, void* pvData)
{
	DWORD pcbData[6];
	pcbData[0] = 4;
	LSTATUS ValueW_0 = SHRegGetValueW(hkey, pszSubKey, pszValue, 16, nullptr, pvData, pcbData);
	HRESULT result = static_cast<WORD>(ValueW_0) | 0x80070000;
	if (ValueW_0 <= 0)
		return ValueW_0;
	return result;
}

HRESULT CBackground::GetButtonSet(UINT* buttonSet)
{
	*buttonSet = IsOS(OS_ANYSERVER) ? 2 : 0;
	if ( (int)SHRegGetDWORDW(
				HKEY_LOCAL_MACHINE,
				L"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI",
				L"ButtonSet",
				buttonSet) >= 0
	  && *buttonSet >= 3 )
	{
		*buttonSet = 0;
	}
	return S_OK;
}

bool OEMBackgroundFileExists(ImageData* a1)
{
	WCHAR Dst[MAX_PATH + 4];

	bool bExists = false;
	if (ExpandEnvironmentStringsW(a1->OEMPath, Dst, MAX_PATH) <= MAX_PATH)
	{
		bExists = PathFileExistsW(Dst);
	}
	return bExists;
}

HBITMAP ScaleBitmapToRes(HBITMAP oldBitmap);
static HBITMAP GetHBITMAPFromImageFile(WCHAR* pFilePath)
{
    Gdiplus::GdiplusStartupInput gpStartupInput;
    ULONG_PTR gpToken;
    Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, NULL);
    HBITMAP result = NULL;
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(pFilePath, false);
    if (bitmap)
    {
        bitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &result);
        result = ScaleBitmapToRes(result);
        delete bitmap;
    }
    Gdiplus::GdiplusShutdown(gpToken);
    return result;
}

HBITMAP GetHBITMAPFromImageResource(int resourceID);

HBITMAP ScaleBitmapToRes(HBITMAP oldBitmap)
{
    Gdiplus::GdiplusStartupInput gpStartupInput;
    ULONG_PTR gpToken;
    Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, NULL);

    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(oldBitmap, 0);

    HBITMAP hbitmap = 0;

    if (bitmap)
    {
        UINT newWidth = GetSystemMetrics(SM_CXSCREEN);
        UINT newHeight = GetSystemMetrics(SM_CYSCREEN);
        Gdiplus::Bitmap bitmap2 = Gdiplus::Bitmap(newWidth, newHeight, PixelFormat32bppARGB);

        Gdiplus::Graphics* graphic = Gdiplus::Graphics::FromImage(&bitmap2);
        if (graphic)
        {
            graphic->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);

            Gdiplus::ImageAttributes attributes;
            attributes.SetWrapMode(Gdiplus::WrapMode::WrapModeTileFlipXY, 0xFF000000, 0);

            UINT oldWidth = bitmap->GetWidth();
            UINT oldHeight = bitmap->GetHeight();

            graphic->DrawImage(bitmap, Gdiplus::Rect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)), 0, 0, bitmap->GetWidth(), bitmap->GetHeight(), Gdiplus::Unit::UnitPixel, &attributes, (Gdiplus::DrawImageAbort)0, 0);

            bitmap2.GetHBITMAP(Gdiplus::Color(0xFF000000), &hbitmap);

            delete graphic;
        }

        delete bitmap;
    }
    Gdiplus::GdiplusShutdown(gpToken);

    return hbitmap;
}

HBITMAP GetHBITMAPFromImageResource(int resourceID)
{
    static auto SHCreateStreamOnModuleResourceW = (HRESULT(WINAPI*)(HMODULE hModule, LPCWSTR pwszName, LPCWSTR pwszType, IStream * *ppStream))(GetProcAddress(LoadLibrary(L"shcore.dll"), (PCSTR)109));

    IStream* Stream = 0;
    HRESULT hr = SHCreateStreamOnModuleResourceW(HINST_THISCOMPONENT, MAKEINTRESOURCEW(resourceID), L"IMAGE", &Stream);
    if (FAILED(hr))
    {
        return 0;
    }
    Gdiplus::GdiplusStartupInput gpStartupInput;
    ULONG_PTR gpToken;
    Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, NULL);

    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromStream(Stream);

    HBITMAP hbitmap = 0;

    if (bitmap)
    {
        bitmap->GetHBITMAP(Gdiplus::Color(0xFF000000), &hbitmap);

        //inefficient, but i cba to clean up my usage of gdiplus
        hbitmap = ScaleBitmapToRes(hbitmap);
    }
    Stream->Release();
    Gdiplus::GdiplusShutdown(gpToken);

    return hbitmap;
}

bool BitmapAspectRatioEqualsScreen(HBITMAP h)
{
	bool v1 = false; // di

	BITMAP pv;
	if (GetObjectW(h, (int)sizeof(BITMAP), &pv))
	{
		float v6;
		float v7;
		
		float w = (float)GetSystemMetrics(SM_CXSCREEN);
		float ratio = w / (float)GetSystemMetrics(SM_CYSCREEN);
		float imgRatio = (float)pv.bmWidth / (float)pv.bmHeight;
		if (ratio <= imgRatio)
			v6 = (float)pv.bmWidth / (float)pv.bmHeight;
		else
			v6 = ratio;
		if (imgRatio <= ratio)
			v7 = (float)pv.bmWidth / (float)pv.bmHeight;
		else
			v7 = ratio;
		if (ratio <= imgRatio)
			ratio = (float)pv.bmWidth / (float)pv.bmHeight;
		if ((v6 - v7) / ratio < 0.01)
			v1 = true;
	}
	return v1;
}

bool GetBitmapResolution(HBITMAP ha, int* w, int* h)
{
	bool v1 = false;

	BITMAP pv;
	if (GetObjectW(ha, sizeof(BITMAP), &pv))
	{
		if (w && h)
		{
			*w = pv.bmWidth;
			*h = pv.bmHeight;
			v1 = true;
		}
	}
	return v1;
}

//i really need to clean this shit up, this is a fucking mess, but i cba rn
HRESULT CBackground::GetBackground(HBITMAP* OutBitmap)
{
    *OutBitmap = 0;

    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    float lastDistance = 1.0e7;

    int ScreenWidthCopy = ScreenWidth;
    int Height = ScreenHeight;
    int iterations = 10;
    int swapIt = 10;

    float ScreenRatio = (float)ScreenWidth / (float)ScreenHeight;

    ImageData* dataToUse = 0;

    int i = 0;

    do
    {
        ratioItem item = ratioItems[i];
        float difference = ScreenRatio - item.ratio;
        if (difference < 0.0)
            difference = difference * -1.0;
        if (lastDistance <= difference)
        {
            ScreenHeight = Height;
            ScreenWidth = (int)ScreenWidthCopy;
            i++;
            swapIt = --iterations;
            continue;
        }

        lastDistance = difference;
        dataToUse = nullptr;
        if (!_UseOEMBackground())
        {
	        if (item.numberOfImages <= 0)
            {
                dataToUse = &item.RatioImageDatas[0];
            }
            else
            {
	            int imageDataNumIterations = 0;
	            bool bBroke = false;
                ImageData* ImageDataIt = &item.RatioImageDatas[0];
                while (ImageDataIt->w != ScreenWidth || ImageDataIt->h != ScreenHeight)
                {
                    ++imageDataNumIterations;
                    if (imageDataNumIterations >= item.numberOfImages)
                    {
                        bBroke = true;
                        break;
                    }

                    ImageDataIt = &item.RatioImageDatas[imageDataNumIterations];
                }
                if (!bBroke)
                    dataToUse = ImageDataIt;

                if (!dataToUse)
                    dataToUse = &item.RatioImageDatas[0];
            }

            ScreenHeight = Height;
            ScreenWidth = ScreenWidthCopy;
            i++;
            swapIt = --iterations;

            continue;
        }
        int imageDataIterations = 0;
        if (item.numberOfImages <= 0)
        {
            dataToUse = &item.RatioImageDatas[0];
            ScreenHeight = Height;
            ScreenWidth = (int)ScreenWidthCopy;
            continue;
        }
        bool bBroke = false;
        ImageData* OEMImageDataIt = 0;
        while (true)
        {
            bool bNotEqual = false;
            OEMImageDataIt = &item.RatioImageDatas[imageDataIterations];
            if (OEMImageDataIt->w != ScreenWidthCopy || OEMImageDataIt->h != Height)
            {
                if (OEMBackgroundFileExists(OEMImageDataIt))
                    dataToUse = OEMImageDataIt;

                bNotEqual = true;
            }

            if (!bNotEqual && OEMBackgroundFileExists(OEMImageDataIt))
            {
                break;
            }
            ++imageDataIterations;
            if (imageDataIterations >= item.numberOfImages)
            {
                bBroke = true;
                break;
            }
        }
        if (!bBroke)
            dataToUse = OEMImageDataIt;

        iterations = swapIt;
        if (!dataToUse)
        {
            ScreenHeight = Height;
            ScreenWidth = (int)ScreenWidthCopy;

            if (item.numberOfImages <= 0)
            {
                dataToUse = &item.RatioImageDatas[0];
            }
            else
            {
	            int imageDataNumIterations = 0;
	            bBroke = false;
                ImageData* ImageDataIt = &item.RatioImageDatas[0];
                while (ImageDataIt->w != ScreenWidth || ImageDataIt->h != ScreenHeight)
                {
                    ++imageDataNumIterations;
                    if (imageDataNumIterations >= item.numberOfImages)
                    {
                        bBroke = true;
                        break;
                    }
                    ImageDataIt = &item.RatioImageDatas[imageDataNumIterations];
                }
                if (!bBroke)
                    dataToUse = ImageDataIt;
                if (!dataToUse)
                    dataToUse = &item.RatioImageDatas[0];
            }

        }
        ScreenHeight = Height;
        ScreenWidth = (int)ScreenWidthCopy;
        swapIt = --iterations;
        i++;
    } while (iterations);

    WCHAR unexpandedPath[] = L"C:\\Windows\\system32\\oobe\\info\\backgrounds\\backgroundDefault.jpg";
	WCHAR path[MAX_PATH];
	path[0] = '\0';
	ExpandEnvironmentStringsW(unexpandedPath,path,MAX_PATH);

    if (_UseOEMBackground() && (PathFileExistsW(path) || OEMBackgroundFileExists(dataToUse)))
    {
        if (OEMBackgroundFileExists(dataToUse))
        {
            WCHAR Dst[264];

            if (ExpandEnvironmentStringsW(dataToUse->OEMPath, Dst, MAX_PATH) <= MAX_PATH)
            {
                HBITMAP bitmapDefault = GetHBITMAPFromImageFile(path);

                bool bDefaultBetter = false; //check if src fallback is better, just so u can have nice images if u have a src image

                int dw = 0; int dh = 0;
                if (GetBitmapResolution(bitmapDefault, &dw, &dh) && BitmapAspectRatioEqualsScreen(bitmapDefault))
                {
                    if ((dw > dataToUse->w || dh > dataToUse->h) && dw <= GetSystemMetrics(SM_CXSCREEN) && dh <= GetSystemMetrics(SM_CYSCREEN))
                        bDefaultBetter = true;
                }

                if (!bDefaultBetter)
                {
                    wcscpy_s(path, MAX_PATH, Dst);
                    DeleteObject(bitmapDefault);
                }
            }
        }

        HBITMAP bitmaplocal = GetHBITMAPFromImageFile(path);
        *OutBitmap = bitmaplocal;
    }
    else
    {
        unsigned long long resIdToUse = IsOS(OS_ANYSERVER) ? dataToUse->resIdServer : dataToUse->resIdConsumer;
        *OutBitmap = GetHBITMAPFromImageResource(resIdToUse);
    }

    if (*OutBitmap)
    {
        return BitmapAspectRatioEqualsScreen(*OutBitmap) == true ? S_OK : E_FAIL;
    }

    return S_OK;
}

bool CBackground::_UseOEMBackground()
{
	HKEY result;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI\\Background", 0, KEY_READ, &result) == S_OK)
	{
		DWORD size = sizeof(DWORD);
		DWORD type = REG_DWORD;

		DWORD data;
		if (RegQueryValueExW(result, L"OEMBackground", nullptr, &type, (LPBYTE)&data, &size) == S_OK)
		{
			return (bool)data;
		}
	}
	return false;
}
