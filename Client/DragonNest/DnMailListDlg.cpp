#include "StdAfx.h"
#include "DnMailListDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void CDnMailListDlg::SMailListRow::Set(const CDnTradeMail::SMailBoxInfo& info, bool bSetCheckBox)
{
	m_nMailID		= info.nMailID;
	m_pStaticTitle->SetTextColor((info.bRead == MailType::Read) ? textcolor::DARK : textcolor::WHITE);

	const wchar_t* pMailTitle = NULL;
	if (info.bNewFlag)
	{
		int iTitleUIStringID = _wtoi( info.strTitle.c_str() );
		pMailTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iTitleUIStringID );

		if (info.mailKind == MailType::AdminMail)
		{
			int iSenderUIStringID = _wtoi( info.strName.c_str() );
			if (iSenderUIStringID == 0)
				m_pStaticName->SetTextWithEllipsis(info.strName.c_str(), L"...");
			else
			{
				const wchar_t* wszSenderUI = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iSenderUIStringID);
				m_pStaticName->SetTextWithEllipsis( wszSenderUI, L"...");
			}
		}
		else
			m_pStaticName->SetTextWithEllipsis(info.strName.c_str(), L"...");
	}
	else
	{
		pMailTitle = info.strTitle.c_str();
		m_pStaticName->SetTextWithEllipsis(info.strName.c_str(), L"...");
	}

#ifdef PRE_ADD_CADGE_CASH
	if( info.mailKind == MailType::Kind::NormalMail )
	{
		m_pBase->Show( true );
	}
	else if( info.mailKind == MailType::Kind::Cadge )
	{
		m_pBase->Show( false );
		m_CashBase->Show( true );
		pMailTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4921 );
	}
	else
	{
		m_pNoticeBase->Show( true );
	}
#else // PRE_ADD_CADGE_CASH
	m_pBase->Show(info.mailKind == MailType::Kind::NormalMail);
	m_pNoticeBase->Show(info.mailKind != MailType::Kind::NormalMail);
#endif // PRE_ADD_CADGE_CASH

	m_pStaticTitle->SetTextWithEllipsis(pMailTitle, L"...");

	m_pStaticName->SetTextColor(textcolor::FONT_GREEN);
	std::wstring remain;
	remain = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1916), info.nRemainDay);
	m_pStaticTime->SetTextWithEllipsis(remain.c_str(), L"...");
	m_pCheckBox->Enable(true);
	m_pCheckBox->SetChecked(bSetCheckBox);
}

CDnMailListDlg::CDnMailListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
{
	m_ButtonWrite		= NULL;
	//m_ButtonRead		= NULL;
	m_ButtonDel			= NULL;
	m_ButtonAttachGet	= NULL;

	m_pButtonPagePrev	= NULL;
	m_pButtonPageNext	= NULL;

	m_pStaticPage		= NULL;
	m_pButtonCheckAll	= NULL;
	m_pSelectBar		= NULL;

	m_pStaticCheckAll	= NULL;

	m_bNewList			= false;
	m_bCheckAll			= false;
	//m_nCurrentPage		= 1;
	m_nCurSelect		= 0;
	m_nCurWindowPage	= 0;

	m_bLock = false;
}

void CDnMailListDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MailListDlg.ui" ).c_str(), bShow );
}

