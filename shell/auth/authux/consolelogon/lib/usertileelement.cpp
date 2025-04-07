#include "pch.h"
#include "usertileelement.h"

#include "combobox.h"
#include "duiutil.h"
#include "labeledcheckbox.h"
#include "restrictededit.h"
#include "zoomableelement.h"

DirectUI::IClassInfo* CDUIUserTileElement::Class = nullptr;

DirectUI::IClassInfo* CDUIUserTileElement::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* CDUIUserTileElement::GetClassInfoPtr()
{
	return Class;
}

HRESULT CDUIUserTileElement::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie,
	DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<CDUIUserTileElement, int>(3, pParent, pdwDeferCookie, ppElement);
}

static DirectUI::PropertyInfoData dataimpTileZoomedProp;
static const int vvimpTileZoomedProp[] = { 2, (int)0x0FFFFFFFF }; //idfk, this is some weirdass shit
static const DirectUI::PropertyInfo impTileZoomedProp =
{
	.pszName = L"TileZoomed",
	.fFlags = 0xA,
	.fGroups = 0,
	.pValidValues = vvimpTileZoomedProp,
	.DefaultProc = DirectUI::Value::GetBoolFalse,
	.pData = &dataimpTileZoomedProp
};

const DirectUI::PropertyInfo* CDUIUserTileElement::TileZoomedProp()
{
	return &impTileZoomedProp;
}

HRESULT CDUIUserTileElement::Register()
{
	static const DirectUI::PropertyInfo* const s_rgProperties[] =
{
		&impTileZoomedProp
	};
	return DirectUI::ClassInfo<CDUIUserTileElement, DirectUI::Button>::RegisterGlobal(HINST_THISCOMPONENT, L"UserTile", s_rgProperties, ARRAYSIZE(s_rgProperties));
}

bool CDUIUserTileElement::GetTileZoomed()
{
	DirectUI::Value* pv = GetValue(TileZoomedProp, 0, nullptr);
	bool v = pv->GetBool();
	pv->Release();
	return v;
}

HRESULT CDUIUserTileElement::SetTileZoomed(bool v)
{
	DirectUI::Value* pv = DirectUI::Value::CreateBool(v);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = SetValue(TileZoomedProp, 0, pv);
		pv->Release();
	}

	return hr;
}

void CDUIUserTileElement::OnEvent(DirectUI::Event* pEvent)
{
	Button::OnEvent(pEvent);
}

HRESULT CDUIUserTileElement::_SetFieldInitialVisibility(DirectUI::Element* field,
	CFieldWrapper* fieldData)
{
	bool isVisible = false;

	LCPD::CredentialFieldKind kind = LCPD::CredentialFieldKind_StaticText;
	if (fieldData->m_dataSourceCredentialField.Get() != nullptr)
		RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->get_Kind(&kind));

	if (kind == LCPD::CredentialFieldKind_CommandLink)
		isVisible = true;
	else if (kind == LCPD::CredentialFieldKind_StaticText)
	{
		LCPD::CredentialTextSize size = fieldData->m_size;

		if (fieldData->m_dataSourceCredentialField.Get() != nullptr)
		{
			Microsoft::WRL::ComPtr<LCPD::ICredentialTextField> textField;
			RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->QueryInterface(IID_PPV_ARGS(&textField)));

			RETURN_IF_FAILED(textField->get_TextSize(&size));
		}

		if (GetTileZoomed() && size == LCPD::CredentialTextSize_Large)
			isVisible = true;
		else if (!GetTileZoomed() && size == LCPD::CredentialTextSize_Small)
			isVisible = true;
		else
			isVisible = false;
	}

	RETURN_IF_FAILED(field->SetLayoutPos(isVisible ? -1 : -3));

	return field->SetVisible(isVisible);
}

