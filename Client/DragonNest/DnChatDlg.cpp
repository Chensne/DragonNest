#include "StdAfx.h"
#include "DnChatDlg.h"
#include "DnChatOptDlg.h"
#include "DebugCmdProc.h"
#include "DnChatTabDlg.h"
#include "DnPartyTask.h"
#include "DnChatTabDlg.h"
#include "TaskManager.h"
#include "DnNameLinkMng.h"
#include "DnInterface.h"
#include "DnCustomControlCommon.h"
#include "DnTooltipDlg.h"
#include "DnContextMenuDlg.h"
#include "DnPVPLobbyVillageTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatDlg::CDnChatDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pChatTextBox(NULL)
	, m_nChatType(CHAT_NO)
	, m_pButtonDummy(NULL)
	//, m_pStaticBase(NULL)
	, m_bIsShowScrollBar(false)
	, m_pChatOptDlg( NULL )
{
}

CDnChatDlg::~CDnChatDlg(void)
{
	SAFE_DELETE( m_pChatOptDlg );
}

void CDnChatDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChatDialog.ui" ).c_str(), bShow );
}

void CDnChatDlg::InitialUpdate()
{
	m_pChatOptDlg = new CDnChatOptDlg( UI_TYPE_CHILD, this );
	m_pChatOptDlg->Initialize( false );

	m_pChatTextBox = GetControl<CEtUITextBox>("ID_TEXTBOX_CHAT");

	m_pButtonDummy = GetControl<CEtUIButton>("ID_BUTTON_DUMMY");
	if( m_pButtonDummy )
		m_pButtonDummy->Show(false);
}

bool CDnChatDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hWnd;

	if( !m_bShow )
	{
		return false;
	}

	if( uMsg == WM_MOUSEWHEEL )
	{
		// 휠메세지는 채팅탭이 보일때만 작동하도록 한다.
		// 다른 메세지(클릭 등)은 ChatDlg의 BUTTON_DUMMY 눌러 활성화시키는 것때문에 채팅탭이 안보여도 처리해야한다.
		CDnChatTabDlg *pChatDialog = (CDnChatTabDlg*)GetParentDialog();
		if( !pChatDialog->IsShow() )
		{
			return false;
		}

		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if( IsMouseInDlg() )
		{
			if( m_pChatTextBox->HandleMouse( uMsg, fMouseX, fMouseY, wParam, lParam ) )
			{
				return true;
			}
		}
	}

#if 0
	if (uMsg == WM_MOUSEMOVE)
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if (IsMouseInDlg() && m_pChatTextBox)
		{
			m_pChatTextBox->SetMouseInLine(fMouseX, fMouseY);
			m_pChatTextBox->SelectWord(fMouseX, fMouseY);

			CWord selectedWord = m_pChatTextBox->GetSelectedWordData();
			if (selectedWord.m_strWordWithTag.empty() == false)
			{
				EtInterface::GetNameLinkMgr().TranslateText(std::wstring(), selectedWord.m_strWordWithTag.c_str(), this);
				m_bShowMouseOverToolTip = true;
			}
			else if (m_bShowMouseOverToolTip)
			{
				CDnTooltipDlg* pToolTipDlg = GetInterface().GetNameLinkToolTipDlg();
				if (pToolTipDlg && pToolTipDlg->IsShow())
					pToolTipDlg->Show(false);
			}
		}
	}
