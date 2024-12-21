#pragma once

class CDnSkillAcquireButton : public CEtUIButton
{
protected:
	bool m_bBlink;
	int m_nResult;

public:
	CDnSkillAcquireButton( CEtUIDialog *pParent );
	virtual ~CDnSkillAcquireButton(void);

public:
	bool IsBlink( void ) { return m_bBlink; };
	void SetBlink( bool bBlink );
	void SetResult( int nResult ){ m_nResult = nResult; }
	int GetResult(){ return m_nResult; }

public:
	virtual void Render( float fElapsedTime );
};