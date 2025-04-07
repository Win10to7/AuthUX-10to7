#pragma once
#include "pch.h"
#include "DirectUI/DirectUI.h"
#include <Gdiplus.h>
#include <gdiplusheaders.h>
#include <gdiplusinit.h>

static HRESULT SetContentAndAcc(DirectUI::Element* element, const wchar_t* content)
{
	HRESULT result = S_OK;

	if (content)
	{
		result = element->SetContentString(content);
		if (SUCCEEDED(result))
			return element->SetAccName(content);
	}
	else
	{
		result = element->RemoveLocalValue(DirectUI::Element::ContentProp);
		if (SUCCEEDED(result))
			return element->RemoveLocalValue(DirectUI::Element::AccNameProp);
	}
	return result;
}

static UINT GetScreenDPI()
{
	UINT DPI;

	HDC DC = GetDC(nullptr);

	if (DC)
		DPI = GetDeviceCaps(DC, 90);
	else
		DPI = 0;

	if (DC)
		ReleaseDC(nullptr, DC);

	return DPI;
}

static HRESULT StringStringAllocCopy(const wchar_t* Src, const wchar_t** a2)
{
	const wchar_t* newString = (const wchar_t*)DirectUI::HAlloc((wcslen(Src)+1)*2);
	memcpy((void*)newString,Src, (wcslen(Src) + 1) * 2);
	*a2 = newString;
	return S_OK;
}

static DirectUI::Value* CreateGraphicFromHBITMAP(HBITMAP oldbitmap, UINT width, UINT height, BYTE blendMode)
{
	Gdiplus::GdiplusStartupInput gpStartupInput;
	ULONG_PTR gpToken;
	Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, NULL);

	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(oldbitmap, 0);

	HBITMAP hbitmap = 0;

	if (bitmap)
	{
		//UINT newWidth = GetSystemMetrics(SM_CXSCREEN);
		//UINT newHeight = GetSystemMetrics(SM_CYSCREEN);
		Gdiplus::Bitmap bitmap2 = Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

		Gdiplus::Graphics* graphic = Gdiplus::Graphics::FromImage(&bitmap2);
		if (graphic)
		{
			graphic->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);

			Gdiplus::ImageAttributes attributes;
			attributes.SetWrapMode(Gdiplus::WrapMode::WrapModeTileFlipXY, 0xFF000000, 0);

			UINT oldWidth = bitmap->GetWidth();
			UINT oldHeight = bitmap->GetHeight();

			graphic->DrawImage(bitmap, Gdiplus::Rect(0, 0, width, height), 0, 0, bitmap->GetWidth(), bitmap->GetHeight(), Gdiplus::Unit::UnitPixel, &attributes, (Gdiplus::DrawImageAbort)0, 0);

			bitmap2.GetHBITMAP(Gdiplus::Color(0xFF000000), &hbitmap);

			delete graphic;
		}

		delete bitmap;
	}
	Gdiplus::GdiplusShutdown(gpToken);

	bool bRtl = false;
	if (blendMode == 2 || blendMode == 7)
		bRtl = true;
	DirectUI::Value* Graphic = DirectUI::Value::CreateGraphic(hbitmap, blendMode, 0xFFFFFFFF, false, false, bRtl);
	if (!Graphic)
		DeleteObject(hbitmap);

	return Graphic;
}

static HRESULT SetBackgroundFromHBITMAP(DirectUI::Element* element, HBITMAP bitmap, UINT width, UINT height)
{
	DirectUI::Value* GraphicFromHBITMAP = CreateGraphicFromHBITMAP(bitmap, width, height, 4);
	DeleteObject(bitmap);
	if (GraphicFromHBITMAP)
	{
		RETURN_IF_FAILED(element->SetValue(DirectUI::Element::BackgroundProp, 1, GraphicFromHBITMAP));
		GraphicFromHBITMAP->Release();
	}
	else
		return E_FAIL;

	return S_OK;
}

static int SetLayoutPosDownTree(int val, DirectUI::Element* element1, DirectUI::Element* element2)
{
	int result;
	DirectUI::Element* ImmediateChild;

	int val_1 = val;
	for (DirectUI::Element* i = element1; ; i = ImmediateChild)
	{
		result = i->SetLayoutPos(val_1);
		if (element1 == element2)
			break;
		ImmediateChild = element1->GetImmediateChild(element2);
		val_1 = val;
		element1 = ImmediateChild;
	}

	return result;
}

static int SetVisibleDownTree(bool val, DirectUI::Element* element1, DirectUI::Element* element2)
{
	int result;
	DirectUI::Element* ImmediateChild;

	bool val_1 = val;
	for (DirectUI::Element* i = element1; ; i = ImmediateChild)
	{
		result = i->SetVisible(val_1);
		if (element1 == element2)
			break;
		ImmediateChild = element1->GetImmediateChild(element2);
		val_1 = val;
		element1 = ImmediateChild;
	}
	return result;
}

static int SetEnabledDownTree(bool val, DirectUI::Element* element1, DirectUI::Element* element2)
{
	int result;
	DirectUI::Element* ImmediateChild;

	bool val_1 = val;
	for (DirectUI::Element* i = element1; ; i = ImmediateChild)
	{
		result = i->SetEnabled(val_1);
		if (element1 == element2)
			break;
		ImmediateChild = element1->GetImmediateChild(element2);
		val_1 = val;
		element1 = ImmediateChild;
	}
	return result;
}

static HRESULT _TraverseTree(DirectUI::Element* elm, DirectUI::IClassInfo* classInfo, HRESULT(STDMETHODCALLTYPE* callbackFunc)(DirectUI::Element*, LPVOID param), LPVOID param)
{
	if (elm->GetClassInfoW()->IsSubclassOf(classInfo))
		RETURN_IF_FAILED(callbackFunc(elm,param));

	DirectUI::Value* childVal;
	auto Children = elm->GetChildren(&childVal);
	if (Children)
	{
		for (UINT i = 0; i < Children->GetSize(); ++i)
		{
			auto Child = Children->GetItem(i);
			RETURN_IF_FAILED(_TraverseTree(Child,classInfo,callbackFunc,param));
		}
	}

	childVal->Release();

	return S_OK;
}

static bool IsElementOfClass(DirectUI::Element* element, const wchar_t* className)
{
	return _wcsicmp(element->GetClassInfoW()->GetName(), className) == 0;
}