HRESULT CDUIUserTileElement::_CreateElementArrays()
{
	size_t size = fieldsArray.GetSize() * sizeof(DirectUI::Element*);

	m_containersArray = (CDUIFieldContainer**)DirectUI::HAlloc(size);
	if (m_containersArray)
		memset((void*)m_containersArray, 0, size);
	else
		return E_OUTOFMEMORY;

	m_elementsArray = (DirectUI::Element**)DirectUI::HAlloc(size);
	if (m_elementsArray)
		memset((void*)m_elementsArray, 0, size);
	else
		return E_OUTOFMEMORY;

	return S_OK;
}

UINT CDUIUserTileElement::_FindFieldInsertionIndex(int fieldIndex)
{
	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(fieldIndex,fieldData));

	const wchar_t* idToFind = L"NonSelectorFieldsFrame";
	if (fieldData->m_isSelectorField)
		idToFind = L"SelectorFieldFrame";

	auto Frame = FindDescendent(DirectUI::StrToID(idToFind));
	if (fieldIndex)
	{
		for (int i = fieldIndex - 1; !Frame->IsDescendent(m_containersArray[i]); --i)
		{
			fieldIndex--;
			if (!fieldIndex)
				return 0;
		}
		return m_containersArray[fieldIndex - 1]->GetIndex();
	}

	return 0;
}

HRESULT CDUIUserTileElement::_AddFieldContainer(int index, DirectUI::Element* Parent, CDUIFieldContainer** outContainer)
{
	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	DirectUI::Element* SelectorFieldFrame = FindDescendent(DirectUI::StrToID(L"SelectorFieldFrame"));
	DirectUI::Element* NonSelectorFieldsFrame = FindDescendent(DirectUI::StrToID(L"NonSelectorFieldsFrame"));

	CDUIFieldContainer* FieldContainer;
	RETURN_IF_FAILED(CDUIFieldContainer::Create(Parent,0,(DirectUI::Element**)&FieldContainer));

	auto scopeExit1 = wil::scope_exit([&]() -> void { FieldContainer->Destroy(false); });

	DirectUI::Element* FieldContainerElement;
	RETURN_IF_FAILED(m_xmlParser->CreateElement(L"FieldContainerTemplate",nullptr,FieldContainer,0,&FieldContainerElement));
	auto scopeExit2 = wil::scope_exit([&]() -> void { FieldContainerElement->Destroy(false); });

	DirectUI::Layout* fillLayout;
	RETURN_IF_FAILED(DirectUI::FillLayout::Create(&fillLayout));
	auto scopeExit3 = wil::scope_exit([&]() -> void { fillLayout->Destroy(); });

	RETURN_IF_FAILED(FieldContainer->SetLayout(fillLayout));

	RETURN_IF_FAILED(FieldContainer->Add(FieldContainerElement));

	UINT FieldInsertionIndex = -1;
	DirectUI::Element* FieldFrameToUse;

	if (fieldData->m_isSelectorField)
	{
		FieldInsertionIndex = _FindFieldInsertionIndex(index);
		FieldFrameToUse = SelectorFieldFrame;
	}
	else
	{
		FieldInsertionIndex = _FindFieldInsertionIndex(index);
		FieldFrameToUse = NonSelectorFieldsFrame;
	}

	RETURN_IF_FAILED(FieldFrameToUse->Insert(FieldContainer,FieldInsertionIndex));

	//FieldFrameToUse->SetActive(3);
	//FieldContainer->SetActive(3);
	//FieldFrameToUse->SetActive(0);
	//FieldContainer->SetActive(0);

	*outContainer = FieldContainer;

	scopeExit1.release();
	scopeExit2.release();
	scopeExit3.release();

	return S_OK;
}

