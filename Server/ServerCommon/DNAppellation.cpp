#include "StdAfx.h"
#include "DNAppellation.h"
#include "DNUserData.h"
#include "DnTableDB.h"
#include "SundriesFunc.h"
#include "DNUserSendManager.h"
#include "DNDBConnectionManager.h"
#include "DNLogConnection.h"
#include "DNUserSession.h"
#include "DNGameDataManager.h"
#include "DNDBConnection.h"
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
#include "DNMailSender.h"
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
#if defined( _GAMESERVER )
#include "DNGameRoom.h"
#endif

CDNAppellation::CDNAppellation( CDNUserSession *pUser )
{
	m_pUserSession = pUser;
	m_bResetPcbangAppellation = false;
}

CDNAppellation::~CDNAppellation()
{
}

bool CDNAppellation::LoadUserData()
{
	std::map<int, int> mapDelAppellation;	//first에 ArrayIndex가 들어옴
	mapDelAppellation.clear();
	TAppellationGroup *pAppellation = m_pUserSession->GetAppellationData();	
	g_pDataManager->ResetPeriodAppellation(pAppellation->Appellation, mapDelAppellation);	//캐쉬 칭호 초기화	
	SetPeriodAppellation(pAppellation->Appellation, mapDelAppellation);	//EffectItem 칭호 셋팅
	for( std::map<int, int>::iterator itor = mapDelAppellation.begin(); itor != mapDelAppellation.end(); itor++ )
		SendDelAppellation(itor->first);	//칭호 삭제	

	return true;
}

void CDNAppellation::CheckCollectionBookReward()
{
	CDNGameDataManager::TMapCollectionBook CollectionList = g_pDataManager->CollectionBookList();
	for (CDNGameDataManager::TMapCollectionBook::iterator iter = CollectionList.begin(); iter != CollectionList.end(); ++iter){
		if (iter->second.nVecAppellationArrayIndex.empty()) continue;
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
		if(IsExistAppellation(iter->second.nRewardAppellationArrayIndex)) continue;
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
		int nCount = 0;
		for (int i = 0; i < (int)iter->second.nVecAppellationArrayIndex.size(); i++){
			if (IsExistAppellation(iter->second.nVecAppellationArrayIndex[i]))
				nCount++;
		}

		if (nCount == iter->second.nVecAppellationArrayIndex.size()){
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
			AddAppellation(iter->second.nRewardAppellationArrayIndex);
			//컬렉션북에 새 호칭이 추가된 경우나, 기존에 익히지 못했던것을 습득한 경우
			CDNMailSender::Process(m_pUserSession, iter->second.nMailID[1]);
#else
			if (!IsExistAppellation(iter->second.nRewardAppellationArrayIndex)){
				AddAppellation(iter->second.nRewardAppellationArrayIndex);
			}
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
		}
	}
}

bool CDNAppellation::IsExistAppellation( int nArrayIndex )
{
#if defined(PRE_FIX_APPELLATION_BITSET_LENGTHCHECK)
	if(nArrayIndex < 0 || nArrayIndex >= APPELLATIONMAX) return false;
#endif	// #if defined(PRE_FIX_APPELLATION_BITSET_LENGTHCHECK)
	TAppellationGroup *pAppellation = m_pUserSession->GetAppellationData();
	return GetBitFlag( pAppellation->Appellation, nArrayIndex );
}

void CDNAppellation::AddAppellation( int nArrayIndex )
{
	TAppellationGroup *pAppellation = m_pUserSession->GetAppellationData();
	SetBitFlag( pAppellation->Appellation, nArrayIndex, true );

	m_pUserSession->GetDBConnection()->QueryAddAppellation( m_pUserSession, nArrayIndex );
}

void CDNAppellation::SelectAppellation( int nArrayIndex, int nCoverArrayIndex )
{
	TAppellationGroup *pAppellation = m_pUserSession->GetAppellationData();
	if(( nArrayIndex != -1 ) && ( !IsExistAppellation( nArrayIndex ) )) return;
	if(( nCoverArrayIndex != -1 ) && ( !IsExistAppellation( nCoverArrayIndex ) )) return;


	if (pAppellation->nSelectAppellation != nArrayIndex){
		pAppellation->nSelectAppellation = nArrayIndex;
		m_pUserSession->GetDBConnection()->QuerySelectAppellation(m_pUserSession, nArrayIndex, DBDNWorldDef::AppellationKind::Select);
	}
	if (pAppellation->nCoverAppellation != nCoverArrayIndex){
		pAppellation->nCoverAppellation = nCoverArrayIndex;
		m_pUserSession->GetDBConnection()->QuerySelectAppellation(m_pUserSession, nCoverArrayIndex, DBDNWorldDef::AppellationKind::Cover);
	}
}

