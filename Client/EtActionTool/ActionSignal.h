#pragma once

#include "ActionBase.h"

#define _EXPORT_EXCEL_ACTION_SIGNAL

class CUnionValueProperty;
class CSignalCustomRender;
class CActionSignal : public CActionBase {
public:
	CActionSignal();
	virtual ~CActionSignal();

protected:
	int m_nSignalIndex;
	int m_nStartFrame;
	int m_nEndFrame;
	int m_nYOrder;

	CSignalCustomRender *m_pCustomRender;
//	std::vector<CUnionValueProperty *> m_pVecList;
//	std::vector<int> m_nVecParamUniqueList;
	std::vector<EtVector2> m_VecVector2List;
	std::vector<EtVector3> m_VecVector3List;
	std::vector<EtVector4> m_VecVector4List;
	std::vector<CString> m_VecStringList;
	std::vector<int> m_nVecVector2TableIndexList;
	std::vector<int> m_nVecVector3TableIndexList;
	std::vector<int> m_nVecVector4TableIndexList;
	std::vector<int> m_nVecStringTableIndexList;
//	std::vector<int> m_nVecParamUniqueList;
	void CalcArray();

public:
	void SetSignalIndex( int nSignalIndex ) { m_nSignalIndex = nSignalIndex; }
	int GetSignalIndex() const { return m_nSignalIndex; }
	void SetStartFrame( int nFrame ) { m_nStartFrame = nFrame; }
	void SetEndFrame( int nFrame ) { m_nEndFrame = nFrame; }
	int GetStartFrame() { return m_nStartFrame; }
	int GetEndFrame() { return m_nEndFrame; }
	void SetYOrder( int nOrder ) { m_nYOrder = nOrder; }
	int GetYOrder() { return m_nYOrder; }
	
#ifdef _EXPORT_EXCEL_ACTION_SIGNAL
	void ExportSignal( FILE *fp, int nCount );
#endif 

	virtual void Process( LOCAL_TIME LocalTime );
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );

	virtual bool ExportObject( FILE *fp, int &nCount );
	virtual bool ImportObject( FILE *fp );

	CSignalCustomRender *GetCustomRender() { return m_pCustomRender; }

	CActionSignal &operator = ( CActionSignal &e );
};

