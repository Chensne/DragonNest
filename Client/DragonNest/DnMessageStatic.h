#pragma once

class CDnMessageStatic : public CEtUIStatic
{
public:
	CDnMessageStatic( CEtUIDialog *pParent );
	virtual ~CDnMessageStatic(void);

protected:
	float m_fMessageDisplayTime;

public:
	void SetMsgTime( float fTime ) { m_fMessageDisplayTime = fTime; }
	float GetMsgTime() { return m_fMessageDisplayTime; }
	
public:
	virtual void Process( float fElapsedTime );
};