HRESULT CDUIUserTileElement::_AddField(DirectUI::Element* field, int index, DirectUI::Element* Parent,
	CDUIFieldContainer** OutContainer)
{
	CDUIFieldContainer* lContainer = 0;
	RETURN_IF_FAILED(_AddFieldContainer(index,Parent,&lContainer));

	auto scopeExit = wil::scope_exit([&]() -> void {lContainer->Destroy(true);});
	RETURN_IF_FAILED(lContainer->AddField(field));

	*OutContainer = lContainer;
	scopeExit.release();

	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateStringField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	DirectUI::Element* element;
	RETURN_IF_FAILED(CDUIZoomableElement::Create(Parent,nullptr,&element));
	//RETURN_IF_FAILED(DirectUI::Element::Create(0,Parent,0,&element));
	auto scopeExit = wil::scope_exit([&]() -> void {element->Destroy(true);});
	if (!fieldData->m_isSelectorField)
		element->SetPadding(0, MulDiv(-5, GetScreenDPI(), 96), 0, MulDiv(5, GetScreenDPI(), 96));

	//element->SetMargin(1, 0, 1, 0);
	LCPD::CredentialTextSize size = fieldData->m_size;
	CoTaskMemNativeString label;

	if (fieldData->m_dataSourceCredentialField)
	{
		Microsoft::WRL::ComPtr<LCPD::ICredentialTextField> textField;
		RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->QueryInterface(IID_PPV_ARGS(&textField)));

		RETURN_IF_FAILED(textField->get_TextSize(&size));

		Microsoft::WRL::Wrappers::HString content;
		RETURN_IF_FAILED(textField->get_Content(content.ReleaseAndGetAddressOf()));

		label.Free();
		RETURN_IF_FAILED(label.Initialize(content.GetRawBuffer(nullptr)));
	}
	else
		label.Initialize(fieldData->m_label);

	if (label.GetCount() > 0)
	{
		RETURN_IF_FAILED(element->SetContentString(label.Get()));
	}

	if (size == LCPD::CredentialTextSize_Large)
	{
		RETURN_IF_FAILED(element->SetClass(L"LargeText"));
	}
	else if (size == LCPD::CredentialTextSize_Small)
	{
		RETURN_IF_FAILED(element->SetClass(L"SmallText"));
	}

	RETURN_IF_FAILED(element->SetAccessible(true));

	RETURN_IF_FAILED(element->SetAccRole(41));

	if (label.GetCount() > 0)
	{
		RETURN_IF_FAILED(element->SetAccName(label.Get()));
	}

	//todo: set tooltip

	HRESULT hr = *OutContainer ? (*OutContainer)->AddField(element) : _AddField(element, index, this, OutContainer);

	RETURN_IF_FAILED(hr);

	_SetFieldInitialVisibility(*OutContainer,fieldData);

	*outElement = element;

	scopeExit.release();
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateEditField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	CDUIRestrictedEdit* restrictedEdit;
	RETURN_IF_FAILED(CDUIRestrictedEdit::Create(Parent,0, (DirectUI::Element**)&restrictedEdit));
	auto scopeExit = wil::scope_exit([&]() -> void {restrictedEdit->Destroy(true);});

	restrictedEdit->m_scenario = LCPD::CredProvScenario_Logon;
	restrictedEdit->m_fieldData = fieldData->m_dataSourceCredentialField;

	Microsoft::WRL::ComPtr<LCPD::ICredentialEditField> editFieldData;
	RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->QueryInterface(IID_PPV_ARGS(&editFieldData)));

	bool bIsPasswordField;
	RETURN_IF_FAILED(editFieldData->get_IsPasswordField(&bIsPasswordField));

	Microsoft::WRL::Wrappers::HString label;
	RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->get_Label(label.ReleaseAndGetAddressOf()));

	Microsoft::WRL::Wrappers::HString content;
	RETURN_IF_FAILED(editFieldData->get_Content(content.ReleaseAndGetAddressOf()));

	RETURN_IF_FAILED(restrictedEdit->SetAccessible(true));
	RETURN_IF_FAILED(restrictedEdit->SetAccRole(42));
	RETURN_IF_FAILED(restrictedEdit->SetAccName(content.GetRawBuffer(nullptr)));

	if (bIsPasswordField)
		RETURN_IF_FAILED(restrictedEdit->SetAccValue(content.GetRawBuffer(nullptr)));

	StringStringAllocCopy(label.GetRawBuffer(nullptr), &restrictedEdit->m_hintText);

	const wchar_t* textClass = L"ClearTextEdit";
	if (bIsPasswordField)
		textClass = L"PasswordEdit";

	restrictedEdit->m_maxTextLength = 127;

	RETURN_IF_FAILED(restrictedEdit->SetClass(textClass));

	if (bIsPasswordField)
		RETURN_IF_FAILED(restrictedEdit->SetEncodedContentString(content.GetRawBuffer(nullptr)));
	else
		RETURN_IF_FAILED(restrictedEdit->SetContentString(content.GetRawBuffer(nullptr)));

	HRESULT hr = *OutContainer ? (*OutContainer)->AddField(restrictedEdit) : _AddField(restrictedEdit, index, this, OutContainer);
	RETURN_IF_FAILED(hr);

	*outElement = restrictedEdit;
	_SetFieldInitialVisibility(*OutContainer, fieldData);

	scopeExit.release();
	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateCommandLinkField(int index, DirectUI::Element* Parent,
	DirectUI::Element** outElement, CDUIFieldContainer** OutContainer)
{
	DirectUI::Element* element;
	RETURN_IF_FAILED(DirectUI::Button::Create(3,Parent,0,&element));
	auto scopeExit = wil::scope_exit([&]() -> void {element->Destroy(true);});

	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	Microsoft::WRL::Wrappers::HString label;
	RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->get_Label(label.ReleaseAndGetAddressOf()));

	if (label.Length() > 0)
		RETURN_IF_FAILED(element->SetContentString(label.GetRawBuffer(nullptr)));

	RETURN_IF_FAILED(element->SetClass(L"SmallText"));

	RETURN_IF_FAILED(element->SetAccessible(true));

	RETURN_IF_FAILED(element->SetAccRole(30));

	if (label.Length() > 0)
		RETURN_IF_FAILED(element->SetAccName(label.GetRawBuffer(nullptr)));

	//set tooltip here, but i cba

	RETURN_IF_FAILED(element->SetClass(L"Link")); //nvm, i guess we want the class to be link now...

	RETURN_IF_FAILED(element->SetID(L"CommandLink"));

	RETURN_IF_FAILED(element->SetActive(3));

	HRESULT hr = *OutContainer ? (*OutContainer)->AddField(element) : _AddField(element, index, this, OutContainer);
	RETURN_IF_FAILED(hr);

	*outElement = element;
	_SetFieldInitialVisibility(*OutContainer, fieldData);

	scopeExit.release();

	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateTileImageField(const wchar_t* pszLabel, HBITMAP bitmap,
	DirectUI::Element** OutElement)
{
	DirectUI::Element* Picture = FindDescendent(DirectUI::StrToID(L"Picture"));
	DirectUI::Element* PictureContainer = FindDescendent(DirectUI::StrToID(L"PictureContainer"));
	DirectUI::Element* PictureOverlay = FindDescendent(DirectUI::StrToID(L"PictureOverlay"));

	HRESULT hr = S_OK;
	if (bitmap)
		hr = SetBackgroundFromHBITMAP(Picture, bitmap, 126, 126);
	else
		hr = Picture->SetBackgroundColor(0);

	RETURN_IF_FAILED(hr);

	if (pszLabel)
		RETURN_IF_FAILED(PictureContainer->SetAccName(pszLabel));

	//todo: check whether i gotta do that scenario credui shit, i believe it handles that extra shine

	if (OutElement)
		*OutElement = Picture;

	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateSubmitButton(int index, DirectUI::Button** outButton,
	DirectUI::Element** outElement)
{
	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	if (!m_containersArray[index])
		RETURN_IF_FAILED(_CreateFieldByIndex(index));

	Microsoft::WRL::Wrappers::HString label;
	RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->get_Label(label.ReleaseAndGetAddressOf()));

	RETURN_IF_FAILED(static_cast<CDUIFieldContainer*>(m_containersArray[index])->ShowSubmitButton(label.GetRawBuffer(nullptr),outButton));

	*outElement = *outButton;
	//*OutContainer = containersArray[index];
	return _SetFieldInitialVisibility(*outElement,fieldData);
}

