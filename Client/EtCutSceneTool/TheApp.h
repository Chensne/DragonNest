#pragma once

class CRTCutSceneMakerFrame;



// 메인 어플리케이션 객체
class CTheApp : public wxApp
{
private:
	CRTCutSceneMakerFrame*			m_pFrame;


public:
	CTheApp( void );
	virtual ~CTheApp( void );

	virtual bool OnInit( void );
};