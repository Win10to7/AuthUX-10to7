#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "controlbase.h"

class MessageOptionControl final : public Microsoft::WRL::RuntimeClass<ControlBase>
{
public:
	MessageOptionControl();
	~MessageOptionControl() override;

	HRESULT RuntimeClassInitialize(
		IConsoleUIView* view, MessageOptionFlag option,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

	HRESULT Repaint(IConsoleUIView* view);

	UINT m_VisibleControlSize;
	CoTaskMemNativeString m_label;
	bool m_isInitialized;
	bool m_hasFocus;
	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>> m_completion;
	MessageOptionFlag m_option;
};
