
#include "StdAfx.h"
#include "DnCustomTooltipDlg.h"
#include "DnTableDB.h"
#include "TimeSet.h"
#include "DnActor.h"
#include "DnInterfaceString.h"
#include "SyncTimer.h"

CDnCustomTooltipDlg::CDnCustomTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	m_pTextBox = NULL;
}

CDnCustomTooltipDlg::~CDnCustomTooltipDlg(void)
{
}

void CDnCustomTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnCustomTooltipDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
}

bool CDnCustomTooltipDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bShow == false)
		return false;

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX = 0;
			float fMouseY = 0;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			if (m_pTextBox && m_pTextBox->IsInside(fMouseX, fMouseY))
				Show(false);
		}
	}

	return bRet;
}

#ifdef PRE_ADD_WEEKLYEVENT
void CDnCustomTooltipDlg::SetWeeklyEventText()
{
	const int WEEKLY_EVENT_MAX = 10;
	const int ArrEventString[WEEKLY_EVENT_MAX] = { 17, 19, 20, 7750, 7751, 7752, 7753, 3201, 7755, 7756 };

	m_pTextBox->ClearText();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEEKLYEVENT );
	if( pSox )
	{
		const __time64_t tCurTime = CSyncTimer::GetInstance().GetCurTime(); 
		DBTIMESTAMP DbTime;  
		CTimeSet::ConvertTimeT64ToDbTimeStamp_LC( tCurTime, &DbTime );
		CTimeSet cTime;
		cTime.Set(DbTime);

		const int nTimeSet_Week = cTime.GetDayOfWeek();
		const int nTiemSet_Hour = cTime.GetHour();

		int nResultStartTime = 0;
		int nResultEndTime = 0;

		bool bNotice = false;
		bool bFound = false;

		for( int i=0; i<pSox->GetItemCount(); ++i ) // START VALIDATE
		{
			int nTableID = pSox->GetItemID( i );
			int nDayOfWeek = pSox->GetFieldFromLablePtr( nTableID , "_DayOfWeek" )->GetInteger();
			if( nDayOfWeek == nTimeSet_Week )
			{
				int nStartTime = pSox->GetFieldFromLablePtr( nTableID , "_StartTime" )->GetInteger();
				int nEndTime = pSox->GetFieldFromLablePtr( nTableID , "_EndTime" )->GetInteger(); 

				if( nStartTime <= nTiemSet_Hour && nTiemSet_Hour <= nEndTime )
				{
					if( !bFound )
					{
						nResultStartTime = nStartTime;
						nResultEndTime = nEndTime;
						m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7744  ) , textcolor::YELLOW );
						m_pTextBox->AddColorText( L"\n" );
					}

					std::wstring wstRaceName, wstEventResult;

					int nRaceType = pSox->GetFieldFromLablePtr( nTableID , "_RaceType" )->GetInteger();
					int nClassType = pSox->GetFieldFromLablePtr( nTableID , "_ClassType" )->GetInteger();
					int nEventType = pSox->GetFieldFromLablePtr( nTableID , "_EventType" )->GetInteger();
					int nValue = pSox->GetFieldFromLablePtr( nTableID , "_Value" )->GetInteger();

					switch( nRaceType )
					{
					case 1:
						if( nClassType >= 0 )
							wstRaceName = DN_INTERFACE::STRING::GetClassString( nClassType );
						else
							wstRaceName =  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1375  );
						break;
					case 2:
						wstRaceName =  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000032258  );
						break;
					}

					int nStringIndex = nEventType - 1;

					if( nStringIndex >= 0 && nStringIndex < WEEKLY_EVENT_MAX )
					{
						switch( nEventType )
						{
						case 1:
						case 2:
						case 3:
							{
								if( nValue > 0 )
								{
									wstEventResult = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7747 ) ,
										wstRaceName.c_str() , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, ArrEventString[nStringIndex]  ) , nValue );
									m_pTextBox->AddColorText( wstEventResult.c_str() , textcolor::GREENYELLOW ); 
								}
								else
								{
									wstEventResult = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7748 ) ,
										wstRaceName.c_str() , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, ArrEventString[nStringIndex]  ) , nValue );
									m_pTextBox->AddColorText( wstEventResult.c_str() , textcolor::ORANGERED ); 
								}
							}
							break;
						default:
							{
								wstEventResult = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7749 ) ,
									 GetEtUIXML().GetUIString( CEtUIXML::idCategory1, ArrEventString[nStringIndex]  ) , nValue );
								m_pTextBox->AddColorText( wstEventResult.c_str() , textcolor::TOMATO ); 
							}
							break;
						}
					}

					bFound = true;
				}
			}
		}

		if( bFound )
		{
			m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7745   ) , textcolor::WHITE );
			m_pTextBox->AddColorText( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7746  ) , nResultStartTime , nResultEndTime ).c_str() , textcolor::LIGHTSKYBLUE );
		}
	}
}
#endif


#ifdef PRE_ADD_ACTIVEMISSION
void CDnCustomTooltipDlg::SetActiveMissionText( std::wstring & str )
{
	m_pTextBox->ClearText();

	// #80010 - 미션텍스트길이만큼 가로길이만 늘어나는 문제가 있어서
	// 15자이상이면 개행처리 하도록 하였음.
	int nn = 15;
	if( (int)str.size() > nn )
	{		
		std::wstring strTemp( str );		
		while( (int)strTemp.size() > nn )
		{
			m_pTextBox->AppendText( strTemp.substr( 0, nn ).c_str() );
			m_pTextBox->DoAddText( L"", L"\n");

			strTemp = strTemp.substr( nn, strTemp.size() );
		}
		m_pTextBox->AppendText( strTemp.c_str() );
		
	}

	else
	{
		m_pTextBox->AddText( str.c_str() );
	}

}
#endif // PRE_ADD_ACTIVEMISSION