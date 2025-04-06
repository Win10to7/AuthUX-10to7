#include "pch.h"
#include "userlist.h"

DirectUI::IClassInfo* UserList::Class = nullptr;

DirectUI::IClassInfo* UserList::GetClassInfoW()
{
	return Class;
}

DirectUI::IClassInfo* UserList::GetClassInfoPtr()
{
	return Class;
}

HRESULT UserList::Create(DirectUI::Element* pParent, unsigned long* pdwDeferCookie, DirectUI::Element** ppElement)
{
	return DirectUI::CreateAndInit<UserList, int>(0, pParent, pdwDeferCookie, ppElement);
}

HRESULT UserList::Register()
{
	return DirectUI::ClassInfo<UserList, DirectUI::Element>::RegisterGlobal(HINST_THISCOMPONENT, L"UserList", nullptr, 0);
}

HRESULT UserList::Configure(DirectUI::DUIXmlParser* parser)
{
	m_xmlParser = parser;

	RETURN_IF_FAILED(m_xmlParser->CreateElement(L"UserListTemplate",nullptr,this,nullptr,&m_UserListContainer));

	RETURN_IF_FAILED(Add(m_UserListContainer));

	m_UserListSelector = (DirectUI::Selector*)m_UserListContainer->FindDescendent(DirectUI::StrToID(L"UserList"));

	return S_OK;
}

HRESULT UserList::AddTileFromData(Microsoft::WRL::ComPtr<IInspectable> tileData)
{
	return S_OK;
}

void UserList::DeselectAllTiles()
{
}

void UserList::HideAllTiles()
{
}

void UserList::FindAndSetKeyFocus()
{
}

void UserList::EnableList()
{
}

HRESULT UserList::ZoomTile(CDUIUserTileElement* userTile)
{
	return S_OK;
}

HRESULT UserList::UnzoomList(CDUIUserTileElement* userTile)
{
	return S_OK;
}

CDUIUserTileElement* UserList::GetZoomedTile()
{
	return nullptr;
}

void UserList::_SetUnzoomedWidth()
{
}

void UserList::_ShowEnumeratedTilesWorker(int visibleIndex)
{
}