void CDnMailListDlg::InitialUpdate()
{
	m_ButtonWrite		= GetControl<CEtUIButton>("ID_BUTTON_WRITE");
	//m_ButtonRead		= GetControl<CEtUIButton>("ID_BUTTON_READ");
	m_ButtonDel			= GetControl<CEtUIButton>("ID_BUTTON_DEL");
	m_ButtonAttachGet	= GetControl<CEtUIButton>("ID_BUTTON_GET");

	m_pStaticPage		= GetControl<CEtUIStatic>("ID_STATIC_PAGE");

	m_pButtonPagePrev	= GetControl<CEtUIButton>("ID_PAGE_PREV");
	m_pButtonPageNext	= GetControl<CEtUIButton>("ID_PAGE_NEXT");

	m_pButtonCheckAll	= GetControl<CEtUICheckBox>("ID_CHECKBOX_ALL");
	m_pStaticCheckAll	= GetControl<CEtUIStatic>("ID_CheckAll");
	m_pSelectBar		= GetControl<CEtUIStatic>("ID_COVER");		// todo : delete
	m_pSelectBar->Show(false);

	std::string baseStr, checkBoxStr, titleStr, nameStr, attachStr, quickStr, timeStr;
	std::string noticeBaseStr;
#ifdef PRE_ADD_CADGE_CASH
	std::string strCash;
#endif // PRE_ADD_CADGE_CASH

	SMailListRow row;
	int i = 0;
	for (; i < MAILPAGEMAX; ++i)
	{
		baseStr			= FormatA("ID_MAIL%d", i);
		checkBoxStr		= FormatA("ID_CHECKBOX_MAIL%d", i);
		titleStr		= FormatA("ID_TITLE_MAIL%d", i);
		nameStr			= FormatA("ID_NAME_MAIL%d", i);
		attachStr		= FormatA("ID_CLIP%d", i);
		timeStr			= FormatA("ID_DAYS_MAIL%d", i);
		quickStr		= FormatA("ID_QUICK%d", i);
		noticeBaseStr	= FormatA("ID_MAIL_NOTICE%d", i);
#ifdef PRE_ADD_CADGE_CASH
		strCash			= FormatA("ID_MAIL_CASH%d", i);;
		row.m_CashBase		= GetControl<CEtUIStatic>(strCash.c_str());
#endif // PRE_ADD_CADGE_CASH
		row.m_pBase			= GetControl<CEtUIStatic>(baseStr.c_str());
		row.m_pNoticeBase	= GetControl<CEtUIStatic>(noticeBaseStr.c_str());
		row.m_pCheckBox		= GetControl<CEtUICheckBox>(checkBoxStr.c_str());
		row.m_pStaticTitle	= GetControl<CEtUIStatic>(titleStr.c_str());
		row.m_pStaticName	= GetControl<CEtUIStatic>(nameStr.c_str());
		row.m_pStaticAttach	= GetControl<CEtUIStatic>(attachStr.c_str());
		row.m_pStaticTime	= GetControl<CEtUIStatic>(timeStr.c_str());
		row.m_pStaticQuick	= GetControl<CEtUIStatic>(quickStr.c_str());
		row.Show(false, false, false);

		row.Init();
		m_MailList.push_back(row);
	}

	m_pButtonPagePrev->Enable(false);
	m_pButtonPageNext->Enable(false);
}

void CDnMailListDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	CEtUIDialog::Show(bShow);
}

int CDnMailListDlg::FindInsideItem(float fX, float fY)
{
	for (int i = 0; i < MAILPAGEMAX; i++)
	{
		if (m_MailList[i].IsInsideItem(fX, fY))
		{
			return i;
		}
	}

	return -1;
}

// void CDnMailListDlg::UpdateSelectBar()
// {
// 	if (m_nCurSelect > MAILPAGEMAX)
// 	{
// 		_ASSERT(0);
// 		return;
// 	}
// 
// 	if (m_nCurSelect < 0)
// 	{
// 		//m_pSelectBar->Show(false);
// 		return;
// 	}
// 
// 	SUICoord uiCoordSelectBar, baseCoord;
// 	CEtUIStatic* pBase = m_MailList[0].m_pBase;
// 	if (pBase == NULL || m_pSelectBar == NULL)
// 	{
// 		_ASSERT(0);
// 		return;
// 	}
// 
// 	m_pSelectBar->GetUICoord(uiCoordSelectBar);
// 	pBase->GetUICoord(baseCoord);
// 	uiCoordSelectBar.fY = baseCoord.fY + baseCoord.fHeight * m_nCurSelect;
// 
// 	m_pSelectBar->SetUICoord(uiCoordSelectBar);
// 	m_pSelectBar->Show(true);
// }