HRESULT CDUIUserTileElement::_CreateCheckboxField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	CDUILabeledCheckbox* checkbox;
	RETURN_IF_FAILED(CDUILabeledCheckbox::Create(Parent,0,(DirectUI::Element**)&checkbox));
	auto scopeExit = wil::scope_exit([&]() -> void {checkbox->Destroy(0);});

	DirectUI::Element* labeledCheckbox;
	RETURN_IF_FAILED(m_xmlParser->CreateElement(L"LabeledCheckboxTemplate",0,checkbox,0,&labeledCheckbox));
	auto scopeExit2 = wil::scope_exit([&]() -> void {labeledCheckbox->Destroy(0);});

	DirectUI::Layout* layout;
	RETURN_IF_FAILED(DirectUI::FillLayout::Create(&layout));
	auto scopeExit3 = wil::scope_exit([&]() -> void {layout->Destroy();});

	RETURN_IF_FAILED(checkbox->SetLayout(layout));

	RETURN_IF_FAILED(checkbox->Add(labeledCheckbox));

	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	Microsoft::WRL::ComPtr<LCPD::ICheckBoxField> checkboxField;
	RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->QueryInterface(IID_PPV_ARGS(&checkboxField))); // 23

	bool isChecked = false;
	RETURN_IF_FAILED(checkboxField->get_Checked(&isChecked));

	Microsoft::WRL::Wrappers::HString label;
    RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->get_Label(label.ReleaseAndGetAddressOf()));

	RETURN_IF_FAILED(checkbox->Configure(isChecked,label.GetRawBuffer(nullptr)));

	if (label.Length() > 0)
		checkbox->SetAccName(label.GetRawBuffer(nullptr));

	HRESULT hr = *OutContainer ? (*OutContainer)->AddField(checkbox) : _AddField(checkbox, index, this, OutContainer);
	RETURN_IF_FAILED(hr);

	*outElement = checkbox;
	_SetFieldInitialVisibility(*OutContainer, fieldData);

	scopeExit.release();
	scopeExit2.release();
	scopeExit3.release();

	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateComboBoxField(int index, DirectUI::Element* Parent, DirectUI::Element** outElement,
	CDUIFieldContainer** OutContainer)
{
	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	CDUIComboBox* comboBox;
	RETURN_IF_FAILED(CDUIComboBox::Create(Parent,0,(DirectUI::Element**)&comboBox));
	auto scopeExit = wil::scope_exit([&]() -> void {comboBox->Destroy(true);});

	Microsoft::WRL::ComPtr<LCPD::IComboBoxField> comboBoxField;
	RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->QueryInterface(IID_PPV_ARGS(&comboBoxField)));

	Microsoft::WRL::ComPtr<WFC::IObservableVector<HSTRING>> observableItems;
	RETURN_IF_FAILED(comboBoxField->get_Items(&observableItems));

	Microsoft::WRL::ComPtr<WFC::IVector<HSTRING>> items;
	RETURN_IF_FAILED(observableItems.As<WFC::IVector<HSTRING>>(&items));

	UINT numItems;
	RETURN_IF_FAILED(items->get_Size(&numItems));

	for (int i = 0; i < numItems; ++i)
	{
		Microsoft::WRL::Wrappers::HString item;
		RETURN_IF_FAILED(items->GetAt(i,item.ReleaseAndGetAddressOf()));
		RETURN_HR_IF(E_FAIL, comboBox->AddStringEx(item.GetRawBuffer(nullptr)) == -1);
	}

	int initialSelection;
	RETURN_IF_FAILED(comboBoxField->get_SelectedIndex(&initialSelection));

	RETURN_IF_FAILED(comboBox->SetSelectionEx(initialSelection));

	HRESULT hr = *OutContainer ? (*OutContainer)->AddField(comboBox) : _AddField(comboBox, index, this, OutContainer);
	RETURN_IF_FAILED(hr);

	*outElement = comboBox;
	_SetFieldInitialVisibility(*OutContainer, fieldData);

	scopeExit.release();

	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateFieldByIndex(int index)
{
	auto slot = &m_elementsArray[index];
	if (*slot)
		return S_OK;

	CFieldWrapper* fieldData;
	RETURN_IF_FAILED(fieldsArray.GetAt(index,fieldData));

	LCPD::CredentialFieldKind kind = LCPD::CredentialFieldKind_StaticText;
	if (fieldData->m_dataSourceCredentialField)
	{
		RETURN_IF_FAILED(fieldData->m_dataSourceCredentialField->get_Kind(&kind));
	}

	if (kind <= LCPD::CredentialFieldKind_Invalid)
		return E_INVALIDARG;

	switch (kind)
	{
	case LCPD::CredentialFieldKind_CommandLink:
		return _CreateCommandLinkField(index, this, slot, &m_containersArray[index]);

	case LCPD::CredentialFieldKind_EditText:
		return _CreateEditField(index, this, slot, &m_containersArray[index]);

	case LCPD::CredentialFieldKind_CheckBox:
		return _CreateCheckboxField(index, this, slot, &m_containersArray[index]);

	case LCPD::CredentialFieldKind_ComboBox:
		return _CreateComboBoxField(index, this, slot, &m_containersArray[index]);

	case LCPD::CredentialFieldKind_SubmitButton:
		return _CreateSubmitButton(index, &m_submitButton,slot);
	}

	return E_INVALIDARG;
}

