#include "StdAfx.h"
#include "DnCharCreatePlayerInfo.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnTableDB.h"
#include "DnJobIconStatic.h"



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifdef PRE_MOD_SELECT_CHAR


CDnCharCreatePlayerInfo::CDnCharCreatePlayerInfo( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pMovieCtrl( NULL )
{
#ifdef _ADD_NEWLOGINUI
	memset(m_pJobHistory, NULL, sizeof(m_pJobHistory));
	memset(m_nJobHistoryArr, NULL, sizeof(m_nJobHistoryArr));
#endif
}

CDnCharCreatePlayerInfo::~CDnCharCreatePlayerInfo(void)
{

}

void CDnCharCreatePlayerInfo::Initialize( bool bShow )
{
#ifdef _ADD_NEWLOGINUI
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreateClassInfoDlg.ui" ).c_str(), bShow );
#else
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("CharCreate_PlayerInfo.ui").c_str(), bShow);
#endif
}

void CDnCharCreatePlayerInfo::InitialUpdate()
{
	m_pMovieCtrl = GetControl<CDnMovieControl>( "ID_MOVIE_SKILL" );
	

#ifdef _ADD_NEWLOGINUI
	m_pClassName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	SUICoord Coord = m_pClassName->GetUICoord();//rlkt_newui
	m_pClassName->SetPosition(Coord.fX, Coord.fY - 0.05f);

	for (int i = 0; i < 7; i++){
		m_pJobHistory[i] = GetControl<CDnJobIconStatic>(FormatA("ID_STATIC_CLASS%d",i).c_str());
	}
#endif
}

void CDnCharCreatePlayerInfo::Show( bool bShow )
{
	if( bShow == m_bShow )
		return;

	if( m_pMovieCtrl )
	{
		m_pMovieCtrl->Show( bShow );

		if( bShow == false )
			m_pMovieCtrl->Stop();
	}

	CDnCustomDlg::Show(bShow);
}

void CDnCharCreatePlayerInfo::PlayMovie( const std::string& fileName )
{
	if( m_pMovieCtrl == NULL )
		return;

	if( fileName.empty() == false )
		m_pMovieCtrl->Play( fileName.c_str(), true );
	else
		m_pMovieCtrl->Stop();
}

void CDnCharCreatePlayerInfo::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
	MoveToTail( m_pMovieCtrl );
}

void CDnCharCreatePlayerInfo::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask ) return;

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl( "ID_BT_OK" ) ) 
		{
			pLoginTask->StartCharacterChangeParts();
		}
//		else if( IsCmdControl( "ID_BT_CANCEL" ) ) 
//		{
//			pLoginTask->DeSelectCharacter();
//		}
	}
	
	if (EVENT_RADIOBUTTON_CHANGED == nCommand)
	{
		if (strstr(pControl->GetControlName(), "ID_RBT"))
		{
			int nSelectIndex = static_cast<CEtUIRadioButton*>(pControl)->GetTabID();
			this->SetJobName(nSelectIndex);
		}
	}
}

void CDnCharCreatePlayerInfo::SetJobID( int nJobClassID, int nJobExplanationID, bool bDarkClass )
{
#ifdef _ADD_NEWLOGINUI
	//clear icons!@
	for (int i = 0; i < 7; i++)
	{
		m_pJobHistory[i]->SetIconID(-1);
		m_nJobHistoryArr[i] = 0;
	}
	if (m_pClassName->IsShow()) m_pClassName->Show(false);
#endif

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );	
	if( pSox == NULL )
		return;


	if( pSox->IsExistItem( nJobClassID + 1 ) ) 
	{
		std::wstring m_wszJobName;
#ifndef _ADD_NEWLOGINUI
		if (nJobClassID == 9) { // rlkt_dark machina fix
			m_wszJobName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nJobClassID, "_JobName")->GetInteger());
		}
		else{
			m_wszJobName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nJobClassID + 1, "_JobName")->GetInteger());
		}
#else
		m_wszJobName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nJobClassID + 1, "_JobName")->GetInteger());
#endif
		CEtUIStatic* pStaticClassName = GetControl<CEtUIStatic>( "ID_TEXT_CLASSNAME" );
		pStaticClassName->SetText( m_wszJobName.c_str() );
		pStaticClassName->Show( true );
	}

	CDnJobIconStatic* pStaticJobIcon = GetControl<CDnJobIconStatic>( "ID_STATIC_CLASSICON" );
	/*if (nJobClassID == 9) { // rlkt_dark machina fix
		pStaticJobIcon->SetIconID(nJobClassID); 
	}else{
		pStaticJobIcon->SetIconID(nJobClassID + 1);
	}*/
	pStaticJobIcon->SetIconID(nJobClassID + 1);
	pStaticJobIcon->Show( true );

	CEtUIStatic* pStaticClassExplanation = GetControl<CEtUIStatic>( "ID_TEXT_CLASSSUMMARY" );
	pStaticClassExplanation->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nJobExplanationID ) );
	pStaticClassExplanation->Show( true );
	//