void CDnMailListDlg::PrevPage()
{
	int curPage = GetTradeTask().GetTradeMail().GetCurrentPage();
	if (curPage <= 1)
		return;

	int serverPrevPage = curPage - 1;
	GetTradeTask().GetTradeMail().RequestMailBox(serverPrevPage);
}

void CDnMailListDlg::NextPage()
{
	int curPage = GetTradeTask().GetTradeMail().GetCurrentPage();
	int maxPage = GetTradeTask().GetTradeMail().GetMaxPage();

	if (curPage < 0 || curPage >= maxPage)
		return;

	int serverNextPage = curPage + 1;
	GetTradeTask().GetTradeMail().RequestMailBox(serverNextPage);
}

void CDnMailListDlg::UpdatePage()
{
	CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
	int nCount = mailTask.GetTotalMailCount();
	if( nCount == 0 )
	{
		m_pStaticPage->Show(false);
		return;
	}

	std::wstring page;
	page = FormatW(_T("%d/%d"), mailTask.GetCurrentPage(), mailTask.GetMaxPage());
	m_pStaticPage->SetText(page.c_str());
	m_pStaticPage->Show(true);

	m_bCheckAll = false;
}

void CDnMailListDlg::UpdatePageButton()
{
	CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
	int nCount = mailTask.GetTotalMailCount();
	if( nCount == 0 )
	{
		m_pButtonPageNext->Enable(false);
		m_pButtonPagePrev->Enable(false);
		return;
	}

	int currentPage = mailTask.GetCurrentPage();
	int maxPage		= mailTask.GetMaxPage();

	if( currentPage == maxPage )
	{
		m_pButtonPageNext->Enable(false);
	}
	else
	{
		m_pButtonPageNext->Enable(true);
	}

	if( currentPage == 1 )
	{
		m_pButtonPagePrev->Enable(false);
	}
	else
	{
		m_pButtonPagePrev->Enable(true);
	}
}

void CDnMailListDlg::UpdateList()
{
	CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
	std::vector<CDnTradeMail::SMailBoxInfo>& mailBoxInfoList = mailTask.GetMailBoxInfoList();

	if (mailBoxInfoList.size() > MAILPAGEMAX)
	{
		_ASSERT(0);
		return;
	}

	std::wstring temp;
	int	nIndex = 0;
	for (int i=0; i<(int)m_MailList.size(); i++, nIndex++)
	{
		if (nIndex >= (int)mailBoxInfoList.size())
			break;

		const CDnTradeMail::SMailBoxInfo& info = mailBoxInfoList[nIndex];
		SMailListRow& raw = m_MailList[i];
		raw.Set(info, false);
		raw.Show(true, info.bItem, false);//info.deliveryType == MAILTYPE_QUICK); - temp by kalliste
	}

	m_pButtonCheckAll->Enable(mailTask.GetTotalMailCount() > 0);
}

void CDnMailListDlg::SetAllCheckBox(bool bSet, bool bCheckBox)
{
	int i = 0;
	for (; i < MAILPAGEMAX; ++i)
	{
		SMailListRow& row = m_MailList[i];
		if (row.IsEmpty() == false)
			row.m_pCheckBox->SetChecked(bSet, false);
		row.m_pCheckBox->Enable(!row.IsEmpty());
	}

	if (bCheckBox)
		m_pButtonCheckAll->SetChecked(bSet);

	RefreshButtons();
}

void CDnMailListDlg::Clear()
{
	std::vector<SMailListRow>::iterator iter = m_MailList.begin();
	for (; iter != m_MailList.end(); ++iter)
	{
		SMailListRow& info = *iter;
		info.Clear();
	}

	SetAllCheckBox(false, true);
	//m_pSelectBar->Show(false);
}

void CDnMailListDlg::RefreshList(const std::vector<CDnTradeMail::SMailBoxInfo>& mailList)
{
	Clear();

	UpdateList();
	UpdatePage();
	UpdatePageButton();
	//UpdateSelectBar();
}

void CDnMailListDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process(fElapsedTime);
}

