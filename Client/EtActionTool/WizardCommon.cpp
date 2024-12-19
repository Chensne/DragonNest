#include "StdAfx.h"
#include "WizardCommon.h"

#include "resource.h"
#include "EventEditView_SelectEditType.h"
#include "EventEditView_SelectSignal.h"
#include "EventEditView_CreateModifySignal.h"
#include "EventEditView_Finish.h"

#include "EventEditView_SelectParameter.h"
#include "EventEditView_CreateModifyParameter.h"

int CWizardCommon::s_nStartType = -1;

CString CWizardCommon::s_szCurrentSignalName;
CSignalItem *CWizardCommon::s_pCurrentSignalItem = NULL;

CString CWizardCommon::s_szCurrentPropertyName;
CUnionValueProperty *CWizardCommon::s_pCurrentPropertyVariable = NULL;


SignalWizardStruct g_SignalWizardStructDefine[] = {
	{ CWizardCommon::SELECT_EDIT_TYPE, RUNTIME_CLASS( CEventEditView_SelectEditType ), NULL },
	{ CWizardCommon::CREATE_SIGNAL, RUNTIME_CLASS( CEventEditView_CreateModifySignal ), NULL },
	{ CWizardCommon::SELECT_SIGNAL, RUNTIME_CLASS( CEventEditView_SelectSignal ), NULL },
	{ CWizardCommon::FINISH, RUNTIME_CLASS( CEventEditView_Finish ), NULL },
	{ CWizardCommon::CREATE_PARAMETER, RUNTIME_CLASS( CEventEditView_CreateModifyParameter ), NULL },
	{ CWizardCommon::SELECT_PARAMETER, RUNTIME_CLASS( CEventEditView_SelectParameter ), NULL },
	{ CWizardCommon::UNKNOWN_TYPE, NULL, NULL },
};

CWnd *CWizardCommon::GetWizardWindow( CWizardCommon::VIEW_TYPE Type )
{
	int nCount = sizeof(g_SignalWizardStructDefine) / sizeof(SignalWizardStruct);
	for( int i=0; i<nCount; i++ ) {
		if( g_SignalWizardStructDefine[i].ViewType == Type ) {
			if( g_SignalWizardStructDefine[i].pThis == NULL ) continue;
			CWnd *pWnd = ((CFrameWnd*)g_SignalWizardStructDefine[i].pThis)->GetWindow(GW_CHILD);
			return pWnd;
		}
	}
	return NULL;
}