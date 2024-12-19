#include "StdAfx.h"
#include "DnFriendGroupDlg.h"
#include "DnFriendTask.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DNCountryUnicodeSet.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnFriendGroupDlg::CDnFriendGroupDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pListBoxGroup(NULL)
	, m_pEditBoxGroup(NULL)
	, m_pButtonAdd(NULL)
	, m_pButtonDel(NULL)
	, m_pButtonSave(NULL)
{
}

CDnFriendGroupDlg::~CDnFriendGroupDlg(void)
{
}

void CDnFriendGroupDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "FriendGroupDlg.ui" ).c_str(), bShow );
}

void CDnFriendGroupDlg::InitialUpdate()
{
	m_pListBoxGroup = GetControl<CEtUIListBox>("ID_LISTBOX_GROUP");
	m_pEditBoxGroup = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_GROUP");
	m_pButtonAdd = GetControl<CEtUIButton>("ID_ADD");
	m_pButtonDel = GetControl<CEtUIButton>("ID_DEL");
	m_pButtonSave = GetControl<CEtUIButton>("ID_SAVE");
	//m_pListBoxGroup->SetSmartAlignRatio(0.5f);
}

void CDnFriendGroupDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RefreshFriendGroup();
		m_pEditBoxGroup->ClearText();
		m_pButtonAdd->Enable(false);
		m_pButtonDel->Enable(false);
		m_pButtonSave->Enable(false);
	}

	CEtUIDialog::Show( bShow );

	if (bShow)
		RequestFocus(m_pEditBoxGroup);
}

void CDnFriendGroupDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process(fElapsedTime);
}

bool CDnFriendGroupDlg::IsDuplicatedName(const wchar_t* inputName)
{
	const CDnFriendTask::FRIENDGROUPINFO_VEC &vecFriendGroupInfo = GetFriendTask().GetFriendGroupInfoList();
	for( int i=0; i<(int)vecFriendGroupInfo.size(); i++ )
	{
		if (!_tcscmp(vecFriendGroupInfo[i].wszGroupName.c_str(), inputName) || !_tcscmp(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1346/*그룹없음*/ ), inputName))
			return true;
	}

	return false;
}

bool CDnFriendGroupDlg::IsFullGroupCount() const
{
	const CDnFriendTask::FRIENDGROUPINFO_VEC &vecFriendGroupInfo = GetFriendTask().GetFriendGroupInfoList();
	if ((int)vecFriendGroupInfo.size() >= FRIEND_GROUP_MAX)
		return true;

	return false;
}

bool CDnFriendGroupDlg::IsSpecialCharacter(std::wstring& strGroupName) const
{
// 	if (DN_INTERFACE::UTIL::CheckSpecialCharacter(strGroupName))
// 		return true;

	DWORD dwCheckType = ALLOW_STRING_DEFAULT;
#if defined (_US)
	dwCheckType = ALLOW_STRING_CHARACTERNAME_ENG;
#endif

	if (g_CountryUnicodeSet.Check(strGroupName.c_str(), dwCheckType) == false)
		return true;

	std::wstring::size_type index;
	index = strGroupName.find_first_of( L"-" );

	if( index != string::npos )
	{
		return true;
	}

	return false;
}

bool CDnFriendGroupDlg::CheckGroupNameValidation(std::wstring& strGroupName)
{
	bool bRet = true;
	if (strGroupName.empty() || strGroupName.size() < _MIN_NAME_SYLLABLE_COUNT)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4301), MB_OK, MESSAGEBOX_FRIEND_GROUP_FILTERWORD, this); // UISTRING : 1자 이상의 한글, 영어, 숫자로 만들 수 있습니다.
		bRet = false;
	}
	else if (IsSpecialCharacter(strGroupName))
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4302), MB_OK, MESSAGEBOX_FRIEND_GROUP_FILTERWORD, this); // UISTRING : 한글, 영어, 숫자로만 만들 수 있습니다.
		bRet = false;
	}
	else if (DN_INTERFACE::UTIL::CheckChat(strGroupName))
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4303 ), MB_OK, MESSAGEBOX_FRIEND_GROUP_FILTERWORD, this); // UISTRING : 금칙어이므로 추가할 수 없습니다.
		bRet = false;
	}
	else if (IsDuplicatedName(strGroupName.c_str()))
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4304 ), MB_OK, MESSAGEBOX_FRIEND_GROUP_DUPNAME, this);	// UISTRING : 이미 존재하는 그룹명입니다. 다른 이름으로 만들어 주세요.
		bRet = false;
	}

	return bRet;
}

