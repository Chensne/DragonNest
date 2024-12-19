#pragma once

class CActionSignal;
class CInputReceiver;
class CSignalCustomRender {
public:
	CSignalCustomRender( const char *szSignalName ) { m_szSignalName = szSignalName; m_pSignal = NULL; m_bActivate = false; }
	virtual ~CSignalCustomRender() {}

protected:
	std::string m_szSignalName;
	CActionSignal *m_pSignal;
	bool m_bActivate;

protected:
	static std::vector<CSignalCustomRender *> s_pVecList;

public:
	static void RegisterClass();
	static void UnRegisterClass();
	static CSignalCustomRender *AllocCustomRender( const char *szSignalName );

	virtual CSignalCustomRender *Clone() { return new CSignalCustomRender( m_szSignalName.c_str() ); }

	void SetSignal( CActionSignal *pSignal ) { m_pSignal = pSignal; }
	const char *GetSignalName() { return m_szSignalName.c_str(); }

	virtual void Initialize() {}
	virtual void Release() {}
	// �ñ׳� ���� Ŭ�� �������� �߻��Ǵ� �̺�Ʈ 
	virtual void OnSelect( bool bFirst = false ) {}
	virtual void OnUnSelect() {}
	virtual void OnModify() {}
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver ) {}
	bool CheckSignal( float fStart, float fEnd );
	bool IsActivate() { return m_bActivate; }

	// #56216.
	void CheckStartEndSignal( float curFrame ); // �ñ׳� �������� �����˻�.  
	virtual void OnStartSignal(){} // �ñ׳� ���۽����� ȣ��Ǵ� �����Լ�.
	virtual void OnEndSignal(){} // �ñ׳� ��������� ȣ��Ǵ� �����Լ�.
};