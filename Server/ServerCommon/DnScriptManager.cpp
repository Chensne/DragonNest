#include "stdafx.h"
#include "DnScriptManager.h"
#include "StringSet.h"
#include "DnLuaAPIDefine.h"
#include "DNQuest.h"
#include "DNQuestManager.h"
#include "DebugSet.h"

DnScriptManager* g_pNpcQuestScriptManager = NULL;

DnScriptManager::DnScriptManager()
{
	RegistLuaAPI();
}

DnScriptManager::~DnScriptManager()
{

}

bool DnScriptManager::LoadScript(const char* szLuaFile, bool bIsCheckServer, int nLuaIndex, bool bQuestScript )
{
	lua_State* pLua = OpenStateByIndex(nLuaIndex);
	if (!pLua) {
		g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! - !pLua (%S)\n", szLuaFile);
		DN_RETURN(false);
	}

	CFileStream Stream( CEtResourceMng::GetInstance().GetFullName(szLuaFile).c_str() , CFileStream::OPEN_READ );
	
	if ( Stream.IsValid() == false )
	{
		OutputDebug("Cannot open file : %s\n" , szLuaFile);
		g_Log.Log( LogType::_FILELOG, "Cannot open file : %s\n" , szLuaFile );
		DN_RETURN(false);
	}

	int nLen = Stream.Size();
	if (0 >= nLen) {
		g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! - 0 >= nLen (1) (%d) (%S)\n", nLen, szLuaFile);
		DN_RETURN(false);
	}

	char* pBuffer = new(std::nothrow) char[nLen+1];
	if (!pBuffer) {
		g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! - !pBuffer (%S)\n", szLuaFile);
		DN_RETURN(false);
	}

	ZeroMemory(pBuffer, nLen+1);

	class __Temp
	{
	public:
		__Temp(char* p) { m_p = p;}
		~__Temp() { delete[] m_p; }
		char* m_p;
	};
	__Temp ScopeDelete(pBuffer);

	int nReadSize = Stream.Read(pBuffer, nLen);
	
	if( nReadSize != nLen ) {
		g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! nLen (%d) != nReadSize (%d) (%S)\n", nLen, nReadSize, szLuaFile);
		DN_RETURN(false);
	}
	
	std::string strErrMsg;

	if (bIsCheckServer) {
		CStringSet pScriptRangeStx;
		CStringSet pScriptRangeEtx;
#if defined(_VILLAGESERVER)
		pScriptRangeStx = _T("<VillageServer>");
		pScriptRangeEtx = _T("</VillageServer>");
#elif defined(_GAMESERVER)
		pScriptRangeStx = _T("<GameServer>");
		pScriptRangeEtx = _T("</GameServer>");
#endif	// _VILLAGESERVER
		DN_ASSERT(0 < pScriptRangeStx.GetLength(),	"Invalid!");
		DN_ASSERT(0 < pScriptRangeEtx.GetLength(),	"Invalid!");

		CStringSet LuaChunk = CA2T(pBuffer);

		int nP1 = LuaChunk.Find(pScriptRangeStx.Get(), 0);
		if (0 > nP1) {
			g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! - LuaChunk.Find(pScriptRangeStx.Get(), 0) (%S)\n", szLuaFile);
			DN_RETURN(false);
		}

		int nP2 = LuaChunk.Find(pScriptRangeEtx.Get(), 0);
		if (0 > nP2) {
			g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! - LuaChunk.Find(pScriptRangeEtx.Get(), 0) (%S)\n", szLuaFile);
			DN_RETURN(false);
		}

		CStringSet LuaChunkSub = LuaChunk.Mid(nP1 + pScriptRangeStx.GetLength(), nP2 - nP1 - pScriptRangeStx.GetLength());
		if (0 >= LuaChunkSub.GetLength()) {
			g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! - LuaChunk.Mid(nP1 + pScriptRangeStx.GetLength(), nP2 - nP1 - pScriptRangeStx.GetLength()) (%S)\n", szLuaFile);
			DN_RETURN(false);
		}

		if (!CheckLuaAPI(LuaChunkSub))
		{
			g_Log.Log(LogType::_FILELOG, L"DnScriptManager::LoadScript(...) Failed !!! - CheckLuaAPI (%S)\n", szLuaFile);		
#if !defined(_FINAL_BUILD)//rlkt_test
			DN_RETURN(false);
#endif
		}

		if(bQuestScript)
			CheckRemoteComplete(szLuaFile, LuaChunkSub);

		if (lua_tinker::dobuffer(pLua, CT2A(LuaChunkSub.Get()), LuaChunkSub.GetLength(), &strErrMsg) != 0) {
			OutputDebug("Cannot open file : %s (%s)\n" , szLuaFile, strErrMsg.c_str());
			g_Log.Log( LogType::_FILELOG, "Cannot open file : %s (%s)\n" , szLuaFile, strErrMsg.c_str());
			DN_RETURN(false);
		}
	}
	else {
		if (lua_tinker::dobuffer(pLua, pBuffer, nLen, &strErrMsg) != 0) {
			OutputDebug("Cannot open file : %s (%s)\n" , szLuaFile, strErrMsg.c_str());
			g_Log.Log( LogType::_FILELOG, "Cannot open file : %s (%s)\n" , szLuaFile, strErrMsg.c_str());
			DN_RETURN(false);
		}
	}

	return true;
}

