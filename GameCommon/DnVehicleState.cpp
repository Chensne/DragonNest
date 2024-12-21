#include "StdAfx.h"
#include "DnVehicleState.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnVehicleState::CDnVehicleState()
{
	m_nVehicleClassID = 0;
}

CDnVehicleState::~CDnVehicleState()
{
}

void CDnVehicleState::CalcBaseState( StateTypeEnum Type )
{
	if( m_nVehicleClassID == 0 ) return;
	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if( !pVehicleTable || !pVehicleTable->IsExistItem(m_nVehicleClassID))
		return;
	if( Type & ST_MoveSpeed ) 
	{
		int nVehicleActorSpeed = pVehicleTable->GetFieldFromLablePtr( m_nVehicleClassID, "_VehicleDefaultSpeed" )->GetInteger();
		m_BaseState.SetMoveSpeed( nVehicleActorSpeed );
	}

	m_BaseState.CalcValueType();
}

void CDnVehicleState::SetVehicleClassID( int nID )
{
	m_nVehicleClassID = nID;
	RefreshState();
} 

void CDnVehicleState::CalcState( RefreshStateExtentEnum Extent, StateTypeEnum Type )
{
	std::vector<CDnState *> pVecAbsoluteList[2];
	std::vector<CDnState *> pVecRatioList[2];

	GetStateList( BaseList | StateEffectList , ValueTypeAbsolute, pVecAbsoluteList[0] );
	GetStateList( BaseList | StateEffectList , ValueTypeRatio, pVecRatioList[0] );
	GetStateList( PostStateEffectList, ValueTypeRatio, pVecRatioList[1] );
	GetStateList( PostStateEffectList, ValueTypeAbsolute, pVecAbsoluteList[1] );

	if( Type & ST_MoveSpeed ) CalcMoveSpeed( Extent, pVecAbsoluteList, pVecRatioList );

	CalcValueType();
}

void CDnVehicleState::CalcMoveSpeed( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList )
{
	float fRatioValue;
	int nAddValue;
	int nTemp;

	if( Extent & RefreshBase ) 
	{
		DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
		if( !pVehicleTable || !pVehicleTable->IsExistItem(m_nVehicleClassID))
			return;

		int nVehicleActorSpeed = pVehicleTable->GetFieldFromLablePtr( m_nVehicleClassID, "_VehicleDefaultSpeed" )->GetInteger();
		m_BaseState.SetMoveSpeed( nVehicleActorSpeed );
	}

	if( Extent & RefreshEquip ) 
	{
		CALC_STATE_VALUE_ABSOLUTE( pVecAbsoluteList[0], nAddValue, GetMoveSpeed() );
		nTemp = nAddValue;
		CALC_STATE_VALUE_RATIO( pVecRatioList[0], nTemp, fRatioValue, GetMoveSpeedRatio() );
		nTemp += (int)fRatioValue;

		m_StateStep[0].SetMoveSpeed( nTemp );
	}

	if( Extent & RefreshSkill ) {
		// PostStateEffect
		CALC_STATE_VALUE_RATIO( pVecRatioList[1], m_StateStep[0].GetMoveSpeed(), fRatioValue, GetMoveSpeedRatio() );
		nTemp = (int)fRatioValue;

		CALC_STATE_VALUE_ABSOLUTE( pVecAbsoluteList[1], nAddValue, GetMoveSpeed() );
		nTemp += nAddValue;

		m_StateStep[1].SetMoveSpeed( nTemp );
	}

	m_nMoveSpeed = m_StateStep[0].GetMoveSpeed() + m_StateStep[1].GetMoveSpeed();
}
