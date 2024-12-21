#pragma once
#include <vector>

struct stControlPositionData
{
	int		nID;
	
	int		nTextID;
	int		nType;
	int		nEnum;
	RECT	RectPos;
};

class CDnControlData
{
private:
	CDnControlData();
	~CDnControlData();


	CString m_szLanguageParam; // EU쪽 멀티랭귀지.
	std::vector<stControlPositionData>	m_vecCtrlPosData;


	bool	LoadXMLFile(char* szFilePath);
	bool	LoadResourceFromXMLFile(int nID, LPCTSTR szFileName);
	void	ParsingRectData(WCHAR* szPosition, RECT& rt);
	bool	SetPositionData(CRect* pRt, int nMax);
	RECT	GetCtrlRect(int enumIndex);

public:
	static CDnControlData &GetInstance()
	{
		static CDnControlData obj;
		return obj;
	}
	
	CString GetFilePath( WCHAR* szFilePath );

	// EU용 멀티랭귀지
	void	SetLanguageParam(CString str) { m_szLanguageParam = str; }
	WCHAR*	GetMulitiLanguageFilePath();
	int		GetMulitiLanguageResourceID(); // 유럽 : 국가에 맞는 리소스 아이디를 뱉는다.
	
	// 컨트롤 위치 받아 셋팅하기.
	HRESULT SetControlDataFromLocalFile( CString szFilePath , CRect* pRect, int nMaxCnt );				 // 로컬파일의 XML
	HRESULT SetControlDataFromResource( int nResourceID, LPCTSTR szFileName, CRect* pRect, int nMaxCnt );// Resource의 XML
	/* arg1 : 리소스 아이디, arg2 : 만들어질 파일이름, arg3: 컨트롤rect, arg4 : 컨트롤 총 갯수 */
};

#define DNCTRLDATA CDnControlData::GetInstance()