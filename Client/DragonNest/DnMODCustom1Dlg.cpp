#include "StdAfx.h"
#include "DnMODCustom1Dlg.h"
#include "DnTrigger.h"
#include "DnGameTask.h"

CDnMODCustom1Dlg::CDnMODCustom1Dlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnMODDlgBase( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
}

CDnMODCustom1Dlg::~CDnMODCustom1Dlg()
{
	SAFE_DELETE_VEC( m_pVecStaticCounter );
}

void CDnMODCustom1Dlg::InitialUpdate()
{
	char szStr[64];
	for( int i=0; ; i++ ) {
		bool bExist;
		CDnDamageCount *pStatic;

		sprintf_s( szStr, "ID_COUNTER%d",  i+1 );
		pStatic = GetControl<CDnDamageCount>( szStr, &bExist );

		if( !bExist ) break;
		pStatic->Init();
		pStatic->SetFontSize( 1.f );
		pStatic->SetRightAlign( true );
		m_pVecStaticCounter.push_back( pStatic );
	}
}

void CDnMODCustom1Dlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( m_szUIFileName ).c_str(), bShow );
}

void CDnMODCustom1Dlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CDnMODDlgBase::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMODCustom1Dlg::Show( bool bShow )
{
	CDnMODDlgBase::Show( bShow );
}

void CDnMODCustom1Dlg::Process( float fElapsedTime )
{
	for( DWORD i=0; i<m_pVecStaticCounter.size(); i++ ) {
		LinkValueStruct *pStruct = GetLinkValue(i);
		if( pStruct ) {
			lua_tinker::table Table = lua_tinker::get<lua_tinker::table>( pStruct->pTrigger->GetLuaState(), "g_value" );
			if( Table.m_obj->validate() ) {
				int nValue = Table.get<int>( pStruct->nDefineValueIndex );
				m_pVecStaticCounter[i]->SetValue( nValue );
				m_pVecStaticCounter[i]->Show( true );
			}
		}
		else m_pVecStaticCounter[i]->Show( false );
	}

	CDnMODDlgBase::Process( fElapsedTime );
}


void CDnMODCustom1Dlg::Render( float fElapsedTime )
{
	CDnMODDlgBase::Render( fElapsedTime );
}