#endif

	bool bCloseContextMenu = false;
	if ( uMsg == WM_LBUTTONDOWN )
	{
		if (IsMouseInDlg())
			SetRenderPriority(this, true);
		bCloseContextMenu = true;
	}

	if (uMsg == WM_KEYDOWN)
		bCloseContextMenu = true;

	if (bCloseContextMenu)
	{
		CDnContextMenuDlg* pDlg = GetInterface().GetContextMenuDlg(CDnInterface::CONTEXTM_CHAT);
		if (pDlg && pDlg->IsShow())
			pDlg->Show(false);
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnChatDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_DUMMY" ) )
		{
			CDnChatTabDlg *pChatDialog = (CDnChatTabDlg*)GetParentDialog();
			pChatDialog->ShowEx(true);
			return;
		}
	}
	else if( nCommand == EVENT_TEXTBOX_SELECTION )
	{
		if( IsCmdControl("ID_TEXTBOX_CHAT" ) )
		{
			CDnChatTabDlg *pChatDialog = (CDnChatTabDlg*)GetParentDialog();
			if( !pChatDialog->IsShow() )
				return;

			CWord selectedWord = m_pChatTextBox->GetSelectedWordData();
			if (selectedWord.m_strWordWithTag.empty() == false)
				EtInterface::GetNameLinkMgr().TranslateText(std::wstring(), selectedWord.m_strWordWithTag.c_str(), this);
		}
	}
}

void CDnChatDlg::InitOption( int nChatType )
{
	m_nChatType = nChatType;
	m_pChatOptDlg->InitOption( nChatType );
}

#ifdef PRE_ADD_MULTILANGUAGE
void CDnChatDlg::ChangeItemNameLinkByLanguage( LPCWSTR szMessage, std::wstring& wstrMessage )
{
	std::wstring wstrMessageTemp;
	wstrMessageTemp = szMessage;
	std::vector<CWord> vecWorld;
	SWORD_PROPERTY basicProperty;
	EtInterface::GetNameLinkMgr().TranslateText( vecWorld, basicProperty, wstrMessageTemp.c_str() );
	for( int i=0; i<static_cast<int>( vecWorld.size() ); i++ )
	{
		if( vecWorld[i].m_strWordWithTag.length() > 0 )
		{
			int nCurrentPos = 0;
			while( nCurrentPos < static_cast<int>( vecWorld[i].m_strWordWithTag.size() ) )
			{
				CEtUINameLinkMgr::eNameLinkTagType type = EtInterface::GetNameLinkMgr().IsTagType( vecWorld[i].m_strWordWithTag, nCurrentPos );
				if( type != CEtUINameLinkMgr::eNLTAG_NONE )
				{
					if( type == CEtUINameLinkMgr::eNLTAG_ITEMINFO )
					{
						std::wstring wstrItemName;
						std::wstring wstrTag = vecWorld[i].m_strWordWithTag.substr( nCurrentPos + 2, std::wstring::npos );
						std::wstring::size_type tagContentsSize = wstrTag.find_first_of( EtInterface::GetNameLinkMgr().GetNameLinkTagCloser() );
						if( tagContentsSize != std::wstring::npos )
						{
							std::wstring wstrArgString = wstrTag.substr( 0, tagContentsSize );
							CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
							if( pMgr )
							{
								CDnItem* pItem = pMgr->MakeItem( wstrArgString );
								if( pItem )
									wstrItemName = pItem->GetName();
							}
						}

						std::wstring::size_type strIndex = wstrMessageTemp.find_first_of( EtInterface::GetNameLinkMgr().GetNameLinkTagCloser() );
						if( strIndex != std::wstring::npos )
						{
							std::wstring wstrTemp = wstrMessageTemp.substr( 0, strIndex + 2 );
							wstrMessage += wstrTemp + wstrItemName + L"]";

							wstrTemp = wstrMessageTemp.substr( strIndex + 2, std::wstring::npos );
							
							strIndex = wstrTemp.find_first_of( ']' );
							if( strIndex != std::wstring::npos )
							{
								wstrTemp.erase( 0, strIndex + 1 );
								wstrMessageTemp = wstrTemp;
							}
						}
					}

					int offset = 0;
					std::wstring parsee = vecWorld[i].m_strWordWithTag.substr( nCurrentPos + 1, std::wstring::npos );
					EtInterface::GetNameLinkMgr().GetTagOffsetByParseText( parsee, offset );
					nCurrentPos += offset + 1;
				}
				else
				{
					break;
				}
			}
		}
	}

	wstrMessage += wstrMessageTemp;
}
#endif // PRE_ADD_MULTILANGUAGE

