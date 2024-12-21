#pragma once

#include "DnWeapon.h"
#include "DnTableDB.h"

template < class T >

class TDnPlayerCommon : public T
{
public:
	TDnPlayerCommon( CMultiRoom *pRoom, int nClassID )
		: T( pRoom, nClassID ) {
	};

	virtual ~TDnPlayerCommon() {
	};

protected:
	std::map<std::string,int> m_szMapEquipMatchActionMap;

public:
	// Actor Message
	virtual bool Initialize()
	{
		if( T::Initialize() == false ) return false;
		CalcEquipMatchAction();
		return true;
	}

	virtual void CalcEquipMatchAction()
	{
		DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TEQUIPMATCHACTION );
		std::vector<int> nVecList;
		pSox->GetItemIDListFromField( "_Class", m_nClassID, nVecList );

		for( DWORD i=0; i<nVecList.size(); i++ ) {
			int nEquipType = pSox->GetFieldFromLablePtr( nVecList[i], "_WeaponType" )->GetInteger();
			const char *szChangeAction = pSox->GetFieldFromLablePtr( nVecList[i], "_ChangeAction" )->GetString();

			m_szMapEquipMatchActionMap.insert( make_pair( szChangeAction, nEquipType ) );
		}
	}
	virtual DnWeaponHandle GetStandardWeapon() { return m_hWeapon[0]; }

	bool IsValidEquipMatchAction( const char *szActionName )
	{
		DnWeaponHandle hWeapon = GetStandardWeapon();
		if( !hWeapon ) return true;
		int nEquipType = (int)hWeapon->GetEquipType();

		std::map<std::string,int>::iterator it = m_szMapEquipMatchActionMap.find( szActionName );
		if( it == m_szMapEquipMatchActionMap.end() ) return true;

		if( nEquipType != it->second ) return false;

		return true;
	}

	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, bool bFromStateBlow = false, bool bSkillChain = false )
	{
		if( !IsValidEquipMatchAction( szActionName ) ) {
			OutputDebug( "무기 매칭 안되는거다. 핵썻당\n" );
		}
		T::CmdAction( szActionName, nLoopCount, fBlendFrame, bCheckOverlapAction, bFromStateBlow, bSkillChain );
	}

	virtual void CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, float fFrame = 0.f, float fBlendFrame = 3.f )
	{
		if( !IsValidEquipMatchAction( szActionName ) ) {
			OutputDebug( "무기 매칭 안되는거다. 핵썻당\n" );
		}
		T::CmdMixedAction( szActionBone, szMaintenanceBone, szActionName, fFrame, fBlendFrame );
	}
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f )
	{
		if( !IsValidEquipMatchAction( szActionName ) ) {
			OutputDebug( "무기 매칭 안되는거다. 핵썻당\n" );
		}
		T::CmdStop( szActionName, nLoopCount, fBlendFrame, fStartFrame );
	}

	virtual void SetCustomAction( const char *szActionName, float fFrame )
	{
		if( !IsValidEquipMatchAction( szActionName ) ) {
			OutputDebug( "무기 매칭 안되는거다. 핵썻당\n" );
		}
		T::SetCustomAction( szActionName, fFrame );
	}

	virtual void CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.0f, bool bChargeKey = false, bool bCheckOverlapAction = true, bool bOnlyCheck = false )
	{
		if( !IsValidEquipMatchAction( szActionName ) ) {
			OutputDebug( "무기 매칭 안되는거다. 핵썻당\n" );
		}
		T::CmdPassiveSkillAction( nSkillID, szActionName, nLoopCount, fBlendFrame, fStartFrame, bChargeKey, bCheckOverlapAction, bOnlyCheck );
	}
};