#pragma once

class CRTCutSceneMakerFrame;



// ���� ���ø����̼� ��ü
class CTheApp : public wxApp
{
private:
	CRTCutSceneMakerFrame*			m_pFrame;


public:
	CTheApp( void );
	virtual ~CTheApp( void );

	virtual bool OnInit( void );
};