#include "stdafx.h"
#include "DnDummyInfoListCtrl.h"
#include "ResourceID.h"
#include "DnDummyClientFrame.h"


BEGIN_EVENT_TABLE(DnDummyInfoListCtrl, wxListCtrl)
EVT_LIST_ITEM_FOCUSED( ResID::USER_LIST_CTRL, DnDummyInfoListCtrl::OnFocusedItem )

END_EVENT_TABLE()



DnDummyInfoListCtrl*	g_pDummyInfoListCtrl = NULL;

DnDummyInfoListCtrl::DnDummyInfoListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxListCtrl(parent, id, pos, size, wxLC_REPORT|wxSUNKEN_BORDER |  LVS_EX_GRIDLINES ),	m_attr(*wxBLUE, *wxLIGHT_GREY, wxNullFont)
{

	ResetAll();
	

	g_pDummyInfoListCtrl = this;

}



DnDummyInfoListCtrl::~DnDummyInfoListCtrl()
{
	g_pDummyInfoListCtrl = NULL;

}

void DnDummyInfoListCtrl::ResetAll()
{

	ClearAll();
	wxListItem itemCol;
	for ( int i = 0 ; i < _countof(gs_Column) ; i++ )
	{
		itemCol.SetText(gs_Column[i]);
		itemCol.SetAlign(wxLIST_FORMAT_LEFT);
		InsertColumn(i, itemCol);

		wxString str;
		str = gs_Column[i];
		//SetColumnWidth( i, int(str.size() * 30) );
	}

	SetColumnWidth( 0, 50 );
	SetColumnWidth( 1, 60 );
	SetColumnWidth( 2, 80 );
	SetColumnWidth( 3, 80 );
	SetColumnWidth( 4, 100 );
	SetColumnWidth( 5, 100 );
	SetColumnWidth( 6, 180 );
	SetColumnWidth( 7, 60 );
	SetColumnWidth( 8, 60 );
	SetColumnWidth( 9, 60 );
	SetColumnWidth( 10, 60 );
	


}

void DnDummyInfoListCtrl::InsertDummyInfo(int nIdx, DnDummyClient::Property& property)
{

		wxString str;
		str = wxString::Format( wxT("%d"), nIdx);


		long n = InsertItem(nIdx, str);

		UpdateDummyInfo(n, property);

}


void DnDummyInfoListCtrl::UpdateDummyInfo(int nIdx, DnDummyClient::Property& property)
{
	

	wxString str;
	str = wxString::Format( wxT("%d"), nIdx);


	std::wstring strTime = GetTimeString(property.ConnectTime);

	wxColour clr;
	if ( property.nConnectionState != StateInfo::_CONNECTED )
	{
		clr.Set(gs_State[property.nConnectionState].color);
	}
	else
	{
		if ( property.nConnectionState == StateInfo::_CONNECTED && property.nPlayState == PlayStateInfo::PS_NONE )
			clr.Set(gs_PlayState[property.nConnectionState].color);
		else
			clr.Set(gs_PlayState[property.nPlayState].color);
	}

	wxString strResponseTime = wxString::Format( _T("%d"), property.nResponseTime);
	wxString strResponseAvrTime = wxString::Format( _T("%d"), property.nResponseAvrTime);
	wxString strOnDamage = wxString::Format( _T("%d"), property.nDamageCnt);
	wxString strHit = wxString::Format( _T("%d"), property.nHitCnt);



	SetItemBackgroundColour(nIdx, clr);
	SetItem( nIdx, 1, gs_State[property.nConnectionState].szState.c_str() );
	SetItem( nIdx, 2, property.szUserID.c_str() );
	SetItem( nIdx, 3, property.szCharacter.c_str() );
	SetItem( nIdx, 4, gs_PlayState[property.nPlayState].szState.c_str() );
	SetItem( nIdx, 5, gs_CurrentServerInfo[property.nCurPosition].szState.c_str() );
	SetItem( nIdx, 6, strTime.c_str() );
	SetItem( nIdx, 7, strResponseTime.c_str() );
	SetItem( nIdx, 8, strResponseAvrTime.c_str() );
	SetItem( nIdx, 9, strOnDamage.c_str() );
	SetItem( nIdx, 10, strHit.c_str() );

	if ( property.nConnectionState == StateInfo::_DISCONNECTED )
	{
		clr.Set(gs_State[property.nConnectionState].color);

	}

	
}


void DnDummyInfoListCtrl::GetSelectedIndex(OUT std::vector<int>& out)
{
	out.clear();
	int item = -1;

	for ( ;; )
	{
		item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);    // use wxLIST_STATE_DONTCARE for all items
			if ( item == -1 )
				break;

		out.push_back(item);
	}
}

void 
DnDummyInfoListCtrl::OnFocusedItem(wxListEvent& event)
{
	

	int nIdx = event.m_itemIndex;

	DnDummyClient* pClient = g_DummyClientMgr.GetDummyClient(nIdx);
	
	if ( !pClient )
		return;

	std::vector<std::wstring>& logArray = pClient->GetUserLog();
	ClearLog();

	for ( int i = 0 ; i < (int)logArray.size() ; i++ )
	{
		Log(logArray[i].c_str());
		Log(_T("\n"));
	}
	

}