int CDNAppellation::GetSelectAppellation()
{
	return m_pUserSession->GetSelectAppellation();
}

int CDNAppellation::GetCoverAppellation()
{
	return m_pUserSession->GetCoverAppellation();
}
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
void CDNAppellation::RequestAddAppellation( int nArrayIndex, int nMailID )
#else
void CDNAppellation::RequestAddAppellation( int nArrayIndex )
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
{
	if( IsExistAppellation( nArrayIndex ) ) return;

	AddAppellation( nArrayIndex );
	m_pUserSession->SendAppellationGain( nArrayIndex );
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
	if(nMailID > 0)
		CDNMailSender::Process(m_pUserSession, nMailID);

	nArrayIndex++;
	TAppellationData* pAppellationData = g_pDataManager->GetAppellationData(nArrayIndex);
	if( !pAppellationData )
		return;
	if(pAppellationData->nUseCollectionBookIndex.empty()) return;
	for(std::vector<int>::iterator ir = pAppellationData->nUseCollectionBookIndex.begin(); ir != pAppellationData->nUseCollectionBookIndex.end();ir++){
		CheckCollectionBook(*ir);
	}
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
}

bool CDNAppellation::OnRecvSelectAppellation( int nArrayIndex, int nCoverArrayIndex )
{
	if ((nArrayIndex != -1) && (!IsExistAppellation(nArrayIndex))) return false;

	if ((nCoverArrayIndex != -1) && (!IsExistAppellation(nCoverArrayIndex))) return false;

	SelectAppellation( nArrayIndex, nCoverArrayIndex );

	m_pUserSession->SendSelectAppellation( nArrayIndex, nCoverArrayIndex );
	BrodcastChangeAppellation();
#if defined( PRE_ADD_NEWCOMEBACK )
	CheckComebackAppellation( nArrayIndex, nCoverArrayIndex );
#endif

	return true;
}
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
void CDNAppellation::CheckCollectionBook(int nTitleID){
	TCollectionBookData *pBookData = g_pDataManager->GetCollectionBookData(nTitleID);
	if (!pBookData) return;
	if (pBookData->nVecAppellationArrayIndex.empty()) return;
	if (IsExistAppellation(pBookData->nRewardAppellationArrayIndex)) return;

	int nCount = 0;
	for ( UINT i = 0; i < pBookData->nVecAppellationArrayIndex.size(); i++){
		if (IsExistAppellation(pBookData->nVecAppellationArrayIndex[i])) nCount++;
	}

	//컬렉션 북에 있는 호칭을 새로 습득한 경우 보상 전송
	if(nCount == pBookData->nVecAppellationArrayIndex.size()){
		RequestAddAppellation(pBookData->nRewardAppellationArrayIndex, pBookData->nMailID[1]);
	}
	else
	{
		//컬렉션북에 조건이 되는 호칭을 얻을때만 호출되는 함수인데 0이 되는 경우 오류. 혹시나 하는 체크용
		if(nCount == 0)
		{
			g_Log.Log(LogType::_FILELOG, m_pUserSession, L"CheckCollectionBook Error!(TitleID : %d)", nTitleID);
			return;
		}
		int nAppletionPercent = (nCount*100)/ (int)(pBookData->nVecAppellationArrayIndex.size());
		int nPreAppletionPercent = ((nCount-1)*100)/ (int)(pBookData->nVecAppellationArrayIndex.size()) ;
		//이전까지는 50%가 안됬다가, 지금 50%가 넘은거라면 메일을 보내준다
		if(nAppletionPercent >= 50 && nPreAppletionPercent < 50)
			CDNMailSender::Process(m_pUserSession, pBookData->nMailID[0]);

		return;
	}
}
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
bool CDNAppellation::OnRecvCollectionBook(int nTitleID)
{
	TCollectionBookData *pBookData = g_pDataManager->GetCollectionBookData(nTitleID);
	if (!pBookData) return false;
	if (pBookData->nVecAppellationArrayIndex.empty()) return false;

	for ( UINT i = 0; i < pBookData->nVecAppellationArrayIndex.size(); i++){
		if ( IsExistAppellation(pBookData->nVecAppellationArrayIndex[i]) == false ) return false;
	}

	RequestAddAppellation(pBookData->nRewardAppellationArrayIndex);

	return true;
}

void CDNAppellation::BrodcastChangeAppellation()
{
#if defined( _VILLAGESERVER )
	m_pUserSession->GetParamData()->nSelectAppellation = GetSelectAppellation();
	m_pUserSession->GetParamData()->nCoverAppellation = GetCoverAppellation();
	m_pUserSession->SendUserLocalMessage( 0, FM_CHANGEAPPELLATION );
#else
	if (!m_pUserSession->GetGameRoom()) return;

	for (DWORD i = 0; i < m_pUserSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pUserSession->GetGameRoom()->GetPartyData(i);
		if( pStruct == NULL ) continue;
		if (pStruct->pSession != m_pUserSession)
			pStruct->pSession->SendChangeAppellation(m_pUserSession->GetSessionID(), GetSelectAppellation(), GetCoverAppellation());
	}
#endif
}