void CDnChatDlg::AddChat( LPCWSTR wszName, LPCWSTR szMessage, int nType, DWORD dwFormat, bool isAppend, bool hasColor, DWORD colorValue, DWORD bgColorValue, bool bIgnoreCarrageReturn, bool bOnResetDevice )
{
#ifdef PRE_ADD_MULTILANGUAGE
	std::wstring wstrMessage;
	ChangeItemNameLinkByLanguage( szMessage, wstrMessage );
#endif // PRE_ADD_MULTILANGUAGE

	if( !bOnResetDevice )
	{
		SAddChatData Data;
		if( wszName != NULL ) Data.wszName = wszName;
#ifdef PRE_ADD_MULTILANGUAGE
		if( szMessage != NULL ) Data.szMessage = wstrMessage;
#else // PRE_ADD_MULTILANGUAGE
		if( szMessage != NULL ) Data.szMessage = szMessage;
#endif // PRE_ADD_MULTILANGUAGE
		Data.nChatType = nType;
		Data.dwFormat = dwFormat;
		Data.isAppend = isAppend;
		Data.hasColor = hasColor;
		Data.colorValue = colorValue;
		Data.bgColorValue = bgColorValue;
		Data.bIgnoreCarrageReturn = bIgnoreCarrageReturn;
		m_listAddChatData.push_back(Data);
		if( (int)m_listAddChatData.size() > 150 )
			m_listAddChatData.pop_front();
	}

	DWORD dwTextColor(textcolor::WHITE);

	switch( nType )
	{
	case CHAT_NORMAL:	dwTextColor = chatcolor::NORMAL;	break;
	case CHAT_PARTY:	dwTextColor = chatcolor::PARTY;		break;
	case CHAT_GUILD:	dwTextColor = chatcolor::GUILD;		break;
	case CHAT_PRIVATE:	dwTextColor = chatcolor::PRIVATE;	break;
	case CHAT_CHANNEL:	dwTextColor = chatcolor::CHANNEL;	bgColorValue = chatcolor::CHANNELBACK;	break;
	case CHAT_WORLD:	dwTextColor = chatcolor::WORLD;		bgColorValue = chatcolor::WORLDBACK;	break;
	case CHAT_SYSTEM:	dwTextColor = textcolor::GOLD;		break;
	case CHAT_RAIDNOTICE: dwTextColor = chatcolor::RAIDNOTICE; break;
	case CHAT_GUILDWAR : dwTextColor = chatcolor::RAIDNOTICE; break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHAT_PRIVATE_CHANNEL:	dwTextColor = chatcolor::PRIVATECHANNEL;	break;
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_WORLD_MSG_RED
	case CHAT_WORLD_RED:	dwTextColor = chatcolor::WORLD_RED;		bgColorValue = chatcolor::WORLD_REDBACK;	break;
#endif // PRE_ADD_WORLD_MSG_RED
	case CHAT_NOTICE:
		{
			dwTextColor = textcolor::HOTPINK;
#ifdef PRE_ADD_MULTILANGUAGE
			m_pChatTextBox->SetText( wstrMessage.c_str(), dwTextColor );
#else // PRE_ADD_MULTILANGUAGE
			m_pChatTextBox->SetText( szMessage, dwTextColor );
#endif // PRE_ADD_MULTILANGUAGE
		}
		return;
	}

	if(GetInterface().GetChatTabDialog()->IsCustomizeChatMode() && GetInterface().GetChatTabDialog()->GetCustomizeChatMode() == nType)
	{
		dwTextColor = GetInterface().GetChatTabDialog()->GetCustomizeChatColor();
	}

	if( wszName != NULL && wszName[0] != '\0')
	{
		SLineData sLineData;
		sLineData.m_strData = wszName;
		m_pChatTextBox->SetLineData( sLineData );
	}

	std::vector<std::wstring> sentences;
#ifdef PRE_ADD_MULTILANGUAGE
	if( bIgnoreCarrageReturn )
		sentences.push_back( wstrMessage.c_str() );
	else
		TokenizeW( wstrMessage.c_str(), sentences, L"\n" );
#else // PRE_ADD_MULTILANGUAGE
	if (bIgnoreCarrageReturn)
	{
		sentences.push_back(szMessage);
	}
	else
	{
		TokenizeW(szMessage, sentences, L"\n");
	}
#endif // PRE_ADD_MULTILANGUAGE

	std::vector<std::wstring>::const_iterator iter = sentences.begin();
	for (; iter != sentences.end(); ++iter)
	{
		const std::wstring& message = *iter;

		if (isAppend)
		{
			if (hasColor) {
				dwTextColor = colorValue;
			}
			m_pChatTextBox->AppendText(message.c_str(), dwTextColor, dwFormat, true, bgColorValue);
		}
		else
		{
			bool bAutoScroll = false;

			if( m_pChatTextBox->IsLastPage() || m_pChatTextBox->IsEmpty() )
				bAutoScroll = true;

			// 스크롤이 올려져있더라도 자신이 말한 내용이라면 자동스크롤을 적용시킨다.
			if( !bAutoScroll && wszName ) {
				bool bMyChat = false;
				if( CDnActor::s_hLocalActor ) {
					if( __wcsicmp_l( wszName, CDnActor::s_hLocalActor->GetName() ) == 0 )
						bMyChat = true;
				}
				else {
					if( CTaskManager::GetInstance().GetTask( "PVPLobbyTask" ) ) {
						// PVP로비에서는 캐릭터 처리가 필요없어서 s_hLocalActor가 없을 수 있다.
						// 이땐 파티태스크 로컬 데이터에서 얻어와서 비교한다.
						if( CDnPartyTask::IsActive() && __wcsicmp_l( wszName, GetPartyTask().GetLocalData()->wszCharacterName ) == 0 )
							bMyChat = true;
					}
				}
				if( bMyChat )
					bAutoScroll = true;
			}
			m_pChatTextBox->SetAutoScroll( bAutoScroll );

			// AddText되는 시스템메세지의 경우 ColorText를 사용.
			// 현재 이렇게 하면 시스템 메세지의 dwFormat을 사용할 수 없게 된다.
			// 정확하게 하려면, #r, #g와 같은 컬러 인자가 있을때만 ColorText를 사용하게 하던지 해야하는데,
			// 어차피 시스템메세지의 경우 항상 UITEXT_NONE을 사용했기때문에 우선은 그냥 이대로 가도록 한다.
// 			if( nType == CHAT_SYSTEM )	// commented by kalliste
// 				m_pChatTextBox->AddColorText( message.c_str(), dwTextColor, bgColorValue );
// 			else
			m_pChatTextBox->AddText( message.c_str(), dwTextColor,  dwFormat, bgColorValue );
		}
	}
}