HRESULT CDUIUserTileElement::_CreateFieldsForDeselected()
{
	if (m_scenario == LCPD::CredProvScenario_Logon)
	{
		//todo: handle that slow download shit maybe idfk
	}

	RETURN_IF_FAILED(_CreateElementArrays());

	bool bHasTileImageField = false;
	for (int i = 0; i < fieldsArray.GetSize(); ++i)
	{
		CFieldWrapper* field;
		RETURN_IF_FAILED(fieldsArray.GetAt(i,field));

		LCPD::CredentialFieldKind kind = LCPD::CredentialFieldKind_StaticText;
		if (field->m_dataSourceCredentialField)
		{
			RETURN_IF_FAILED(field->m_dataSourceCredentialField->get_Kind(&kind));
		}

		if (field->m_isSelectorField)
		{
			switch (kind)
			{
			case LCPD::CredentialFieldKind_Invalid:
				return E_INVALIDARG;

			case LCPD::CredentialFieldKind_TileImage:
				{
					//TODO: take from IUserTileImage
					HBITMAP bitmap = LoadBitmapW(HINST_THISCOMPONENT,MAKEINTRESOURCEW(IDB_DEFAULTPFP));
					if (SUCCEEDED(_CreateTileImageField(L"", bitmap, &m_elementsArray[i])))
						bHasTileImageField = true;
					break;
				}

			case LCPD::CredentialFieldKind_StaticText:
				RETURN_IF_FAILED(_CreateStringField(i,this,&m_elementsArray[i],&m_containersArray[i]));
				break;
			}
		}
	}

	if (!bHasTileImageField)
	{
		HBITMAP bitmap = LoadBitmapW(HINST_THISCOMPONENT,MAKEINTRESOURCEW(IDB_DEFAULTPFP));
		return _CreateTileImageField(L"",bitmap,nullptr);
	}

	return S_OK;
}

