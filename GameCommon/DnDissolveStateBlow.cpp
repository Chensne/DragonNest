#include "StdAfx.h"
#include "DnDissolveStateBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnDissolveStateBlow::CDnDissolveStateBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_155;
	SetValue( szValue );
	//m_nValue = atoi( szValue );
	m_fValue = 0.0f;//(float)atof(szValue);

	m_BeginLevel = m_EndLevel = 0;

	AddDissolveStateID(szValue);
}

CDnDissolveStateBlow::~CDnDissolveStateBlow(void)
{
}

void CDnDissolveStateBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}

void CDnDissolveStateBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

	// 서버에서만 동작 하면 될듯. [2010/12/17 semozz]
	// 서버에서 상태효과 제거 할것들.. 패킷 보내도록 수정함.
#if defined(_GAMESERVER)
	//적용 레벨 범위 밖에 있으면 그냥 건너뛴다.
	int nCharLevel = m_hActor->GetLevel();
	if (nCharLevel < m_BeginLevel || m_EndLevel < nCharLevel)
		return;

	DNVector(int) vlAppliedDebuffBlowIds;

	//등록된 StateBlowIndex를 순회
	DISSOLVE_LIST::iterator iter = m_DissolveStateIDList.begin();
	DISSOLVE_LIST::iterator endIter = m_DissolveStateIDList.end();
	for ( ; iter != endIter; ++iter)
	{
		DNVector(DnBlowHandle) vResult;
		m_hActor->GatherAppliedStateBlowByBlowIndex((STATE_BLOW::emBLOW_INDEX)iter->second, vResult);

		//등록되어 있는 StateBlow들 각각을 제거..
		for( UINT i=0 ; i<vResult.size() ; ++i ) 
		{
			DnBlowHandle hBlow = vResult.at( i );
			if( !hBlow )
				continue;

			vlAppliedDebuffBlowIds.push_back(hBlow->GetBlowID());
		}
	}
	
	// 해제 가능한 나쁜 스킬, 상태효과만 해제
	int iNumBlow = (int)vlAppliedDebuffBlowIds.size();
	for( int iBlow = 0; iBlow < iNumBlow; ++iBlow )
	{
		int iBlowID = vlAppliedDebuffBlowIds.at( iBlow );
		//m_hActor->RemoveStateBlowFromID( iBlowID );
		
		m_hActor->CmdRemoveStateEffectFromID(iBlowID);
	}
#endif // _GAMESERVER
}

void CDnDissolveStateBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}

void CDnDissolveStateBlow::AddDissolveStateID(const char* szValue)
{
	// 적용 레벨 기능 추가 [2010/11/30 semozz]
	// [##;##;...##;][##:##]
	// [상태효과 Index들][적용레벨 시작 : 적용레벨  끝]

	std::string str = szValue;//"[45;46;47;][23:45]";
	std::vector<std::string> tokens;
	std::string delimiters = "[]";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);
	}
	else
	{
		//2. 상태효과 인덱스 리스트 구분
		std::string stateIndexList = tokens[0];
		std::vector<std::string> stateIndexTokens;
		delimiters = ";";

		TokenizeA(stateIndexList, stateIndexTokens, delimiters);
		std::vector<std::string>::iterator iter = stateIndexTokens.begin();
		for ( ; iter != stateIndexTokens.end(); ++iter)
		{
			AddDissolveStateID(atoi(iter->c_str()));
		}

		//3. 적용 레벨 구분
		std::string levelInfo = tokens[1];
		std::vector<std::string> levelInfoTokens;
		delimiters = ":";
		TokenizeA(levelInfo, levelInfoTokens, delimiters);
		
		if (levelInfoTokens.size() != 2)
		{
			OutputDebug("%s :: %s --> 상태효과 적용 레벨 점검 필요!!!\n", __FUNCTION__, szValue);
		}
		else
		{
			int startLevel = atoi(levelInfoTokens[0].c_str());
			int endLevel = atoi(levelInfoTokens[1].c_str());

			SetLevelInfo(startLevel, endLevel);
		}
		
	}
}