bool CDnChatDlg::IsShowOptDlg()
{ 
	return m_pChatOptDlg->IsShow();
}

void CDnChatDlg::ShowOptDlg( bool bShow )			
{ 
	ShowChildDialog( m_pChatOptDlg, bShow );
}

void CDnChatDlg::UpdateDlgCoord( float fX, float fY, float fWidth, float fHeight )
{
	CEtUIDialog::UpdateDlgCoord( fX, fY, fWidth, fHeight );

	m_pChatOptDlg->UpdateDlgCoord( 0, -fHeight, 0, 0 );

	m_pChatTextBox->UpdateUICoord( 0, 0, 0, fHeight );

	if( m_pButtonDummy )
		m_pButtonDummy->UpdateUICoord( 0, 0, 0, fHeight );
}

void CDnChatDlg::ShowScrollBar( bool bShow )
{
	SUIControlProperty *pProperty = m_pChatTextBox->GetProperty();
	pProperty->TextBoxProperty.bVerticalScrollBar = bShow;

	if( m_pButtonDummy )
		m_pButtonDummy->Show(!bShow);

	m_bIsShowScrollBar = bShow;
}

bool CDnChatDlg::IsTextBoxEmpty()
{
	return m_pChatTextBox->IsEmpty();
}

void CDnChatDlg::OnChangeResolution()
{
	m_pChatTextBox->ClearText();
	CEtUIDialog::OnChangeResolution();
	for( std::list<SAddChatData>::iterator iter = m_listAddChatData.begin(); iter != m_listAddChatData.end(); ++iter )
	{
		SAddChatData &Data = *iter;
		LPCWSTR wszName = NULL;
		if( (int)Data.wszName.size() ) wszName = Data.wszName.c_str();	// 이름엔 NULL들어가는 처리가 가능하다.
		AddChat( wszName, Data.szMessage.c_str(), Data.nChatType, Data.dwFormat, Data.isAppend, Data.hasColor, Data.colorValue, Data.bgColorValue, Data.bIgnoreCarrageReturn, true );
	}
}