void CDnMailListDlg::RefreshButtons()
{
	if (m_MailList.size() != MAILPAGEMAX)
		return;

	int i, count, attachCount;
	i = count = attachCount = 0;

	for (; i < MAILPAGEMAX; ++i)
	{
		if (m_MailList[i].m_pCheckBox->IsChecked())
		{
			count++;
			if (m_MailList[i].m_pStaticAttach->IsShow())
				attachCount++;
		}
	}

	if (count > 1)
	{
		//m_ButtonRead->Enable(false);
		m_ButtonDel->Enable(true);
		if (attachCount > 0)
			m_ButtonAttachGet->Enable(true);
	}
	else if (count == 1)
	{
		//m_ButtonRead->Enable(true);
		m_ButtonDel->Enable(true);
		if (attachCount > 0)
			m_ButtonAttachGet->Enable(true);
	}
	else if (count <= 0)
	{
		//m_ButtonRead->Enable(false);
		m_ButtonDel->Enable(false);
		m_ButtonAttachGet->Enable(false);
	}
}

void CDnMailListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if (IsLocked())
		return;

	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_PAGE_PREV"))
		{
			//m_pSelectBar->Show(false);
			PrevPage();
			return;
		}

		if (IsCmdControl("ID_PAGE_NEXT"))
		{
			//m_pSelectBar->Show(false);
			NextPage();
			return;
		}

		if (IsCmdControl("ID_BUTTON_READ"))
		{
			if ((m_nCurSelect >= 0) && (m_nCurSelect < MAILPAGEMAX))
				GetTradeTask().GetTradeMail().RequestMailRead(m_MailList[m_nCurSelect].m_nMailID);
			return;
		}

		if (IsCmdControl("ID_BUTTON_WRITE"))
		{
			GetInterface().OpenMailDialog(true, _T(""), _T(""));
			return;
		}

		if (IsCmdControl("ID_BUTTON_DEL"))
		{
			std::wstring str;
			str = FormatW(_T("%s\n%s"), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1960), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1961));
			GetInterface().MessageBox(str.c_str(), MB_YESNO, MESSAGEBOX_DEL_CONFIRM, this);
			return;
		}

		if (IsCmdControl("ID_BUTTON_GET"))
		{
			if (IsAnyChecked() == false)
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1964), MB_OK);	// UISTRING : 체크된 아이템이 없어 찾을 수 없습니다.
				return;
			}

			CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
			int idList[MAILPAGEMAX];
			ZeroMemory(idList, sizeof(idList));

			int i = 0;
			for (; i < MAILPAGEMAX; ++i)
			{
				SMailListRow& row = m_MailList[i];
				if (row.IsEmpty() == false && row.m_pCheckBox->IsChecked())
					idList[i] = row.m_nMailID;
			}
			mailTask.RequestMailAttachAll(idList);
		}
	}
	else if (nCommand == EVENT_CHECKBOX_CHANGED)
	{
		if (IsCmdControl("ID_CHECKBOX_ALL"))
		{
			bool bChecked = m_pButtonCheckAll->IsChecked();
			SetAllCheckBox(bChecked);
			return;
		}
		else if (strstr(pControl->GetControlName(), "ID_CHECKBOX_MAIL"))
		{
			RefreshButtons();

			if (m_pButtonCheckAll->IsChecked())
			{
				int i = 0;
				for (; i < MAILPAGEMAX; ++i)
				{
					if (m_MailList[i].IsEmpty() == false && m_MailList[i].m_pCheckBox->IsChecked() == false)
					{
						m_pButtonCheckAll->SetChecked(false, false);
						break;
					}
				}
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnMailListDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IsShow() || IsLocked())
	{
		return false;
	}

	switch(uMsg)
	{
// 	case WM_LBUTTONDOWN:
// 		{
// 			POINT MousePoint;
// 			float fMouseX, fMouseY;
// 
// 			MousePoint.x = short(LOWORD(lParam));
// 			MousePoint.y = short(HIWORD(lParam));
// 			PointToFloat(MousePoint, fMouseX, fMouseY);
// 
// 			int nFindSelect = FindInsideItem(fMouseX, fMouseY);
// 			CDnTradeMail& tm = GetTradeTask().GetTradeMail();
// 			int listSize = tm.GetTotalMailCount();
// 
// 			if ((nFindSelect != -1) && (listSize > 0))
// 			{
// 				if (listSize > nFindSelect)
// 				{
// 					//m_nCurSelect = nFindSelect;
// 					//UpdateSelectBar();
// 					if (nFindSelect > MAILPAGEMAX)
// 					{
// 						_ASSERT(0);
// 						return true;
// 					}
// 
// 					SMailListRow& curRow = m_MailList[nFindSelect];
// 					curRow.m_pCheckBox->SetChecked(!curRow.m_pCheckBox->IsChecked());
// 					return true;
// 				}
// 				else
// 				{
// 					//m_pSelectBar->Show(false);
// 					m_nCurSelect = -1;
// 				}
// 			}
// 		}
// 		break;
	case WM_LBUTTONUP:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short(LOWORD(lParam));
			MousePoint.y = short(HIWORD(lParam));
			PointToFloat(MousePoint, fMouseX, fMouseY);

			int nFindSelect = FindInsideItem(fMouseX, fMouseY);
			CDnTradeMail& tm = GetTradeTask().GetTradeMail();
			int listSize = tm.GetTotalMailCount();

			if ((nFindSelect != -1) && (listSize > 0))
			{
				m_nCurSelect = nFindSelect;
				//UpdateSelectBar();
				GetTradeTask().GetTradeMail().RequestMailRead(m_MailList[m_nCurSelect].m_nMailID);
				SetAllCheckBox(false, true);
				break;
			}

			if (m_pStaticCheckAll->IsInside(fMouseX, fMouseY))
			{
				bool bChecked = m_pButtonCheckAll->IsChecked();
				SetAllCheckBox(!bChecked, true);
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMailListDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (nID == MESSAGEBOX_DEL_CONFIRM)
		{
			if (IsCmdControl("ID_YES"))
			{
				int deleteIds[MAILPAGEMAX];
				ZeroMemory(deleteIds, sizeof(deleteIds));

				int i = 0;
				int count = 0;
				for (; i < MAILPAGEMAX; ++i)
				{
					const SMailListRow& row = m_MailList[i];
					if (row.m_pCheckBox->IsChecked())
					{
						deleteIds[i] = row.m_nMailID;
						count++;
					}
				}

				GetTradeTask().GetTradeMail().RequestMailDelete(deleteIds, count);
			}
			else if (IsCmdControl("ID_NO"))
			{
				SetAllCheckBox(false, true);
			}
		}
	}
}

bool CDnMailListDlg::IsAnyChecked() const
{
	int i = 0;
	for (; i < MAILPAGEMAX; ++i)
	{
		if (m_MailList[i].m_pCheckBox->IsChecked())
			return true;
	}

	return false;
}

void CDnMailListDlg::EnableButtons(bool bEnable)
{
	if (bEnable == false)
	{
		m_ButtonWrite->Enable(false);
		m_ButtonDel->Enable(false);
		m_ButtonAttachGet->Enable(false);

		m_pButtonPagePrev->Enable(false);
		m_pButtonPageNext->Enable(false);
		m_pButtonCheckAll->Enable(false);
		for (int i = 0; i < MAILPAGEMAX; i++)
		{
			if (m_MailList[i].IsEmpty() == false && m_MailList[i].m_pCheckBox)
				m_MailList[i].m_pCheckBox->Enable(false);
		}
	}
	else
	{
		m_ButtonWrite->Enable(true);

		for (int i = 0; i < MAILPAGEMAX; i++)
		{
			if (m_MailList[i].IsEmpty() == false && m_MailList[i].m_pCheckBox)
				m_MailList[i].m_pCheckBox->Enable(true);
		}
		m_pButtonCheckAll->Enable(true);
		UpdatePageButton();
		RefreshButtons();
	}
}