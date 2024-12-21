#include "StdAfx.h"
#include "DnWorldTrapProp.h"
#include "DnTableDB.h"
#include "DnActorClassDefine.h"
#include "DnMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldTrapProp::CDnWorldTrapProp()
: CDnDamageBase( DamageObjectTypeEnum::Prop )
, m_iActivateType( LOOP )
, m_bIdle( false )
{
}

CDnWorldTrapProp::~CDnWorldTrapProp()
{
	
}

bool CDnWorldTrapProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	if( GetData() )
	{
		TrapStruct *pStruct			= (TrapStruct *)GetData();
		int			nMonsterTableID	= pStruct->nMonsterTableID;
		bool		bResult			= false;

//#61146 TrapŸ�� ������ ���ݷ��� ���̵��� ���� �ٸ��� ������ �� �ִ� ����.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( nMonsterTableID );
#endif	

		if( bResult )
		{
			m_iActivateType = pStruct->ActivateType;
		}
	}

	//if( m_pAdditionalPropInfo ) {
	//	TrapInfoStruct *pStruct = (TrapInfoStruct *)m_pAdditionalPropInfo;
	//	int nMonsterTableID = pStruct->nMonsterTableID;

	//	if( nMonsterTableID > 0 ) {
	//		m_hMonster = CreateActor( -1 );
	//		if( !m_hMonster ) return false;
	//		m_hMonster->SetProcess( false );

	//		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );
	//		CDnMonsterActor *pMonster = dynamic_cast<CDnMonsterActor*>(m_hMonster.GetPointer());

	//		pMonster->SetMonsterClassID( nMonsterTableID );
	//		pMonster->SetLevel( pSox->GetFieldFromLablePtr( nMonsterTableID, "_Level" )->GetInteger() );
	//		m_hMonster->Initialize();
	//		
	//		char szWeaponLable[32];
	//		for( int j=0; j<2; j++ ) {
	//			sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );
	//			int nWeapon = pSox->GetFieldFromLablePtr( nMonsterTableID, szWeaponLable )->GetInteger();
	//			if( nWeapon < 1 ) continue;
	//			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand() );
	//			m_hMonster->AttachWeapon( hWeapon, j, true );
	//		}

	//		m_hMonster->RefreshState();
	//	}
	//	SAFE_DELETE( m_pAdditionalPropInfo );
	//}

	return true;
}

bool CDnWorldTrapProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}


bool CDnWorldTrapProp::CreateObject( void )
{
	bool bResult = CDnWorldActProp::CreateObject();

	//if( bResult )
	//{
	//	if( LOOP == m_iActivateType )
	//		GetElement( "Activate" )->szNextActionName.assign( "Activate" );
	//}

	return bResult;
}


void CDnWorldTrapProp::SetActionQueue( const char *szActionName, int nLoopCount , float fBlendFrame , float fStartFrame )
{
	CDnWorldActProp::SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame );
	if( strcmp( szActionName, "Idle" ) == 0 )
		m_bIdle = true;
	else
		m_bIdle = false;
}



// 2010.4.22 �������� �Ź� �׼� ����� ������ CmdAction ��Ŷ�� �����ֹǷ� ���ŵ�.
//// �׼������� ����Ʈ�� idle �� �Ǿ������Ƿ� ���� Ÿ���� ��� ���⼭ �ٲ��ش�.
//// ��, ���� Ÿ���ε� Idle �׼��� Queuing ���� ���� Activate �� �ٲ����� �ʴ´�.
//void CDnWorldTrapProp::OnFinishAction( const char* szPrevAction, const char *szNextAction, LOCAL_TIME time )
//{
//	if( LOOP == m_iActivateType && false == m_bIdle )
//		SetActionQueue( "Activate" );
//}
