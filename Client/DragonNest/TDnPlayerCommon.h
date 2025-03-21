#pragma once

#include "DnWeapon.h"
#include "DnTableDB.h"

template < class T >

class TDnPlayerCommon : public T
{
public:
	TDnPlayerCommon( int nClassID, bool bProcess = true )
		: T( nClassID, bProcess ) {
	};

	virtual ~TDnPlayerCommon() {
		for( int i=0; i<2; i++ )
			SAFE_RELEASE_SPTR( m_hFoot[i] );
	};

protected:
	DnWeaponHandle m_hFoot[2];
	std::map<std::string,std::map<int,std::string>> m_szMapEquipMatchActionMap;

public:
	// Actor Message
	virtual bool Initialize()
	{
		T::Initialize();

		int j = 0;
		for (; j < 2; j++)
			SAFE_RELEASE_SPTR(m_hFoot[j]);

		m_hFoot[0] = CDnWeapon::CreateWeapon( 88, 0, 0, 0, 0, 0, false, false, false );
		m_hFoot[1] = CDnWeapon::CreateWeapon( 89, 0, 0, 0, 0, 0, false, false, false );
		const char *szFootName[2] = { "Bip01 L Foot", "Bip01 R Foot" };
		for( int i=0; i<2; i++ ) {
			m_hFoot[i]->CreateObject();
			m_hFoot[i]->LinkWeapon( GetMySmartPtr(), -1, szFootName[i] );
		}

		CalcEquipMatchAction();
		return true;
	}

	virtual void CalcEquipMatchAction()
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TEQUIPMATCHACTION );
		std::vector<int> nVecList;
		pSox->GetItemIDListFromField( "_Class", m_nClassID, nVecList );

		for( DWORD i=0; i<nVecList.size(); i++ ) {
			int nEquipType = pSox->GetFieldFromLablePtr( nVecList[i], "_WeaponType" )->GetInteger();
			const char *szAction = pSox->GetFieldFromLablePtr( nVecList[i], "_Action" )->GetString();
			const char *szChangeAction = pSox->GetFieldFromLablePtr( nVecList[i], "_ChangeAction" )->GetString();

			std::map<std::string,std::map<int,std::string>>::iterator it = m_szMapEquipMatchActionMap.find( szAction );
			if( it != m_szMapEquipMatchActionMap.end() ) {
				it->second.insert( make_pair( nEquipType, szChangeAction ) );
			}
			else {
				std::map<int,std::string> nMapTemp;
				nMapTemp.insert( make_pair( nEquipType, szChangeAction ) );
				m_szMapEquipMatchActionMap.insert( make_pair( szAction, nMapTemp ) );
			}
		}
	}
	virtual DnWeaponHandle GetStandardWeapon() { return m_hWeapon[0]; }

	const char *GetEquipMatchAction( const char *szActionName )
	{
		m_bChangedEquipAction = false;

		if( IsSwapSingleSkin() ) return szActionName;
		DnWeaponHandle hWeapon = GetStandardWeapon();
		if( !hWeapon ) return szActionName;
		int nEquipType = (int)hWeapon->GetEquipType();

		std::map<std::string,std::map<int,std::string>>::iterator it = m_szMapEquipMatchActionMap.find( szActionName );
		if( it == m_szMapEquipMatchActionMap.end() ) return szActionName;

		std::map<int,std::string>::iterator it2 = it->second.find( nEquipType );
		if( it2 == it->second.end() ) return szActionName;

		m_bChangedEquipAction = true;
		m_strOriginalNoneEquipAction.assign( szActionName );
	
		return it2->second.c_str();
	}


	virtual void Process( LOCAL_TIME LocalTime, float fDelta )
	{
		T::Process( LocalTime, fDelta );

		for( int i=0; i<2; i++ )
		{
			if( m_hFoot[i] )
				m_hFoot[i]->Process( LocalTime, fDelta );
		}
	}
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
	{
		switch( Type ) {
			case STE_ShowSwordTrail:
				{
					ShowSwordTrailStruct *pStruct = (ShowSwordTrailStruct *)pPtr;
					if( pStruct->nWeaponIndex >= 2 && pStruct->nWeaponIndex <= 3 ) {
						m_hFoot[pStruct->nWeaponIndex - 2]->ShowTrail( SignalEndTime - LocalTime, pStruct );
						return;
					}
				}
				break;
		}
		T::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	}

	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, bool bFromStateBlow = false, bool bSkillChain = false )
	{
		T::CmdAction( GetEquipMatchAction(szActionName), nLoopCount, fBlendFrame, bCheckOverlapAction, bFromStateBlow, bSkillChain );
	}

	virtual void CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, int nLoopCount = 0, float fFrame = 0.f, float fBlendFrame = 3.f )
	{
		T::CmdMixedAction( szActionBone, szMaintenanceBone, GetEquipMatchAction(szActionName), nLoopCount, fFrame, fBlendFrame );
	}
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bResetStop = false, bool bForce = false )
	{
		T::CmdStop( GetEquipMatchAction(szActionName), nLoopCount, fBlendFrame, fStartFrame, bResetStop, bForce );
	}

	virtual void SetCustomAction( const char *szActionName, float fFrame )
	{
		T::SetCustomAction( GetEquipMatchAction(szActionName), fFrame );
	}

	virtual void CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.0f, bool bChargeKey = false, bool bCheckOverlapAction = true, bool bOnlyCheck = false )
	{
		T::CmdPassiveSkillAction( nSkillID, GetEquipMatchAction(szActionName), nLoopCount, fBlendFrame, fStartFrame, bChargeKey, bCheckOverlapAction, bOnlyCheck );
	}
};
