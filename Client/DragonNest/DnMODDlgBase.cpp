#include "StdAfx.h"
#include "DnMODDlgBase.h"
#include "DnMODCustom1Dlg.h"
#include "DnMODCustom2Dlg.h"
#include "DnMODCustom3Dlg.h"
#include "DnTrigger.h"
#include "DnTriggerObject.h"
#include "DnTriggerElement.h"


CDnMODDlgBase::CDnMODDlgBase( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
	m_MODType = Unknown;
}

CDnMODDlgBase::~CDnMODDlgBase()
{
}

bool CDnMODDlgBase::InitializeMOD( MODDlgType Type, const char *szUIFileName )
{
	m_MODType = Type;
	m_szUIFileName = szUIFileName;
	return true;
}

#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
void CDnMODDlgBase::LinkValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex, int nGaugeType, int nPartsIndex )
{
	if( !pElement || !pElement->GetTriggerObject() || !pElement->GetTriggerObject()->GetTrigger() ) return;
	LinkValueStruct Struct;
	Struct.pTrigger = (CDnTrigger*)pElement->GetTriggerObject()->GetTrigger();
	Struct.nIndex = nIndex;
	Struct.nDefineValueIndex = nDefineValueIndex;
	Struct.nGaugeType = nGaugeType;
	Struct.nPartsIndex = nPartsIndex;
	m_VecLinkValueList.push_back( Struct );
}
#endif 

void CDnMODDlgBase::LinkValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex )
{
	if( !pElement || !pElement->GetTriggerObject() || !pElement->GetTriggerObject()->GetTrigger() ) return;
	LinkValueStruct Struct;
	Struct.pTrigger = (CDnTrigger*)pElement->GetTriggerObject()->GetTrigger();
	Struct.nIndex = nIndex;
	Struct.nDefineValueIndex = nDefineValueIndex;
	m_VecLinkValueList.push_back( Struct );
}

CDnMODDlgBase *CDnMODDlgBase::CreateMODDlg( MODDlgType Type, const char *szUIFileName )
{
	CDnMODDlgBase *pDlg = NULL;

	switch( Type ) {
		// Trigger Define ���� �����ؼ� �ش� int ���ڸ� ����ִ� UI
		case Counter: pDlg = new CDnMODCustom1Dlg( UI_TYPE_BOTTOM ); break;
		// ������ Actor�� ������ ���� �������� ����ִ� UI. Value �� 2�� �Ѽ�Ʈ�� �����մϴ�.( 0 = ���� �ڵ�, 1 = Progress �� ����� ��. ������ 0�� ����(HP) )
		case GaugeProgress: pDlg = new CDnMODCustom2Dlg( UI_TYPE_BOTTOM ); break;
#ifdef PRE_ADD_CRAZYDUC_UI
			// Ÿ�̸ӿ� ī���͸� ����ִ� UI
		case TimerAndCounter: pDlg = new CDnMODCustom3Dlg( UI_TYPE_BOTTOM ); break;
#endif 
	}
	if( pDlg ) {
		pDlg->InitializeMOD( Type, szUIFileName );
	}
	return pDlg;
}

CDnMODDlgBase::LinkValueStruct *CDnMODDlgBase::GetLinkValue( int nIndex )
{
	for( DWORD i=0; i<m_VecLinkValueList.size(); i++ ) {
		if( m_VecLinkValueList[i].nIndex == nIndex ) return &m_VecLinkValueList[i];
	}
	return NULL;
}