HRESULT CDUIUserTileElement::_CreateFieldsForSelected()
{
	if (m_bHasMadeSelectedFields)
		return S_OK;

	int submitButtonIndex = -1;

	int adjacentSubmitButtonIndex = -1;
	LCPD::ICredentialField* submitButtonField;
	RETURN_IF_FAILED(m_dataSourceCredential->get_SubmitButton(&submitButtonField)); // 40
	if (submitButtonField)
	{
		bool isVisibleInDeselectedTile = false;
		RETURN_IF_FAILED(submitButtonField->get_IsVisibleInDeselectedTile(&isVisibleInDeselectedTile));

		if (!isVisibleInDeselectedTile)
		{
			Microsoft::WRL::ComPtr<LCPD::ICredentialSubmitButtonField> submitButtonField2;
			RETURN_IF_FAILED(submitButtonField->QueryInterface(IID_PPV_ARGS(&submitButtonField2)));

			RETURN_IF_FAILED(submitButtonField2->get_AdjacentID((UINT*)&adjacentSubmitButtonIndex)); // ye this really shouldn't be done, but they do treat the unsigned int as an int, so idgaf
		}
		else
		{
			//not sure if we need to handle anything here
		}

	}

	for (int i = 0; i < fieldsArray.GetSize(); ++i)
	{
		CFieldWrapper* field;
		RETURN_IF_FAILED(fieldsArray.GetAt(i,field));

		if (field->m_isSelectorField)
			continue;

		LCPD::CredentialFieldKind kind = LCPD::CredentialFieldKind_StaticText;
		if (field->m_dataSourceCredentialField)
		{
			RETURN_IF_FAILED(field->m_dataSourceCredentialField->get_Kind(&kind));

			int fieldId = -1;
			RETURN_IF_FAILED(field->m_dataSourceCredentialField->get_ID((UINT*)&fieldId));

			if (fieldId != -1 && fieldId == adjacentSubmitButtonIndex)
				submitButtonIndex = i;
		}

		switch (kind)
		{
		default:
		case LCPD::CredentialFieldKind_Invalid:
			return E_INVALIDARG;

		case LCPD::CredentialFieldKind_StaticText:
			RETURN_IF_FAILED(_CreateStringField(i, this, &m_elementsArray[i], &m_containersArray[i]));
			break;
		case LCPD::CredentialFieldKind_CommandLink:
			RETURN_IF_FAILED(_CreateCommandLinkField(i, this, &m_elementsArray[i], &m_containersArray[i]));
			break;
		case LCPD::CredentialFieldKind_EditText:
			RETURN_IF_FAILED(_CreateEditField(i, this, &m_elementsArray[i], &m_containersArray[i]));
			break;
		case LCPD::CredentialFieldKind_TileImage:
			//tileIndex = i;
				break;
		case LCPD::CredentialFieldKind_CheckBox:
			RETURN_IF_FAILED(_CreateCheckboxField(i, this, &m_elementsArray[i], &m_containersArray[i]));
			break;
		case LCPD::CredentialFieldKind_ComboBox:
			RETURN_IF_FAILED(_CreateComboBoxField(i, this, &m_elementsArray[i], &m_containersArray[i]));
			break;
			//case CredentialFieldKind_SubmitButton:
			//	submitButtonIndex = i;
			//	break;
		}
	}
	if (submitButtonIndex != -1)
		RETURN_IF_FAILED(m_containersArray[submitButtonIndex]->ShowSubmitButton(L"Submit",&m_submitButton));
	//RETURN_IF_FAILED(_CreateSubmitButton(submitButtonIndex, &submitButton, &elementsArray[submitButtonIndex]));

	m_bHasMadeSelectedFields = true;

	return S_OK;
}