bool CDnChatDlg::OnParseTextItemInfo(const std::wstring& argString)
{
	CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
	if (pMgr)
	{
		CDnItem* pLinkItem = pMgr->MakeItem(argString);
		if (pLinkItem)
		{
			CDnTooltipDlg* pToolTipDlg = GetInterface().GetNameLinkToolTipDlg();
			if (pToolTipDlg)
			{
				ITEM_SLOT_TYPE slotType = ST_ITEM_NONE;
				if (pLinkItem->GetType() == MIInventoryItem::Item)
					slotType = ST_INVENTORY;
				else if (pLinkItem->GetType() == MIInventoryItem::Skill)
					slotType = ST_SKILL;
				pToolTipDlg->ShowTooltip(pLinkItem, pLinkItem->GetType(), slotType, GetScreenWidthRatio() * 0.5f, GetScreenHeightRatio() * 0.83f, true);
			}
		}
	}

	return true;
}

bool CDnChatDlg::OnParseTextUserName(const std::wstring& name)
{
	float mousePointX, mousePointY;
	GetMouseMovePoints(mousePointX, mousePointY);
	mousePointX += GetXCoord();
	mousePointY += GetYCoord();
	ShowChatContextMenuDlg(true, mousePointX, mousePointY, name);

	return true;
}

void CDnChatDlg::ShowChatContextMenuDlg(bool bShow, float fX, float fY, const std::wstring& name)
{
	CDnContextMenuDlg* pDlg = GetInterface().GetContextMenuDlg(CDnInterface::CONTEXTM_CHAT);
	CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );

	if (pDlg)
	{
		if (CDnActor::s_hLocalActor)
		{
			if (name.compare(CDnActor::s_hLocalActor->GetName()) == 0)
				return;
		}
		else if( pPVPLobbyTask )
		{
			if( __wcsicmp_l( name.c_str(), pPVPLobbyTask->GetUserInfo().wszCharacterName ) == 0 ) 
				return;
		}

		CDnChatContextMenuDlg::SContextMenuChatInfo info;
		info.name = name;

		SUICoord Coord = pDlg->GetBackgroundUISize();

		if( fY + Coord.fHeight > 1.f )
			pDlg->SetPosition( fX, 1.f - Coord.fHeight );
		else
			pDlg->SetPosition(fX, fY);

		pDlg->GetDlgCoord( Coord );

		pDlg->SetInfo(&info);
		pDlg->Show(bShow);

		if (bShow)
			SetRenderPriority(pDlg, true);
	}
}