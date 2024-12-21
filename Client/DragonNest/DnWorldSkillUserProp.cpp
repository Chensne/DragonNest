#include "StdAfx.h"
#include "DnWorldSkillUserProp.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



CDnWorldSkillUserProp::CDnWorldSkillUserProp(void)
{

}

CDnWorldSkillUserProp::~CDnWorldSkillUserProp(void)
{
}


bool CDnWorldSkillUserProp::InitializeTable(int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	if( GetData() )
	{
		// 클라 쪽에서는 별로 할 일이 없지.
		//SkillUserStruct* pStruct = (SkillUserStruct*)GetData();
		//int iSkillID = pStruct->SkillTableID;
		//int iSkillLevel = pStruct->SkillLevel;
		//m_fCheckRadius = pStruct->fRadius;
		//m_iUseType = pStruct->UseType;
	}

	return true;
}

bool CDnWorldSkillUserProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}


void CDnWorldSkillUserProp::Process( LOCAL_TIME LocalTime, float fDelta )
{
	//switch( m_iUseType )
	//{
	//	case ONCE:
	//		
	//		break;

	//	case CONTINUOUS:
	//		
	//		break;
	//}
}



void CDnWorldSkillUserProp::OnSignal( SignalTypeEnum Type, void* pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	// 프랍에 정의된 이펙트도 ActionBase 에서 자동으로 나올테니 여기서도 역시 별로 할 일이 없지..
}