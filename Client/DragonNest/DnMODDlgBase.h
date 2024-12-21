#pragma once

#include "DnCustomDlg.h"

class CDnTrigger;
class CEtTriggerElement;
class CDnMODDlgBase : public CDnCustomDlg {
public:
	CDnMODDlgBase( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnMODDlgBase();

	enum MODDlgType {
		Unknown,
		Counter,
		GaugeProgress,
#ifdef PRE_ADD_CRAZYDUC_UI
		TimerAndCounter,
#endif 
	};
	
#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
	enum eGaugeType
	{
		eHP,
		ePartsHP,
		eSuperArmor,
	};
#endif

	struct LinkValueStruct {
		CDnTrigger *pTrigger;
		int nIndex;
		int nDefineValueIndex;
#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
		int nGaugeType;
		int nPartsIndex;
#endif
	};

protected:
	MODDlgType m_MODType;
	std::string m_szUIFileName;
	std::vector<LinkValueStruct> m_VecLinkValueList;

protected:
	LinkValueStruct *GetLinkValue( int nIndex );
	DWORD GetLinkValueCount() { return (DWORD)m_VecLinkValueList.size(); }
public:
	virtual bool InitializeMOD( MODDlgType Type, const char *szUIFileName );
#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
	virtual void LinkValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex, int nGaugeType = 0, int nPartsIndex = 0 );
#endif 
	virtual void LinkValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex );

	static CDnMODDlgBase *CreateMODDlg( MODDlgType Type, const char *szUIFileName );

};