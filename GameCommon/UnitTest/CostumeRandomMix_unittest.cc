
#include "stdafx.h"
#include "EtUIXML.h"
#include "DNAuthManager.h"
#include "DNUserSession.h"
#include "DNGameDataManager.h"
#include "DnCostumeRandomMixDataMgr.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
#if defined( PRE_ADD_COSRANDMIX )

extern CEtUIXML* g_pUIXML;
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
extern CEtExceptionalUIXML*	g_pExceptionalUIXML;
#endif

class CostumeRandomMix_unittest:public testing::Test
{
#define MAX_TEST_CLASS_KIND 5
#define RANDOM_TEST_COUNT 100
#define PROBABILITY_TEST_RANK_COUNT 5
#define PROBABILITY_TEST_COUNT 10000

protected:
	static void SetUpTestCase()
	{
		g_pUIXML = new CEtUIXML;
		CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring.xml" );
		g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1 );
		SAFE_DELETE( pStream );

		g_pAuthManager = new CDNAuthManager;
		g_pAuthManager->Init();

#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
		g_pExceptionalUIXML = new CEtExceptionalUIXML;
		if( g_pExceptionalUIXML )
			g_pExceptionalUIXML->LoadXML( "uistring_exception.xml" );
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING

		g_pDataManager = new CDNGameDataManager();
		g_pDataManager->LoadItemData();
		g_pDataManager->LoadPartData();
	}

	static void TearDownTestCase()
	{
		SAFE_DELETE( g_pDataManager );
		SAFE_DELETE( g_pUIXML );
		SAFE_DELETE( g_pAuthManager );
	}

	virtual void SetUp()
	{
		m_Random.srand(timeGetTime());
		m_pSession = new CDNUserSession;

		m_pMgr = const_cast<CDnCostumeRandomMixDataMgr*>(&g_pDataManager->GetCosRandomMixDataMgr());

		EXPECT_TRUE(g_pDataManager->LoadCostumeRandomMixData());
	}

	virtual void TearDown()
	{
		SAFE_DELETE(m_pSession);
	}

	CDNUserSession*	m_pSession;
	CMtRandom m_Random;
	CDnCostumeRandomMixDataMgr* m_pMgr;
};

