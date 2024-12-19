#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"
#include "DnParts.h"
#ifdef PRE_MOD_WARRIOR_RIGHTHAND
#include "DnLoadingTask.h"
#include "VillageClientSession.h"
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

template < class T >

class TDnPlayerWarrior : public TDnPlayerCommon<T>
{
public:
	TDnPlayerWarrior( int nClassID, bool bProcess = true )
		: TDnPlayerCommon<T>(nClassID, bProcess)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		m_bUseThreadOnCombineParts = false;
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
	}

	virtual ~TDnPlayerWarrior()
	{
		for( int i=0; i<4; i++ ) SAFE_RELEASE_SPTR( m_hKick[i] );
	}

protected:
	int m_nKickActionIndex[4];
	DnWeaponHandle m_hKick[4];

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
	bool m_bUseThreadOnCombineParts;
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

public:
	// Actor Message
	virtual bool Initialize()
	{
		TDnPlayerCommon<T>::Initialize();

		int j = 0;
		for (; j < 4; j++)
			SAFE_RELEASE_SPTR(m_hKick[j]);

		m_hKick[0] = CDnWeapon::CreateWeapon( 5, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( 6, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( 7, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( 5, 0 );
		for( int i=0; i<4; i++ ) {
			m_hKick[i]->CreateObject();
			m_hKick[i]->SetLinkActor( GetMySmartPtr() );
		}

		m_nKickActionIndex[0] = GetElementIndex( "Attack_SideKick" );
		m_nKickActionIndex[1] = GetElementIndex( "Attack_SoccerKick" );
		m_nKickActionIndex[2] = GetElementIndex( "Attack_Down" );
		m_nKickActionIndex[3] = GetElementIndex( "Jump_Kick" );

		return true;
	}

	virtual DnWeaponHandle GetWeapon( int nEquipIndex = 0, bool bActionMatchWeapon = true )
	{
		if( IsGhost() || !bActionMatchWeapon ) return m_hWeapon[nEquipIndex];
		if( nEquipIndex == 0 ) {
			for( int i=0; i<4; i++ ) {
				if( m_nActionIndex == m_nKickActionIndex[i] ) return m_hKick[i];
			}
		}

		return TDnPlayerCommon<T>::GetWeapon( nEquipIndex, bActionMatchWeapon );
	}
	virtual DnWeaponHandle GetActiveWeapon( int nEquipIndex )
	{
		if( IsGhost() ) return m_hWeapon[nEquipIndex];
		if( nEquipIndex == 0 ) {
			for( int i=0; i<4; i++ ) {
				if( m_nActionIndex == m_nKickActionIndex[i] ) return m_hKick[i];
			}
		}
		return TDnPlayerCommon<T>::GetActiveWeapon( nEquipIndex );
	}
	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false )
	{
		TDnPlayerCommon<T>::AttachWeapon( hWeapon, nEquipIndex, bDelete );

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		if( nEquipIndex == 1 ) {
			if( m_bUseThreadOnCombineParts ) {
				DWORD dwUniqueID = GetUniqueID();
				CDnLoadingTask::GetInstance().InsertLoadObject(
					OnLoadChangeParts,
					OnLoadChangePartsUniqueID,
					&CVillageClientSession::OnCheckLoadingPacket,
					NULL,
					(void*)this, (void*)&dwUniqueID, sizeof(DWORD), 0 );
			}
			else {
				OnLoadChangeParts( this, NULL, NULL, 0 );
			}

			// 이렇게 Attach, Detach에서 둘다 CombineParts를 하면, 장착할때 두번 호출되는 구조가 되버린다.
			// 이건 어차피 한벌옷 할때 여러번 나눠서 파츠해제가 오기때문에 계속 컴바인 되는 것과 같은 이유니,
			// 아예 컴바인단에서 변경점이 있는지 판단하고 컴바인하는 것으로 해결하겠다.
		}
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
	}
	virtual void DetachWeapon( int nEquipIndex = 0 )
	{
		TDnPlayerCommon<T>::DetachWeapon( nEquipIndex );

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		if( nEquipIndex == 1 ) {
			if( m_bUseThreadOnCombineParts ) {
				DWORD dwUniqueID = GetUniqueID();
				CDnLoadingTask::GetInstance().InsertLoadObject(
					OnLoadChangeParts,
					OnLoadChangePartsUniqueID,
					&CVillageClientSession::OnCheckLoadingPacket,
					NULL,
					(void*)this, (void*)&dwUniqueID, sizeof(DWORD), 0 );
			}
			else {
				OnLoadChangeParts( this, NULL, NULL, 0 );
			}
		}
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
	}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
	virtual void AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false, bool bRefreshWeaponOrder = true )
	{
		TDnPlayerCommon<T>::AttachCashWeapon( hWeapon, nEquipIndex, bDelete, bRefreshWeaponOrder );

		if( nEquipIndex == 1 ) {
			if( m_bUseThreadOnCombineParts ) {
				DWORD dwUniqueID = GetUniqueID();
				CDnLoadingTask::GetInstance().InsertLoadObject(
					OnLoadChangeParts,
					OnLoadChangePartsUniqueID,
					&CVillageClientSession::OnCheckLoadingPacket,
					NULL,
					(void*)this, (void*)&dwUniqueID, sizeof(DWORD), 0 );
			}
			else {
				OnLoadChangeParts( this, NULL, NULL, 0 );
			}
		}
	}

	virtual void DetachCashWeapon( int nEquipIndex = 0 )
	{
		TDnPlayerCommon<T>::DetachCashWeapon( nEquipIndex );

		if( nEquipIndex == 1 ) {
			if( m_bUseThreadOnCombineParts ) {
				DWORD dwUniqueID = GetUniqueID();
				CDnLoadingTask::GetInstance().InsertLoadObject(
					OnLoadChangeParts,
					OnLoadChangePartsUniqueID,
					&CVillageClientSession::OnCheckLoadingPacket,
					NULL,
					(void*)this, (void*)&dwUniqueID, sizeof(DWORD), 0 );
			}
			else {
				OnLoadChangeParts( this, NULL, NULL, 0 );
			}
		}
	}

	// Loading 관련 함수들
	static int __stdcall OnLoadChangePartsUniqueID( void *pParam, int nSize ) { return *((DWORD *)pParam); }

	// ItemTask의 InsertChangePartsThread를 호출하는 구조로 가면 월드안에 들어간 캐릭터 자신밖에 안되니,
	// 여기서 쓰레드 호출을 해야한다. 그래야 로그인단에 있는 워리어에게도 적용시킬 수 있다.
	static bool __stdcall OnLoadChangeParts( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime )
	{
		if( nSize > 0 && CDnLoadingTask::GetInstance().CheckSameLoadObject( pParam, nSize ) ) return true;

		TDnPlayerCommon<T>* pTemp = (TDnPlayerCommon<T>*)pThis;
		MAPartsBody *pBody = dynamic_cast<MAPartsBody*>(pTemp);
		if( pBody ) pBody->CombineParts();
		return true;
	}
	// IgnoreCombine이 true일때는 여기서
	virtual void OnCombineParts()
	{
		MAPartsBody *pBody = dynamic_cast<MAPartsBody*>(this);
		if( pBody ) {
			DnPartsHandle hHand = pBody->GetParts( CDnParts::Hand );
			DnPartsHandle hCashHand = pBody->GetCashParts( CDnParts::CashHand );
			DnPartsHandle hParts = pBody->GetCashParts( CDnParts::CashBody );	// 한벌옷일때를 대비해서 Body부터 체크.
			if( hHand && hHand->GetObjectHandle() && hHand->IsShow() ) hParts = hHand;
			if( hCashHand && hCashHand->GetObjectHandle() && hCashHand->IsShow() ) hParts = hCashHand;
			if( hParts && hParts->GetObjectHandle() ) {
				EtAniObjectHandle hHandle = hParts->GetObjectHandle();
				for( int i=0; i<hHandle->GetSubMeshCount(); i++ ) {
					CEtSubMesh *pSubMesh = hParts->GetObjectHandle()->GetSubMesh(i);
					if( pSubMesh && strstr( pSubMesh->GetSubMeshName(), "Glove_R" ) ) {
						bool bShowSubMesh = true;
						if( m_hWeapon[1] && m_hWeapon[1]->IsShow() ) bShowSubMesh = false;
						if( m_hCashWeapon[1] && m_hCashWeapon[1]->IsShow() ) bShowSubMesh = false;
						hHandle->ShowSubmesh( i, bShowSubMesh );
						break;
					}
				}
			}
		}
	}

	// IgnoreCombine이 false일때는 여기서
	virtual void OnCombineParts( EtSkinHandle hSkin, CEtSkinInstance **ppSkinInstance )
	{
		DnWeaponHandle hWeapon = GetActiveWeapon(1);
		if( hSkin && hSkin->GetMeshHandle() && hSkin->GetMeshHandle()->GetSubMeshCount() > 0 && hWeapon && hWeapon->IsShow() ) 
		{
			for( int i=0; i<hSkin->GetMeshHandle()->GetSubMeshCount(); i++ ) 
			{
				CEtSubMesh *pSubMesh = hSkin->GetMeshHandle()->GetSubMesh(i);
				// && !strstr( pSubMesh->GetSubMeshName(), "Basic" ) 이렇게 추가검사하면, 맨손+건틀일때 맨손 보이고, 장갑끼면 베이직파츠 안보인다.
				// 원하는건 이게 아니다. 제대로 할거면, 장갑끼고 있을때 건틀끼면 베이직파츠로 컴바인해야하는데,
				// 이럴거면, 장갑 착용중에도 베이직 파츠를 로드해서 들고있어야한다. 우선 구조상 패스.
				if( pSubMesh && strstr( pSubMesh->GetSubMeshName(), "Glove_R" )	) {
					(*ppSkinInstance)->GetSkinRenderInfo( i )->bShowSubmesh = false;
					break;
				}
			}
		}
		else {
			MAPartsBody::OnCombineParts( hSkin, ppSkinInstance );
		}
	}

	virtual void SetProcess( bool bFlag )
	{
		if( IsProcess() == bFlag ) return;
		TDnPlayerCommon<T>::SetProcess( bFlag );

		// 기본적으로 일반적인 장착을 제외하고, 스테이지에 입장하면서 초기화되거나 렌더아바타에서 캐릭터를 만들때는 쓰레드를 사용하지 않는다.
		m_bUseThreadOnCombineParts = bFlag;
	}

	virtual void ShowWeapon( int nEquipIndex, bool bShow )
	{
		bool bRefreshCombine = false;
		if( nEquipIndex == 1 && m_hWeapon[nEquipIndex] && m_hWeapon[nEquipIndex]->IsShow() != bShow ) bRefreshCombine = true;

		// 난입시에 워리어몸통이 안보이는 버그이슈.
		// 확인해보니 CDnPlayerActor::Show가 난입중에 연속으로 false, true가 호출되는데,
		// 이때 ShowWeapon도 호출되기때문에 Combine처리가 들어가게 된다.
		// 문제는 Show(false)호출하면서 Combine이 진행중일때 Show(true)가 호출되면서 CDnPlayerActor::ShowParts가 호출되고,
		// 이 안에서 m_MergedObject가 Identity(컴바인중이라)상태라 MergedObject를 Show(true)하지 못한채 다음번 Combine이 진행되게 된다.
		// 그래서 Show(false)상태가 유지되는 것.
		//
		// 첫번째 해결책은 CDnPlayerActor::ShowParts의 MergedObject Show설정부분에 s_PartsLock락거는건데, 락은 싫어해서 패스.
		// 두번째 해결책은 Combine에서 이전 MergedObject의 IsShow를 받아오지 않고 그냥 액터의 IsShow를 받아오는 건데, 입증된 코드가 아니라서 넘기고,
		// 세번째 해결책은 ProcessHidePlayerActor처리에서 MergedObject의 IsShow를 검사해 주는건데 너무 강제성이 있어서 넘긴다.
		// 그래서 마지막 해결책으로 ShowWeapon(false)될때 현재 캐릭터가 하이드상태라면 컴바인할필요 없으니 아예 컴바인을 건너뛰는걸로 하도록 한다.
		if( bShow == false && IsShow() == false ) bRefreshCombine = false;
		if( IsSwapSingleSkin() ) bRefreshCombine = false;

		TDnPlayerCommon<T>::ShowWeapon( nEquipIndex, bShow );
		if( bRefreshCombine ) {
			CombineParts();
		}
	}

	virtual void ShowCashWeapon( int nEquipIndex, bool bShow )
	{
		bool bRefreshCombine = false;
		if( nEquipIndex == 1 && m_hCashWeapon[nEquipIndex] && m_hCashWeapon[nEquipIndex]->IsShow() != bShow ) bRefreshCombine = true;

		if( bShow == false && IsShow() == false ) bRefreshCombine = false;
		if( IsSwapSingleSkin() ) bRefreshCombine = false;

		TDnPlayerCommon<T>::ShowCashWeapon( nEquipIndex, bShow );
		if( bRefreshCombine ) {
			CombineParts();
		}
	}
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
};