void CDNAppellation::SetPCBang()
{
	switch (m_pUserSession->GetPCBangGrade())
	{
	case PCBang::Grade::Normal:
		{
			SetPCbangAppellation(false);
		}
		break;

	case PCBang::Grade::Premium:
	case PCBang::Grade::Gold:
	case PCBang::Grade::Silver:
	case PCBang::Grade::Red:
		{
			ResetPCbangAppellation(false);
			SetPCbangAppellation(false);

			CheckAppellation();
		}
		break;

	default:
		{
			ResetPCbangAppellation(true);
		}
		break;
	}
}

void CDNAppellation::AddPeriodAppellation(int nArrayID, __time64_t tExpireDate)
{
	if(nArrayID > -1)
		m_mPeriodAppellation.insert( std::make_pair(nArrayID,tExpireDate) );
		
	return;
}

void CDNAppellation::SetPeriodAppellation(char *Appellation, std::map<int, int> &mapDelAppellation)
{
	for(std::map<int, __time64_t>::iterator itor = m_mPeriodAppellation.begin(); itor != m_mPeriodAppellation.end(); itor++)
	{
		if( itor->first > -1 )
		{
			SetBitFlag( Appellation, itor->first, true );
			std::map<int, int>::iterator itordel = mapDelAppellation.find(itor->first);
			if(itordel != mapDelAppellation.end())
				mapDelAppellation.erase(itordel);
		}
	}
	return;
}

void CDNAppellation::SendPeriodAppellationTime(int nArrayID, __time64_t tExpireDate, bool bAllSend)
{
	BYTE count = 0;
	TAppellationTime AppellationTime[PERIODAPPELLATIONMAX];
	memset(AppellationTime, 0, sizeof(AppellationTime));	
	if(bAllSend)
	{
		for(std::map<int, __time64_t>::iterator itor = m_mPeriodAppellation.begin(); itor != m_mPeriodAppellation.end(); itor++)
		{
			AppellationTime[count].nArrayID = itor->first;
			AppellationTime[count].tExpireDate = itor->second;
			count++;
		}		
	}
	else
	{
		AppellationTime[count].nArrayID = nArrayID;
		AppellationTime[count].tExpireDate = tExpireDate;
		count++;
	}
	m_pUserSession->SendAppellationTime(count, AppellationTime);
}

bool CDNAppellation::CheckAppellation()
{	
	TAppellationGroup *pAppellation = m_pUserSession->GetAppellationData();			

	if (pAppellation->nSelectAppellation != -1)
	{
		if (IsExistAppellation(pAppellation->nSelectAppellation) == false)
		{
			if(g_pDataManager->CheckPcbangAppellation(pAppellation->nSelectAppellation))
			{
				//피씨방칭호 초기화
				m_bResetPcbangAppellation = true;
			}

			SelectAppellation(-1, -1);			
			return true;
		}
	}
	if (pAppellation->nCoverAppellation != -1)
	{
		if (IsExistAppellation(pAppellation->nCoverAppellation) == false)
		{	
			if(g_pDataManager->CheckPcbangAppellation(pAppellation->nCoverAppellation))
			{
				//피씨방칭호 초기화
				m_bResetPcbangAppellation = true;
			}

			SelectAppellation(-1, -1);
			return true;
		}
	}
	return false;
}
void CDNAppellation::SendDelAppellation(int ArrayIndex)
{
	//칭호 제거
	m_pUserSession->GetDBConnection()->QueryDelAppellation(m_pUserSession, ArrayIndex);
	return;
}

void CDNAppellation::SetPCbangAppellation(bool bSend/* = false*/)
{
	TPCBangData *pData = g_pDataManager->GetPCBangData(PCBang::Type::Appellation);
	if (!pData) return;

	std::vector<TPCBangParam> Param;
	Param.clear();

	switch (m_pUserSession->GetPCBangGrade())
	{
	case PCBang::Grade::Normal:
		{
			Param = pData->VecPCBangParam;
		}
		break;

	case PCBang::Grade::Premium:
		{
			Param = pData->VecPremiumPCBangParam;
		}
		break;

	case PCBang::Grade::Gold:
		{
			Param = pData->VecGoldPCBangParam;
		}
		break;

	case PCBang::Grade::Silver:
		{
			Param = pData->VecSilverPCBangParam;
		}
		break;

	case PCBang::Grade::Red:
		{
			Param = pData->VecRedPCBangParam;
		}
		break;

	default:
		break;
	}

	if (!Param.empty()){
		for (int i = 0; i < (int)Param.size(); i++){
			TAppellationData *pAData = g_pDataManager->GetAppellationData(Param[i].nParam[0]);

			if ((m_pUserSession->GetLevel() >= pAData->nLevelLimit) && (IsExistAppellation(pAData->nArrayIndex)) == false){
				AddAppellation(pAData->nArrayIndex);

				if (bSend == true)
					m_pUserSession->SendAppellationGain(pAData->nArrayIndex);
			}
		}

		if (bSend == false)
			CheckAppellation();
	}
}