TEST_F( CostumeRandomMix_unittest, RANDOMTEST )
{
	std::vector<int> enableList[CASHEQUIPMAX];

	typedef std::map<int, CDnCostumeRandomMixDataMgr::SRandomMixEnableData> MIXENABLELIST;
	MIXENABLELIST& enableListAll = m_pMgr->m_CosRandomMixEnableList;
	MIXENABLELIST::const_iterator iter = enableListAll.begin();
	for (; iter != enableListAll.end(); ++iter)
	{
		int stuffItemId = (*iter).first;
		const TPartData* pData = g_pDataManager->GetPartData(stuffItemId);
		if (pData == NULL)
		{
			EXPECT_FALSE(true) << "There is no stuffItemId:" << stuffItemId;
		}
		else
		{
			EXPECT_TRUE(pData->nParts >= CASHEQUIPMIN && pData->nParts < CASHEQUIPMAX);
			EXPECT_FALSE(std::find(enableList[pData->nParts].begin(), enableList[pData->nParts].end(), stuffItemId) != enableList[pData->nParts].end());

			enableList[pData->nParts].push_back(stuffItemId);
		}
	}

	FILE* pStream = NULL;
	struct tm* now=NULL;
	time_t systemTime;
	time(&systemTime);  
	now=localtime(&systemTime);
	wchar_t szTail[MAX_PATH] = {0,};

	wsprintfW(szTail, L"CostumeRandomMixTest_EnableItemList_%02d%02d%02d_%02d%02d%02d.txt", 1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	EXPECT_FALSE(_wfopen_s(&pStream, szTail, L"wb") != 0);

	if (pStream)
	{
		fseek(pStream, 0L, SEEK_SET);

		WORD mark = 0xFEFF;
		fwrite(&mark, sizeof(WORD), 1, pStream);
	}

	const wchar_t* pCashPartsName[CASHEQUIP_FOOT + 1] = { L"HELMET", L"BODY", L"LEG", L"HAND", L"FOOT" };

	EXPECT_FALSE(pStream == NULL);

	fwprintf(pStream, L"===================== Enable Item List =====================\n");
	int i = 0;
	for (; i <= CASHEQUIP_FOOT; ++i)
	{
		fwprintf(pStream, L"\nPARTS - %s\n\n", pCashPartsName[i]);
		
		int j = 0;
		for (; j < (int)enableList[i].size(); ++j)
		{
			const int& itemId = enableList[i][j];

			const TItemData* pData = g_pDataManager->GetItemData(itemId);
			if (pData == NULL)
			{
				fwprintf(pStream, L"(%d) No ItemData!!\n", itemId);
			}

			fwprintf(pStream, L"(%d) %s\n", itemId, pData->wszItemName);
		}

		fwprintf(pStream, L"\n");
	}

	fclose(pStream);

	pStream = NULL;
	wsprintfW(szTail, L"CostumeRandomMixTest_MixResultList_%02d%02d%02d_%02d%02d%02d.txt", 1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	EXPECT_FALSE(_wfopen_s(&pStream, szTail, L"wb") != 0);

	int classID = 1;
	fwprintf(pStream, L"CLASSID SPECIFICATION : 1(Warrior) 2(Archer) 3(Sorceress) 4(Cleric) 5(Academic) 6(kali)\n");
	for (; classID <= MAX_TEST_CLASS_KIND; ++classID)
	{
		fwprintf(pStream, L"\nCLASSID : (%d)\n", classID);
		int i = CASHEQUIPMIN;
		for (; i <= CASHEQUIP_FOOT; ++i)
		{
			std::vector<int>& curEnableList = enableList[i];
			if (curEnableList.size() <= 0)
			{
				EXPECT_FALSE(true) << "There is NO Equip List eCashEquipType: " << i;
				return;
			}

			std::vector<int>::const_iterator iter = curEnableList.begin();
			int j = 0, noCount = 0;
			for (; j <= RANDOM_TEST_COUNT;)
			{
				std::vector<TItem> stuffs;
				int k = 0;
				bool bTestOK = true;
				for (; k < MAX_COSTUME_RANDOMMIX_STUFF; ++k)
				{
					int curStuffIndex = m_Random.rand(0, (int)curEnableList.size() - 1);
					int itemId = curEnableList[curStuffIndex];

					if (bTestOK && g_pDataManager->IsItemNeedJob(itemId, classID) == false)
						bTestOK = false;

					TItem stuffItem;
					if (CDNUserItem::MakeItemStruct(curEnableList[curStuffIndex], stuffItem) == false)
					{
						EXPECT_FALSE(true);
						return;
					}
					
					stuffs.push_back(stuffItem);
				}

				if (bTestOK)
				{
					bool bEnableMix = m_pMgr->IsEnableCostumeRandomMixStuff(stuffs, classID);
					EXPECT_TRUE(bEnableMix);
					if (bEnableMix == false)
						continue;
				}
				else
				{
					EXPECT_FALSE(m_pMgr->IsEnableCostumeRandomMixStuff(stuffs, classID));
					noCount++;
					continue;
				}

				eError lastEror;
				int resultItemId = m_pMgr->DoRandomMix(stuffs, classID, *(m_pSession->GetItem()), 0, lastEror);
				EXPECT_TRUE(resultItemId);

				if (resultItemId != 0)
				{
					std::wstring resultItemName;
					const TItemData* pData = g_pDataManager->GetItemData(resultItemId);
					if (pData == NULL)
						resultItemName = L"Result NO ItemData!";
					else
						resultItemName = pData->wszItemName;

					int rank = m_pMgr->GetRandomMixStuffItemRank(stuffs[0].nItemID);
					const TItemData* pStuffData = g_pDataManager->GetItemData(stuffs[0].nItemID);
					if (pStuffData)
						fwprintf(pStream, L"(%02d:%10d)%s + ", rank, stuffs[0].nItemID, pStuffData->wszItemName);
					else
						fwprintf(pStream, L"(%02d:%10d) + ", rank, stuffs[0].nItemID);

					for (k = 1; k < MAX_COSTUME_RANDOMMIX_STUFF - 1; ++k)
					{
						int rank = m_pMgr->GetRandomMixStuffItemRank(stuffs[k].nItemID);
						const TItemData* pStuffData = g_pDataManager->GetItemData(stuffs[k].nItemID);
						if (pStuffData)
							fwprintf(pStream, L"(%02d:%10d)%s + ", rank, stuffs[k].nItemID, pStuffData->wszItemName);
						else
							fwprintf(pStream, L"(%02d:%10d) + ", rank, stuffs[k].nItemID);
					}

					rank = m_pMgr->GetRandomMixStuffItemRank(stuffs[MAX_COSTUME_RANDOMMIX_STUFF - 1].nItemID);
					pStuffData = g_pDataManager->GetItemData(stuffs[MAX_COSTUME_RANDOMMIX_STUFF - 1].nItemID);
					if (pStuffData)
						fwprintf(pStream, L"(%02d:%10d)%s", rank, stuffs[MAX_COSTUME_RANDOMMIX_STUFF - 1].nItemID, pStuffData->wszItemName);
					else
						fwprintf(pStream, L"(%02d:%10d)", rank, stuffs[MAX_COSTUME_RANDOMMIX_STUFF - 1].nItemID);

					rank = m_pMgr->GetRandomMixResultItemRank(resultItemId);
					if (pData == NULL)
						fwprintf(pStream, L" = (%02d:%10d)\n", rank, resultItemId);
					else
						fwprintf(pStream, L" = (%02d:%10d) %s\n", rank, resultItemId, pData->wszItemName);

					++j;
				}
			}
		}
	}

	fclose(pStream);
}

TEST_F( CostumeRandomMix_unittest, PROBABILITY_TEST )
{
	std::vector<int> enableList[MAX_TEST_CLASS_KIND + 1][CASHEQUIPMAX][PROBABILITY_TEST_RANK_COUNT];

	typedef std::map<int, CDnCostumeRandomMixDataMgr::SRandomMixEnableData> MIXENABLELIST;
	MIXENABLELIST& enableListAll = m_pMgr->m_CosRandomMixEnableList;
	MIXENABLELIST::const_iterator iter = enableListAll.begin();
	for (; iter != enableListAll.end(); ++iter)
	{
		int stuffItemId = (*iter).first;
		const CDnCostumeRandomMixDataMgr::SRandomMixEnableData& rddata = (*iter).second;
		const TPartData* pData = g_pDataManager->GetPartData(stuffItemId);
		if (pData == NULL)
		{
			EXPECT_FALSE(true) << "There is no stuffItemId:" << stuffItemId;
		}
		else
		{
			bool bIsValidRank = (rddata.nRank >= 0 && rddata.nRank < PROBABILITY_TEST_RANK_COUNT);
			EXPECT_TRUE(bIsValidRank);

			int i = 1;
			for (; i <= MAX_TEST_CLASS_KIND; ++i)
			{
				if (g_pDataManager->IsItemNeedJob(stuffItemId, i) && bIsValidRank)
					enableList[i][pData->nParts][rddata.nRank].push_back(stuffItemId);
			}
		}
	}

	FILE* pStream = NULL;
	struct tm* now=NULL;
	time_t systemTime;
	time(&systemTime);  
	now=localtime(&systemTime);
	wchar_t szTail[MAX_PATH] = {0,};

	wsprintfW(szTail, L"CostumeRandomMixTest_ProbabilityTest_%02d%02d%02d_%02d%02d%02d.txt", 1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	EXPECT_FALSE(_wfopen_s(&pStream, szTail, L"wb") != 0);

	if (pStream)
	{
		fseek(pStream, 0L, SEEK_SET);

		WORD mark = 0xFEFF;
		fwrite(&mark, sizeof(WORD), 1, pStream);
	}

	const wchar_t* pCashPartsName[CASHEQUIP_FOOT + 1] = { L"HELMET", L"BODY", L"LEG", L"HAND", L"FOOT" };

	EXPECT_FALSE(pStream == NULL);

	int classID = 1;
	fwprintf(pStream, L"CLASSID SPECIFICATION : 1(Warrior) 2(Archer) 3(Sorceress) 4(Cleric) 5(Academic) 6(kali)\n");
	for (; classID <= MAX_TEST_CLASS_KIND; ++classID)
	{
		fwprintf(pStream, L"\nCLASSID : (%d)\n", classID);
		int i = CASHEQUIPMIN;
		for (; i <= CASHEQUIP_FOOT; ++i)
		{
			fwprintf(pStream, L"PARTS - %s\n", pCashPartsName[i]);
			int j = 0;
			for (; j < PROBABILITY_TEST_RANK_COUNT; ++j)
			{
				fwprintf(pStream, L"RANK(%d) x RANK(%d)\n", j, j);
				const std::vector<int>& curEnableList = enableList[classID][i][j];
				if (curEnableList.size() <= 0)
				{
					fwprintf(pStream, L"No ITEMS\n\n");
					//EXPECT_FALSE(true) << "There is no stuffs[class:%d][equip:%d][rank:%d]" << classID << i << j;
					continue;
				}

				int k = 0;
				std::vector<TItem> stuffs;
				int rankCount[PROBABILITY_TEST_RANK_COUNT] = {0,};
				for (; k < PROBABILITY_TEST_COUNT; ++k)
				{
					stuffs.clear();

					for (int l = 0; l < MAX_COSTUME_RANDOMMIX_STUFF; ++l)
					{
						int curIdx = m_Random.rand(0, (int)curEnableList.size() - 1);
						const int& stuffItemId = curEnableList[curIdx];

						TItem stuffItem;
						if (CDNUserItem::MakeItemStruct(stuffItemId, stuffItem) == false)
						{
							EXPECT_FALSE(true);
							continue;
						}

						stuffs.push_back(stuffItem);
					}

					if (stuffs.size() != MAX_COSTUME_RANDOMMIX_STUFF)
					{
						EXPECT_FALSE(true) << "There is no stuffs[class:%d][equip:%d][rank:%d]" << classID << i << j;
						continue;
					}

					if (m_pMgr->IsEnableCostumeRandomMixStuff(stuffs, classID) == false)
					{
						//EXPECT_FALSE(true) << "There is no ENABLE stuffs[class:%d][equip:%d][rank:%d]" << classID << i << j;
						continue;
					}

					eError lastEror;
					int resultItemId = m_pMgr->DoRandomMix(stuffs, classID, *(m_pSession->GetItem()), 0, lastEror);
					int resultRank = m_pMgr->GetRandomMixResultItemRank(resultItemId);

					if (resultRank >= 0 && resultRank < PROBABILITY_TEST_RANK_COUNT)
						rankCount[resultRank]++;
					else
						EXPECT_FALSE(true) << "Result Rank Error resultItemId:%d [class:%d][equip:%d][rank:%d]" << resultItemId << classID << i << j;
				}

				fwprintf(pStream, L"RESULT in %d count\n", PROBABILITY_TEST_COUNT);
				for (int m = 0; m < PROBABILITY_TEST_RANK_COUNT; ++m)
				{
					fwprintf(pStream, L"rank_%d : %d ", m, rankCount[m]);
				}

				fwprintf(pStream, L"\n\n");
			}
		}
	}

	fclose(pStream);
}

TEST_F( CostumeRandomMix_unittest, PROBABILITY_TEST2 )
{
	std::vector<int> enableList[MAX_TEST_CLASS_KIND + 1][CASHEQUIPMAX][PROBABILITY_TEST_RANK_COUNT];

	typedef std::map<int, CDnCostumeRandomMixDataMgr::SRandomMixEnableData> MIXENABLELIST;
	MIXENABLELIST& enableListAll = m_pMgr->m_CosRandomMixEnableList;
	MIXENABLELIST::const_iterator iter = enableListAll.begin();
	for (; iter != enableListAll.end(); ++iter)
	{
		int stuffItemId = (*iter).first;
		const CDnCostumeRandomMixDataMgr::SRandomMixEnableData& rddata = (*iter).second;
		const TPartData* pData = g_pDataManager->GetPartData(stuffItemId);
		if (pData == NULL)
		{
			EXPECT_FALSE(true) << "There is no stuffItemId:" << stuffItemId;
		}
		else
		{
			bool bIsValidRank = (rddata.nRank >= 0 && rddata.nRank < PROBABILITY_TEST_RANK_COUNT);
			EXPECT_TRUE(bIsValidRank);

			int i = 1;
			for (; i <= MAX_TEST_CLASS_KIND; ++i)
			{
				if (g_pDataManager->IsItemNeedJob(stuffItemId, i) && bIsValidRank)
					enableList[i][pData->nParts][rddata.nRank].push_back(stuffItemId);
			}
		}
	}

	FILE* pStream = NULL;
	struct tm* now=NULL;
	time_t systemTime;
	time(&systemTime);  
	now=localtime(&systemTime);
	wchar_t szTail[MAX_PATH] = {0,};

	wsprintfW(szTail, L"CostumeRandomMixTest_ProbabilityTest2_%02d%02d%02d_%02d%02d%02d.txt", 1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	EXPECT_FALSE(_wfopen_s(&pStream, szTail, L"wb") != 0);

	if (pStream)
	{
		fseek(pStream, 0L, SEEK_SET);

		WORD mark = 0xFEFF;
		fwrite(&mark, sizeof(WORD), 1, pStream);
	}

	const wchar_t* pCashPartsName[CASHEQUIP_FOOT + 1] = { L"HELMET", L"BODY", L"LEG", L"HAND", L"FOOT" };

	EXPECT_FALSE(pStream == NULL);

	int classID = 1;
	fwprintf(pStream, L"CLASSID SPECIFICATION : 1(Warrior) 2(Archer) 3(Sorceress) 4(Cleric) 5(Academic) 6(kali)\n");
	for (; classID <= MAX_TEST_CLASS_KIND; ++classID)
	{
		fwprintf(pStream, L"\nCLASSID : (%d)\n", classID);
		int i = CASHEQUIPMIN;
		for (; i <= CASHEQUIP_FOOT; ++i)
		{
			fwprintf(pStream, L"PARTS - %s\n", pCashPartsName[i]);

			// todo by kalliste
		}
	}

	fclose(pStream);
}

TEST_F(CostumeRandomMix_unittest, PROBABILITY_TEST3)
{
	int classID = 1;
	int stuffItemId[2] = {0,};
	stuffItemId[0] = 1073765120;
	stuffItemId[1] = 1073765121;

	std::vector<TItem> stuffs;
	for (int l = 0; l < 2; ++l)
	{
		TItem stuffItem;
		if (CDNUserItem::MakeItemStruct(stuffItemId[l], stuffItem) == false)
		{
			EXPECT_FALSE(true);
			continue;
		}

		stuffs.push_back(stuffItem);
	}

	FILE* pStream = NULL;
	struct tm* now=NULL;
	time_t systemTime;
	eError lastEror;

	time(&systemTime);  
	now=localtime(&systemTime);
	wchar_t szTail[MAX_PATH] = {0,};
	wsprintfW(szTail, L"CostumeRandomMixTest_ProbabilityTest3_%02d%02d%02d_%02d%02d%02d.txt", 1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	EXPECT_FALSE(_wfopen_s(&pStream, szTail, L"wb") != 0);

	int ranks[10] = {0,};
	for (int m = 0; m < 1000000; ++m)
	{
		int resultItemId = m_pMgr->DoRandomMix(stuffs, classID, *(m_pSession->GetItem()), 0, lastEror);
		int resultRank = m_pMgr->GetRandomMixResultItemRank(resultItemId);

		EXPECT_FALSE(resultRank < 0 || resultRank >= 10);
		ranks[resultRank]++;
	}

	for (int n = 0; n < 10; ++n)
	{
		fwprintf(pStream, L"\nRank%d: (%d)\n", n, ranks[n]);
	}
}

#endif // #if defined( PRE_ADD_COSRANDMIX )
#endif // #if !defined( _FINAL_BUILD )