void CDnDissolveStateBlow::AddDissolveStateID(int nStateBlowIndex)
{
	m_DissolveStateIDList.insert(DISSOLVE_LIST::value_type(nStateBlowIndex, nStateBlowIndex));
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDissolveStateBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[256] = {0, };

	std::string szStateID[2];
	int startLevel[2] = {0, };
	int endLevel[2] = {0, };

	std::string str = szOrigValue;//"[45;46;47;][23:45]";
	std::vector<std::string> tokens;
	std::string delimiters = "[]";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);
	}
	else
	{
		//2. 상태효과 인덱스 리스트 구분
		szStateID[0] = tokens[0];
		
		//3. 적용 레벨 구분
		std::string levelInfo = tokens[1];
		std::vector<std::string> levelInfoTokens;
		delimiters = ":";
		TokenizeA(levelInfo, levelInfoTokens, delimiters);

		if (levelInfoTokens.size() != 2)
		{
			OutputDebug("%s :: %s --> 상태효과 적용 레벨 점검 필요!!!\n", __FUNCTION__, szOrigValue);
		}
		else
		{
			startLevel[0] = atoi(levelInfoTokens[0].c_str());
			endLevel[0] = atoi(levelInfoTokens[1].c_str());
		}
	}


	//////////////////////////////////////////////////////////////////////////
	str = szAddValue;//"[45;46;47;][23:45]";
	
	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);
	}
	else
	{
		//2. 상태효과 인덱스 리스트 구분
		szStateID[1] = tokens[0];

		//3. 적용 레벨 구분
		std::string levelInfo = tokens[1];
		std::vector<std::string> levelInfoTokens;
		delimiters = ":";
		TokenizeA(levelInfo, levelInfoTokens, delimiters);

		if (levelInfoTokens.size() != 2)
		{
			OutputDebug("%s :: %s --> 상태효과 적용 레벨 점검 필요!!!\n", __FUNCTION__, szAddValue);
		}
		else
		{
			startLevel[1] = atoi(levelInfoTokens[0].c_str());
			endLevel[1] = atoi(levelInfoTokens[1].c_str());
		}
	}


	//////////////////////////////////////////////////////////////////////////
	std::string szResultStateIDList = szStateID[0];
	//szOrigValue 마지막에 ";"가 없으면 추가 해서 szAddValue를 추가 한다.
	int nLength = (int)szResultStateIDList.size();

	if (nLength == 0 || szOrigValue[nLength - 1] == ';')
		szResultStateIDList += szStateID[1];
	else
	{
		szResultStateIDList += ";";
		szResultStateIDList += szStateID[1];
	}

	int nResultStartLevel = min(startLevel[0], startLevel[1]);
	int nResultEndLevel = max(endLevel[0], endLevel[1]);
	//////////////////////////////////////////////////////////////////////////

	sprintf(szBuff, "[%s][%d;%d]", szResultStateIDList.c_str(), nResultStartLevel, nResultEndLevel);

	szNewValue = szBuff;
}

void CDnDissolveStateBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	std::string szStateID[2];
	int startLevel[2] = {0, };
	int endLevel[2] = {0, };

	std::string str = szOrigValue;//"[45;46;47;][23:45]";
	std::vector<std::string> tokens;
	std::string delimiters = "[]";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);
	}
	else
	{
		//2. 상태효과 인덱스 리스트 구분
		szStateID[0] = tokens[0];

		//3. 적용 레벨 구분
		std::string levelInfo = tokens[1];
		std::vector<std::string> levelInfoTokens;
		delimiters = ":";
		TokenizeA(levelInfo, levelInfoTokens, delimiters);

		if (levelInfoTokens.size() != 2)
		{
			OutputDebug("%s :: %s --> 상태효과 적용 레벨 점검 필요!!!\n", __FUNCTION__, szOrigValue);
		}
		else
		{
			startLevel[0] = atoi(levelInfoTokens[0].c_str());
			endLevel[0] = atoi(levelInfoTokens[1].c_str());
		}
	}


	//////////////////////////////////////////////////////////////////////////
	str = szAddValue;//"[45;46;47;][23:45]";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);
	}
	else
	{
		//2. 상태효과 인덱스 리스트 구분
		szStateID[1] = tokens[0];

		//3. 적용 레벨 구분
		std::string levelInfo = tokens[1];
		std::vector<std::string> levelInfoTokens;
		delimiters = ":";
		TokenizeA(levelInfo, levelInfoTokens, delimiters);

		if (levelInfoTokens.size() != 2)
		{
			OutputDebug("%s :: %s --> 상태효과 적용 레벨 점검 필요!!!\n", __FUNCTION__, szAddValue);
		}
		else
		{
			startLevel[1] = atoi(levelInfoTokens[0].c_str());
			endLevel[1] = atoi(levelInfoTokens[1].c_str());
		}
	}


	//////////////////////////////////////////////////////////////////////////
	std::string szResultStateIDList = "";
	
	//"###;###;###;###;....;##;##;##"
	//|<--이전 문자열----->|<-AddValue->|

	//szOrigValue에서 szAddValue 문자열길이 만큼 뒤에서 자른다.
	int nOrigLength = (int)szStateID[0].size();
	int nAddLength = (int)szStateID[1].size();
	int nCount = nOrigLength - nAddLength;

	sprintf_s(szBuff, "%s", szStateID[0].c_str());
	szBuff[nCount] = 0;

	szResultStateIDList = szBuff;


	int nResultStartLevel = min(startLevel[0], startLevel[1]);
	int nResultEndLevel = max(endLevel[0], endLevel[1]);
	//////////////////////////////////////////////////////////////////////////

	sprintf(szBuff, "[%s][%d;%d]", szResultStateIDList.c_str(), nResultStartLevel, nResultEndLevel);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