void DnScriptManager::RegistLuaAPI()
{
	for( int i=0;; i++ ) 
	{
		if( g_szLuaAPIList[i] == NULL ) 
		{	
			break;
		}

		m_mLuaAPI.insert(make_pair(g_szLuaAPIList[i], i));
	}
}

bool DnScriptManager::IsValidLuaAPI(LPCSTR pApiName)
{
	if (pApiName == NULL)
		return false;

	std::map<std::string, int>::iterator iter = m_mLuaAPI.find(pApiName);
	if (iter != m_mLuaAPI.end())
		return true;

	return false;
}

bool DnScriptManager::CheckLuaAPI(CStringSet& LuaChunkSub)
{
	CStringSet pScriptAPIStx = _T("api_");
	CStringSet pScriptAPIEtx = _T("(");
	CStringSet LuaChunkApi;

	int nCurrentPos = 0;
	int nStartPos = 0;
	int nEndPos = 0;
	int nAPILength = 0;

	while (true)
	{
		nStartPos = LuaChunkSub.Find(pScriptAPIStx.Get(), nCurrentPos);
		nCurrentPos = nStartPos;

		if (nStartPos == -1)
			break;

		nEndPos = LuaChunkSub.Find(pScriptAPIEtx.Get(), nCurrentPos);
		nCurrentPos = nEndPos;

		nAPILength = nEndPos - nStartPos;
		if (nAPILength < 0)
			return false;

		LuaChunkApi.Reset();
		LuaChunkApi = LuaChunkSub.Mid (nStartPos, nAPILength);

		LuaChunkApi.Trim();	// 공백제거

		LPCSTR pApiName = LuaChunkApi.GetA();
		if (!IsValidLuaAPI (pApiName))
		{
			
			CDebugSet::ToFile("Invalid LuaAPI : %s\n" , pApiName);
			g_Log.Log( LogType::_FILELOG, "Invalid LuaAPI : %s\n" , pApiName);
			free((VOID*)pApiName);
			return false;
		}
		free((VOID*)pApiName);
	}

	return true;
}

void DnScriptManager::CheckRemoteComplete(const char* szLuaFile, CStringSet& LuaChunkSub)
{
	CStringSet pScriptAPI = _T("_CanRemoteCompleteStep");
	int nPos = LuaChunkSub.Find(pScriptAPI.Get(), 0);
	if(0 < nPos)
		return;

	std::vector<std::string> paths;
	TokenizeA(szLuaFile, paths, "\\");

	if(paths.size() < 1)
		return;

	std::vector<std::string> tokens;
	TokenizeA(paths[paths.size() - 1], tokens, "_");

	if(tokens.size() < 2)
		return;

	int nQuestIndex = atoi(tokens[1].c_str());
	CDNQuest * pQuest = g_pQuestManager->GetQuest( nQuestIndex );
	if(!pQuest)
		return;

	pQuest->GetQuestInfo().bCheckRemoteComplete = false;
}