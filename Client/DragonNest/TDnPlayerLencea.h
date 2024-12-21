#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"
#include "DnParts.h"
#ifdef PRE_MOD_WARRIOR_RIGHTHAND
#include "DnLoadingTask.h"
#include "VillageClientSession.h"
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

template < class T >

class TDnPlayerLencea : public TDnPlayerCommon<T>
{
public:
	TDnPlayerLencea( int nClassID, bool bProcess = true )
		: TDnPlayerCommon<T>(nClassID, bProcess)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		m_bUseThreadOnCombineParts = false;
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
	}

	virtual ~TDnPlayerLencea()
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

		m_hKick[0] = CDnWeapon::CreateWeapon( 5901, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( 5902, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( 5903, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( 5901, 0 );
		for( int i=0; i<4; i++ ) {
			m_hKick[i]->CreateObject();
			m_hKick[i]->SetLinkActor( GetMySmartPtr() );
		}

		m_nKickActionIndex[0] = GetElementIndex( "Skill_PushKick" );
		m_nKickActionIndex[1] = GetElementIndex( "Skill_PutKick" );
		m_nKickActionIndex[2] = GetElementIndex( "Skill_Probe" );
		m_nKickActionIndex[3] = GetElementIndex( "Skill_Swoop" );

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

			// �̷��� Attach, Detach���� �Ѵ� CombineParts�� �ϸ�, �����Ҷ� �ι� ȣ��Ǵ� ������ �ǹ�����.
			// �̰� ������ �ѹ��� �Ҷ� ������ ������ ���������� ���⶧���� ��� �Ĺ��� �Ǵ� �Ͱ� ���� ������,
			// �ƿ� �Ĺ��δܿ��� �������� �ִ��� �Ǵ��ϰ� �Ĺ����ϴ� ������ �ذ��ϰڴ�.
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

	// Loading ���� �Լ���
	static int __stdcall OnLoadChangePartsUniqueID( void *pParam, int nSize ) { return *((DWORD *)pParam); }

	// ItemTask�� InsertChangePartsThread�� ȣ���ϴ� ������ ���� ����ȿ� �� ĳ���� �ڽŹۿ� �ȵǴ�,
	// ���⼭ ������ ȣ���� �ؾ��Ѵ�. �׷��� �α��δܿ� �ִ� ������Ե� �����ų �� �ִ�.
	static bool __stdcall OnLoadChangeParts( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime )
	{
		if( nSize > 0 && CDnLoadingTask::GetInstance().CheckSameLoadObject( pParam, nSize ) ) return true;

		TDnPlayerCommon<T>* pTemp = (TDnPlayerCommon<T>*)pThis;
		MAPartsBody *pBody = dynamic_cast<MAPartsBody*>(pTemp);
		if( pBody ) pBody->CombineParts();
		return true;
	}
	// IgnoreCombine�� true�϶��� ���⼭
	virtual void OnCombineParts()
	{
		MAPartsBody *pBody = dynamic_cast<MAPartsBody*>(this);
		if( pBody ) {
			DnPartsHandle hHand = pBody->GetParts( CDnParts::Hand );
			DnPartsHandle hCashHand = pBody->GetCashParts( CDnParts::CashHand );
			DnPartsHandle hParts = pBody->GetCashParts( CDnParts::CashBody );	// �ѹ����϶��� ����ؼ� Body���� üũ.
			if( hHand && hHand->GetObjectHandle() && hHand->IsShow() ) hParts = hHand;
			if( hCashHand && hCashHand->GetObjectHandle() && hCashHand->IsShow() ) hParts = hCashHand;
			if( hParts && hParts->GetObjectHandle() ) {
				EtAniObjectHandle hHandle = hParts->GetObjectHandle();
				for( int i=0; i<hHandle->GetSubMeshCount(); i++ ) {
					CEtSubMesh *pSubMesh = hParts->GetObjectHandle()->GetSubMesh(i);
					if( pSubMesh && strstr( pSubMesh->GetSubMeshName(), "Glove_L" ) ) {
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

	// IgnoreCombine�� false�϶��� ���⼭
	virtual void OnCombineParts( EtSkinHandle hSkin, CEtSkinInstance **ppSkinInstance )
	{
		if( hSkin && hSkin->GetMeshHandle() && hSkin->GetMeshHandle()->GetSubMeshCount() > 0 && GetActiveWeapon(1) && GetActiveWeapon(1)->IsShow() ) {
			for( int i=0; i<hSkin->GetMeshHandle()->GetSubMeshCount(); i++ ) {
				CEtSubMesh *pSubMesh = hSkin->GetMeshHandle()->GetSubMesh(i);
				if( pSubMesh && strstr( pSubMesh->GetSubMeshName(), "Glove_L" )	) {
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

		// �⺻������ �Ϲ����� ������ �����ϰ�, ���������� �����ϸ鼭 �ʱ�ȭ�ǰų� �����ƹ�Ÿ���� ĳ���͸� ���鶧�� �����带 ������� �ʴ´�.
		m_bUseThreadOnCombineParts = bFlag;
	}

	virtual void ShowWeapon( int nEquipIndex, bool bShow )
	{	
	
		bool bRefreshCombine = false;
		if( nEquipIndex == 1 && m_hWeapon[nEquipIndex] && m_hWeapon[nEquipIndex]->IsShow() != bShow ) bRefreshCombine = true;

		// ���Խÿ� ����������� �Ⱥ��̴� �����̽�.
		// Ȯ���غ��� CDnPlayerActor::Show�� �����߿� �������� false, true�� ȣ��Ǵµ�,
		// �̶� ShowWeapon�� ȣ��Ǳ⶧���� Combineó���� ���� �ȴ�.
		// ������ Show(false)ȣ���ϸ鼭 Combine�� �������϶� Show(true)�� ȣ��Ǹ鼭 CDnPlayerActor::ShowParts�� ȣ��ǰ�,
		// �� �ȿ��� m_MergedObject�� Identity(�Ĺ������̶�)���¶� MergedObject�� Show(true)���� ����ä ������ Combine�� ����ǰ� �ȴ�.
		// �׷��� Show(false)���°� �����Ǵ� ��.
		//
		// ù��° �ذ�å�� CDnPlayerActor::ShowParts�� MergedObject Show�����κп� s_PartsLock���Ŵ°ǵ�, ���� �Ⱦ��ؼ� �н�.
		// �ι�° �ذ�å�� Combine���� ���� MergedObject�� IsShow�� �޾ƿ��� �ʰ� �׳� ������ IsShow�� �޾ƿ��� �ǵ�, ������ �ڵ尡 �ƴ϶� �ѱ��,
		// ����° �ذ�å�� ProcessHidePlayerActoró������ MergedObject�� IsShow�� �˻��� �ִ°ǵ� �ʹ� �������� �־ �ѱ��.
		// �׷��� ������ �ذ�å���� ShowWeapon(false)�ɶ� ���� ĳ���Ͱ� ���̵���¶�� �Ĺ������ʿ� ������ �ƿ� �Ĺ����� �ǳʶٴ°ɷ� �ϵ��� �Ѵ�.
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