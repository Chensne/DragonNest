#include "StdAfx.h"
#include "MAActorProp.h"
#include "DnMonsterActor.h"
#include "DnWorldActProp.h"
#include "DnActorClassDefine.h"
#include "DnTableDB.h"
#include "DnNPCActor.h"



/*
#ifndef _GAMESERVER
DWORD MAActorProp::s_dwActorPropUniqueID = 0;
vector<DnActorHandle> MAActorProp::s_vlhActorProp;
#else
#include "DNGameDataManager.h"
STATIC_DECL_INIT( MAActorProp, DWORD, s_dwActorPropUniqueID ) = { 0, };
#endif
*/
#ifdef _GAMESERVER
#include "DNGameDataManager.h"
#include "DnGameRoom.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MAActorProp::MAActorProp(void)
{

}

MAActorProp::~MAActorProp(void)
{
	/*
#ifndef _GAMESERVER
	vector<DnActorHandle>::iterator iter = find( s_vlhActorProp.begin(), s_vlhActorProp.end(), m_hMonster );
	if( s_vlhActorProp.end() != iter )
		s_vlhActorProp.erase( iter );
#endif
	*/

	SAFE_RELEASE_SPTR( m_hMonster );
}

#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
bool MAActorProp::InitializeMonsterActorProp( int nMonsterTableID , BOOL bUseMonsterWeightTable /* = false*/ )
#else
bool MAActorProp::InitializeMonsterActorProp( int nMonsterTableID )
#endif
{
	bool bResult = false;

	CDnWorldActProp* pWorldActProp = dynamic_cast<CDnWorldActProp*>(this);

	if( nMonsterTableID > 0 )
	{
#ifdef _GAMESERVER
		m_hMonster = CreateActor( pWorldActProp->GetRoom(), -1 );
#else
		m_hMonster = CreateActor( -1, false, false, false );
#endif
		if( !m_hMonster ) return false;
		m_hMonster->SetProcess( false );
		CDnMonsterActor *pMonster = dynamic_cast<CDnMonsterActor*>(m_hMonster.GetPointer());
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );
		if( !pSox->IsExistItem( nMonsterTableID ) ) 
		{
			SAFE_RELEASE_SPTR( m_hMonster );
			return false;
		}

//#ifdef _GAMESERVER
		pMonster->SetPropHandle( pWorldActProp->GetMySmartPtr() );
//#endif
		
		pMonster->SetMonsterClassID( nMonsterTableID );
		pMonster->SetLevel( pSox->GetFieldFromLablePtr( nMonsterTableID, "_Level" )->GetInteger() );
#ifdef _GAMESERVER
		pMonster->InitializeRoom( pWorldActProp->GetGameRoom() );

#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		if(bUseMonsterWeightTable)
		{
			// 이곳에서 설정해주지 않으면, Prop(Trap)의 AI는 Easy로 고정되어있다.
			pMonster->SetAIDifficult(pWorldActProp->GetGameRoom()->m_StageDifficulty);
		}
#endif // PER_ADD_PROP_TRAP_DIFFICULT_SET

#else
		pMonster->InitializeSession( pWorldActProp->GetSession() );
#endif 
		m_hMonster->Initialize();
		m_hMonster->SetPosition( pWorldActProp->GetMatEx()->m_vPosition );
		m_hMonster->RefreshState();
		m_hMonster->SetHP( m_hMonster->GetMaxHP() );
		m_hMonster->SetSP( m_hMonster->GetMaxSP() );
		

		// Note: 현재 액터들의 유니크 아이디 체계를 프랍액터도 쓰는 것으로 변경되어서..
		// ScanActor 로도 얻어지고 팀 셋팅을 따로 하지 않아 몬스터들이 공격하는 경우가 있어서
		// Hittable 플래그를 꺼놉니다..
		m_hMonster->SetHittable( false );
		m_hMonster->RemoveOctreeNode();
		m_hMonster->SetActorType( CDnActorState::PropActor );

#ifdef _GAMESERVER
		CMultiRoom *pRoom = m_hMonster->GetRoom();
		m_hMonster->SetUniqueID( STATIC_INSTANCE_(CDnActor::s_dwUniqueCount)++ );
#else
		// NOTE: 유니크 아이디가 없으면 SC_ONDAMAGE 패킷을 받고도 프랍의 액터를 찾을 수가 없어서 hiiter가 NULL 이 되어버려서 맞는 액션이 나오지 않는다.
		m_hMonster->SetUniqueID( CDnActor::s_dwUniqueCount++ );
//		s_vlhActorProp.push_back( m_hMonster );
#endif

		pSox = GetDNTable( CDnTableDB::TMONSTER );
		char szWeaponLable[32];
		for( int j=0; j<2; j++ )
		{
			sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );
			int nWeapon = pSox->GetFieldFromLablePtr( nMonsterTableID, szWeaponLable )->GetInteger();
			if( nWeapon < 1 ) continue;

#ifdef _GAMESERVER
			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( pWorldActProp->GetRoom(), nWeapon, _rand(pWorldActProp->GetRoom()) );
#else
			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand(/*pWorldActProp*/) );
#endif
			m_hMonster->AttachWeapon( hWeapon, j, true );
			
			bResult = true;
		}
		// hp 가 0 이면 죽은 것으로 처리 되어 데미지 안 먹히는 경우가 있으므로 1로 셋팅
		if( 0 >= m_hMonster->GetHP() )
			m_hMonster->SetHP( 1 );
	}
	
//	_ASSERT( bResult && "프랍용 몬스터 생성 실패" );

	return bResult;
}

bool MAActorProp::InitializeNpcActorProp( int nNpcTableID )
{
	CDnWorldActProp* pWorldActProp = dynamic_cast<CDnWorldActProp*>(this);

#ifdef _GAMESERVER
	TNpcData* pNpcData = NULL;
	pNpcData = g_pDataManager->GetNpcData(nNpcTableID);
	if (pNpcData)
		m_hMonster = CreateNpcActor( pWorldActProp->GetRoom(), nNpcTableID, pNpcData, pWorldActProp->GetMatEx()->m_vPosition, pWorldActProp->GetRotation()->y );
	if( m_hMonster ) m_hMonster->InitializeRoom( pWorldActProp->GetGameRoom() );
#else
	m_hMonster = CreateNpcActor( -1, nNpcTableID, pWorldActProp->GetMatEx()->m_vPosition, pWorldActProp->GetRotation()->y );
#endif
	if( m_hMonster ) {
		CDnNPCActor *pNpc = dynamic_cast<CDnNPCActor *>(m_hMonster.GetPointer());
		if( pNpc ) pNpc->SetPropHandle( pWorldActProp->GetMySmartPtr() );
//		m_hMonster->SetProcess( false );
#ifndef _GAMESERVER
		SAFE_RELEASE_SPTR( m_hMonster->GetAniObjectHandle() );
#endif
		m_hMonster->Show( true );
	}
	return true;
}

bool MAActorProp::CopyActorStateFromThis( DnActorHandle hActor )
{
	// 스탯만 동일하게 맞춰준다.
	// 테스트가 필요함.
	m_hMonster->CopyStateFromThis( /*static_cast<CDnActorState*>(hActor.GetPointer())*/hActor );

	return true;
}