#pragma once

class CGlobalValue : public CSingleton< CGlobalValue > 
{
public:
	CGlobalValue(void);
	virtual ~CGlobalValue(void);

public:
	CView *m_pCurView;

public:

};