bool CDnFriendGroupDlg::DoAdd()
{
	std::wstring strGroupName( m_pEditBoxGroup->GetText() );
	if( !strGroupName.empty() )
	{
		if (IsFullGroupCount())
		{
			GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4313 ), MB_OK, MESSAGEBOX_FRIEND_GROUP_DUPNAME, this);	// UISTRING : 더 이상 그룹을 추가할 수 없습니다
		}
		else
		{
			if (CheckGroupNameValidation(strGroupName))
			{
				GetFriendTask().RequestFriendGroupAdd( strGroupName.c_str() );
			}
			else
			{
				focus::ReleaseControl();
				return false;
			}
		}
		//GetFriendTask().TAddGroup( strGroupName.c_str() );
	}
	else
	{
		// Note : 그룹이름을 입력하라는 메세지 박스를 출력.
		//
		RequestFocus(m_pEditBoxGroup);
		return false;
	}

	return true;
}

void CDnFriendGroupDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_ADD") )
		{
			DoAdd();
			return;
		}

		if( IsCmdControl("ID_SAVE" ) )
		{
			std::wstring strGroupName( m_pEditBoxGroup->GetText() );
			SListBoxItem *pItem = m_pListBoxGroup->GetSelectedItem();

			if( pItem && (strGroupName != pItem->strText) )
			{
				if (CheckGroupNameValidation(strGroupName))
				{
					int nGroupID(0);
					if( m_pListBoxGroup->GetSelectedValue( nGroupID ) )
					{
						GetFriendTask().RequestFriendGroupUpdate( nGroupID, strGroupName.c_str() );
						//GetFriendTask().TRenameGroup( nGroupID, strGroupName.c_str() );
						_wcscpy( pItem->strText, _countof(pItem->strText), strGroupName.c_str(), (int)wcslen(strGroupName.c_str()) );
						SetEditBox();
					}
				}
			}

			return;
		}
		
		if( IsCmdControl("ID_DEL") )
		{
			int nGroupID(0);
			if( m_pListBoxGroup->GetSelectedValue( nGroupID ) )
			{
				GetFriendTask().RequestFriendGroupDel( nGroupID );
				//GetFriendTask().TDeleteGroup( nGroupID );

				int nSelectedIndex = m_pListBoxGroup->GetSelectedIndex();
				m_pListBoxGroup->RemoveItem( nSelectedIndex );

				SetEditBox();
			}

			return;
		}
	}
	else if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		if( IsCmdControl("ID_LISTBOX_GROUP" ) )
		{
			int nGroupID(0);
			if( m_pListBoxGroup->GetSelectedValue( nGroupID ) )
			{
				if( nGroupID )
				{
					m_pButtonDel->Enable(true);
					m_pButtonSave->Enable(true);
				}
				else
				{
					m_pButtonDel->Enable(false);
					m_pButtonSave->Enable(false);
				}
			}

			SetEditBox();
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE || nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( IsCmdControl("ID_IMEEDITBOX_GROUP") )
		{
			std::wstring strName = m_pEditBoxGroup->GetText();
			strName = boost::algorithm::trim_copy(strName);
			m_pButtonAdd->Enable(strName.empty() == false);
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnFriendGroupDlg::OnAddFriendGroup()
{
	RefreshFriendGroup();
	m_pEditBoxGroup->ClearText();
	m_pButtonAdd->Enable(false);
	RequestFocus(m_pEditBoxGroup);
}

void CDnFriendGroupDlg::RefreshFriendGroup()
{
	// Note : Friend Task에서 그룹 정보를 받아서 셋팅한다.
	//
	m_pListBoxGroup->RemoveAllItems();
	
	const CDnFriendTask::FRIENDGROUPINFO_VEC &vecFriendGroupInfo = GetFriendTask().GetFriendGroupInfoList();
	for( int i=0; i<(int)vecFriendGroupInfo.size(); i++ )
	{
		m_pListBoxGroup->AddItem( vecFriendGroupInfo[i].wszGroupName.c_str(), NULL, vecFriendGroupInfo[i].nGroupDBID );
	}
	//m_pListBoxGroup->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1346/*그룹없음*/ ), NULL, 0 );
}

void CDnFriendGroupDlg::SetEditBox()
{
	SListBoxItem *pItem = m_pListBoxGroup->GetSelectedItem();
	if( pItem )
	{
		m_pEditBoxGroup->SetText( pItem->strText );
	}
}

bool CDnFriendGroupDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			return true;
		}
		else if (wParam == VK_RETURN)
		{
			if (m_pButtonAdd->IsEnable())
			{
				DoAdd();
			}
			return true;
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnFriendGroupDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( nID == MESSAGEBOX_FRIEND_GROUP_DUPNAME )
		{
			if( IsCmdControl("ID_OK" ) )
			{
				RequestFocus(m_pEditBoxGroup);
				m_pEditBoxGroup->SetText(m_pEditBoxGroup->GetText(), true);
			}
		}
		else if (nID == MESSAGEBOX_FRIEND_GROUP_FILTERWORD)
		{
			if (IsCmdControl("ID_OK"))
			{
				RequestFocus(m_pEditBoxGroup);
				m_pEditBoxGroup->ClearText();
				m_pButtonAdd->Enable(false);
			}
		}
	}
}