#ifdef _ADD_NEWLOGINUI
	DNTableFileFormat* pSoxDefaultCreate = GetDNTable(CDnTableDB::TDEFAULTCREATE);
	if (pSoxDefaultCreate == NULL)
		return;
	
	if (bDarkClass)
	{
		for (int i = 0; i < pSoxDefaultCreate->GetItemCount(); i++)
		{
			int RowID = pSoxDefaultCreate->GetFieldFromLablePtr(i, "_JobCode1")->GetInteger();
			if (RowID == nJobClassID)
				nJobClassID = i - 1;
		}
	}

	DNTableCell JobTree1, JobTree2, JobTree3;
	if (pSoxDefaultCreate->GetFieldFromLable(nJobClassID + 1, "_JobTree1", JobTree1) &&
		pSoxDefaultCreate->GetFieldFromLable(nJobClassID + 1, "_JobTree2", JobTree2) &&
		pSoxDefaultCreate->GetFieldFromLable(nJobClassID + 1, "_JobTree3", JobTree3))
	{
		m_pJobHistory[0]->SetIconID(atoi(JobTree1.GetString()));
		m_nJobHistoryArr[0] = atoi(JobTree1.GetString());

		//2nd class
		if (strstr(JobTree2.GetString(), ";"))
		{
			std::vector<string> vlTokens2;
			TokenizeA(JobTree2.GetString(), vlTokens2, ";");

			if (vlTokens2.size() > 0)
			{
				m_pJobHistory[1]->SetIconID(atoi(vlTokens2[0].c_str()));
				m_nJobHistoryArr[1] = atoi(vlTokens2[0].c_str());
			}
			if (vlTokens2.size() > 1)
			{
				m_pJobHistory[4]->SetIconID(atoi(vlTokens2[1].c_str()));
				m_nJobHistoryArr[4] = atoi(vlTokens2[1].c_str());
			}
		}
		else {
			m_pJobHistory[1]->SetIconID(atoi(JobTree2.GetString()));
			m_nJobHistoryArr[1] = atoi(JobTree2.GetString());
		}
		//3rd class
		if (strstr(JobTree3.GetString(), ";"))
		{
			std::vector<string> vlTokens3;
			TokenizeA(JobTree3.GetString(), vlTokens3, ";");
			if (vlTokens3.size() > 0)
			{
				m_pJobHistory[2]->SetIconID(atoi(vlTokens3[0].c_str()));
				m_nJobHistoryArr[2] = atoi(vlTokens3[0].c_str());
			}
			if (vlTokens3.size() > 1)
			{
				m_pJobHistory[3]->SetIconID(atoi(vlTokens3[1].c_str()));
				m_nJobHistoryArr[3] = atoi(vlTokens3[1].c_str());
			}
			if (vlTokens3.size() > 3)
			{
				m_pJobHistory[5]->SetIconID(atoi(vlTokens3[2].c_str()));
				m_pJobHistory[6]->SetIconID(atoi(vlTokens3[3].c_str()));
				m_nJobHistoryArr[5] = atoi(vlTokens3[2].c_str());
				m_nJobHistoryArr[6] = atoi(vlTokens3[3].c_str());
			}
		}
		else {
			m_pJobHistory[2]->SetIconID(atoi(JobTree3.GetString()));
			m_nJobHistoryArr[2] = atoi(JobTree3.GetString());
		}
	}

#endif
#ifndef _ADD_NEWLOGINUI
	CEtUIStatic* pStaticClassAbility[DARK_JOBMAX];
	pStaticClassAbility[0] = GetControl<CEtUIStatic>( "ID_STATIC_WARRIOR" );
	pStaticClassAbility[1] = GetControl<CEtUIStatic>( "ID_STATIC_ARCHER" );
	pStaticClassAbility[2] = GetControl<CEtUIStatic>( "ID_STATIC_SOSERESS" );
	pStaticClassAbility[3] = GetControl<CEtUIStatic>( "ID_STATIC_CLERIC" );
	pStaticClassAbility[4] = GetControl<CEtUIStatic>( "ID_STATIC_ACADEMIC" );
	pStaticClassAbility[5] = GetControl<CEtUIStatic>( "ID_STATIC_KALI" );
#if defined( PRE_ADD_ASSASSIN )
	pStaticClassAbility[6] = GetControl<CEtUIStatic>( "ID_STATIC_ASSASSIN" );
#endif
#if defined( PRE_ADD_LENCEA )
	pStaticClassAbility[7] = GetControl<CEtUIStatic>( "ID_STATIC_LENCEA" );
#endif

	//8 = DARK AVENGER!@
	pStaticClassAbility[8] = GetControl<CEtUIStatic>("ID_STATIC_DARKAV");

#if defined( PRE_ADD_MACHINA )
	pStaticClassAbility[9] = GetControl<CEtUIStatic>("ID_STATIC_MACHINA");
#endif

	//10 = Silver HUNTER!
	pStaticClassAbility[10] = GetControl<CEtUIStatic>("ID_STATIC_SILVERH");

	for (int i = 0; i<DARK_JOBMAX; i++)
		pStaticClassAbility[i]->Show( false );

	pStaticClassAbility[nJobClassID]->Show( true );
#endif
}

#endif // PRE_MOD_SELECT_CHAR

void CDnCharCreatePlayerInfo::SetJobName(int SelectIndex)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TJOB);
	if (pSox == NULL)
		return;

	int nJobName = pSox->GetFieldFromLablePtr(m_nJobHistoryArr[SelectIndex], "_JobName")->GetInteger();
	if (nJobName > 0)
	{
		m_pClassName->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nJobName));
		m_pClassName->Show(true);
	}
}