#pragma once

#include "SignalItem.h"

class CUnionValueProperty;
class CWizardCommon {
public:
	enum VIEW_TYPE {
		UNKNOWN_TYPE,
		SELECT_EDIT_TYPE,
		CREATE_SIGNAL,
		SELECT_SIGNAL,
		CREATE_PARAMETER,
		SELECT_PARAMETER,
		FINISH,
	};

	CWizardCommon() {}
	virtual ~CWizardCommon() {}

protected:
	VIEW_TYPE m_PrevViewType;


public:
	virtual VIEW_TYPE GetNextView() { return UNKNOWN_TYPE; }
	VIEW_TYPE GetPrevView() { return m_PrevViewType; }
	void SetPrevViewType( VIEW_TYPE View ) { m_PrevViewType = View; }
	CWnd *GetWizardWindow( CWizardCommon::VIEW_TYPE Type );

	virtual bool IsVisivleNextButton() { return true; }
	virtual bool IsVisivlePrevButton() { return true; }
	virtual bool IsVisivleFinishButton() { return true; }
	virtual bool IsFinishOrCancel() { return true; }

	virtual void PreProcess() {}
	virtual bool PostProcess() { return true; }

	static CSignalItem *s_pCurrentSignalItem;
	static CUnionValueProperty *s_pCurrentPropertyVariable;
	static int s_nStartType;
	static CString s_szCurrentSignalName;
	static CString s_szCurrentPropertyName;
};

struct SignalWizardStruct {
	CWizardCommon::VIEW_TYPE ViewType;
	CRuntimeClass *pRuntimeClass;
	CWnd *pThis;
};


extern SignalWizardStruct g_SignalWizardStructDefine[];