void CDNAppellation::ResetPCbangAppellation(bool bCheck)
{
	TPCBangData *pData = g_pDataManager->GetPCBangData(PCBang::Type::Appellation);
	if (!pData) return;

	bool bFlag = false;
	if (m_pUserSession->GetPCBangGrade() != PCBang::Grade::Normal)
		SendDelPCBangAppellation(pData->VecPCBangParam);

	if (m_pUserSession->GetPCBangGrade() != PCBang::Grade::Premium)
		SendDelPCBangAppellation(pData->VecPremiumPCBangParam);

	if (m_pUserSession->GetPCBangGrade() != PCBang::Grade::Gold)
		SendDelPCBangAppellation(pData->VecGoldPCBangParam);

	if (m_pUserSession->GetPCBangGrade() != PCBang::Grade::Silver)
		SendDelPCBangAppellation(pData->VecSilverPCBangParam);

	if (m_pUserSession->GetPCBangGrade() != PCBang::Grade::Red)
		SendDelPCBangAppellation(pData->VecRedPCBangParam);

	if (bCheck)
		CheckAppellation();
}

void CDNAppellation::SendDelPCBangAppellation(std::vector<TPCBangParam> &VecPCBangParam)
{
	if (!VecPCBangParam.empty()){
		for (int i = 0; i < (int)VecPCBangParam.size(); i++){
			int nArrayIndex = g_pDataManager->GetAppellationArrayIndex(VecPCBangParam[i].nParam[0]);
			if (IsExistAppellation(nArrayIndex) == true){
				SetBitFlag(m_pUserSession->GetAppellationData()->Appellation, nArrayIndex, false);
				SendDelAppellation(nArrayIndex);
			}
		}
	}
}

#if defined( PRE_ADD_BESTFRIEND )
void CDNAppellation::DelAppellation( AppellationType::Type::eCode eType, bool bCheckAppellation  )
{
	std::map<int, int> mapDelAppellation;
	mapDelAppellation.clear();
	TAppellationGroup *pAppellation = m_pUserSession->GetAppellationData();		

	switch(eType)
	{
	case AppellationType::Type::Period:
		{
			g_pDataManager->ResetPeriodAppellation(pAppellation->Appellation, mapDelAppellation);
			
		}
		break;
	case AppellationType::Type::Guild:
		{
			g_pDataManager->ResetGuildAppellation(pAppellation->Appellation, mapDelAppellation);
		}
		break;
	case AppellationType::Type::BestFriend:
		{
			g_pDataManager->ResetBestFriendAppellation(pAppellation->Appellation, mapDelAppellation);
		}
		break;
	default:
			return;
	}

	for( std::map<int, int>::iterator itor = mapDelAppellation.begin(); itor != mapDelAppellation.end(); itor++ )
		SendDelAppellation(itor->first);	//칭호 삭제	
	//선택된 칭호가 존재하는지 체크(기간제 칭호의 경우 사라질수 있기 때문에)
	if(bCheckAppellation)
	{		
		if( CheckAppellation() )
		{
			m_pUserSession->SendAppellationList(pAppellation->Appellation);
			OnRecvSelectAppellation( pAppellation->nSelectAppellation, pAppellation->nCoverAppellation );	
		}		
	}
}
#endif

#if defined( PRE_ADD_NEWCOMEBACK )
void CDNAppellation::CheckComebackAppellation( int nArrayIndex, int nCoverArrayIndex )
{
	if( nArrayIndex != -1 )
	{
		nArrayIndex++;
		TAppellationData *pAData = g_pDataManager->GetAppellationData(nArrayIndex);
		if( pAData )
		{
			if( pAData->nType == AppellationType::Type::ComeBack )
			{
				m_pUserSession->SetComebackAppellation(nArrayIndex);
				return;
			}
		}
	}
	if( nCoverArrayIndex != -1)
	{
		nCoverArrayIndex++;
		TAppellationData *pAData = g_pDataManager->GetAppellationData(nCoverArrayIndex);
		if( pAData )
		{
			if( pAData->nType == AppellationType::Type::ComeBack )
			{
				m_pUserSession->SetComebackAppellation(nCoverArrayIndex);
				return;
			}
		}
	}
	if( m_pUserSession->GetComebackAppellation() )
	{
		m_pUserSession->SetComebackAppellation(0);
	}
}
#endif