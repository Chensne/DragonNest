#include "StdAfx.h"
#include "DNField.h"
#include "DNFieldBlock.h"
#include "DNFieldData.h"
#include "DNNpcObject.h"
#include "DNNpcObjectManager.h"
#include "DNGameDataManager.h"
#include "Log.h"

CDNField::CDNField(void): m_pMapInfo(NULL), m_pFieldData(NULL), m_pNpcObjectManager(NULL)
{
	m_nChannelID = m_nMapIndex= m_nChannelAtt = m_nWidth = m_nHeight = m_nWidthNo = m_nHeightNo = 0;
	// m_nMapArrayIndex = 0;
	m_nMinX = m_nMinZ = m_nMaxX = m_nMaxZ = 0;

	m_FieldBlocks.clear();

	m_pNpcObjectManager = new CDNNpcObjectManager;
	m_nPermitLevel = -1;
	m_nChannelMaxUser = 0;
}

CDNField::~CDNField(void)
{
	SAFE_DELETE(m_pNpcObjectManager);
	Final();
}

bool CDNField::Init(int nChannelID, int nChannelAtt, int nChannelMaxUser, int nMapIndex, const TMapInfo *pMapInfo, CDNFieldData *pFieldData, int nPermitLevel)
{
	if ((!pMapInfo) || (nChannelID <= 0) || (nMapIndex <= 0)) return false;
	Final();

	m_nChannelID = nChannelID;
	m_nChannelAtt = nChannelAtt;
	m_nChannelMaxUser = nChannelMaxUser;
	m_nMapIndex = nMapIndex;
	m_nPermitLevel = nPermitLevel;
	
	// m_nMapArrayIndex = 0;
	m_pMapInfo = const_cast<TMapInfo*>(pMapInfo);
	m_pFieldData = pFieldData;

	m_pFieldData->GetWidthHeight(m_nWidth, m_nHeight);

	int Width = m_pFieldData->GetGridX() * m_nWidth / 2;	// 미터라 100을 곱해야한다. 나누기 2를 하여 똑같이 -, + 를 한다.
	int Height = m_pFieldData->GetGridY() * m_nHeight / 2;

	m_nMinX = -Width;
	m_nMinZ = -Height;
	m_nMaxX = Width;
	m_nMaxZ = Height;

	m_nWidthNo = (UINT)(m_nWidth / FIELDBLOCKRANGE);
	if (m_nWidth % FIELDBLOCKRANGE > 0) 
		m_nWidthNo++;

	m_nHeightNo = (UINT)(m_nHeight / FIELDBLOCKRANGE);
	if (m_nHeight % FIELDBLOCKRANGE > 0)
		m_nHeightNo++;

	CDNFieldBlock *pBlock = NULL;
	for (UINT i = 0; i < m_nHeightNo; i++){
		for (UINT j = 0; j < m_nWidthNo; j++){
			pBlock = new CDNFieldBlock;
			if (!pBlock) return false;

			pBlock->Init(j, i, m_nMinX + (j * FIELDBLOCKRANGE), m_nMinZ + (i * FIELDBLOCKRANGE));
			m_FieldBlocks.push_back(pBlock);
		}
	}

	return true;
}

bool CDNField::Final()
{
	if (m_FieldBlocks.empty()) return false;

	CDNFieldBlock *pBlock = NULL;
	for (int i = 0; i < (int)m_FieldBlocks.size(); i++){
		pBlock = m_FieldBlocks[i];
		SAFE_DELETE(pBlock);
	}

	m_FieldBlocks.clear();

	return true;
}
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
void CDNField::GetWaitUserList( std::list<CDNUserSession*>& UserList, int iLimitLevel/*=-1*/, BYTE cPvPChannelType/* = static_cast<BYTE>(PvPCommon::RoomType::max)*/ )
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
void CDNField::GetWaitUserList( std::list<CDNUserSession*>& UserList, int iLimitLevel/*=-1*/ )
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
{
	for( UINT i=0; i <m_FieldBlocks.size() ; ++i)
	{
		if( !m_FieldBlocks[i] )
			continue;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		m_FieldBlocks[i]->GetWaitUserList( UserList, iLimitLevel, cPvPChannelType );
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		m_FieldBlocks[i]->GetWaitUserList( UserList, iLimitLevel );
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	}	
}

bool CDNField::EnterObject(CDNBaseObject *pObj)
{
	CDNFieldBlock *pBlock = GetFieldBlock(&pObj->GetCurrentPos());
	if (!pBlock){
		if( !bIsPvPLobby() )
			g_Log.Log(LogType::_ERROR, L"[ObjUID:%u] FieldBlock NULL!! (%d,%d,%d)\r\n", pObj->GetObjectID(), pObj->GetCurrentPos().nX, pObj->GetCurrentPos().nY, pObj->GetCurrentPos().nZ);
		return false;
	}
	
	return pBlock->AddBaseObject(pObj);
}

bool CDNField::LeaveObject(CDNBaseObject *pObj)
{
	CDNFieldBlock *pBlock = GetFieldBlock(&pObj->GetCurrentPos());
	if (!pBlock){
		if( !bIsPvPLobby() )
			g_Log.Log(LogType::_ERROR, L"[ObjUID:%u] FieldBlock NULL!! (%d,%d,%d)\r\n", pObj->GetObjectID(), pObj->GetCurrentPos().nX, pObj->GetCurrentPos().nY, pObj->GetCurrentPos().nZ);
		return false;
	}

	return pBlock->DelBaseObject(pObj);
}

void CDNField::FinalUser( CDNBaseObject* pObj )
{
	for( UINT i=0 ; i<m_FieldBlocks.size() ; ++i )
	{
		m_FieldBlocks[i]->DelBaseObject( pObj );
	}
}

int CDNField::SendFieldMessage(CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData)
{
	int cursxp, curszp, curexp, curezp;
	int targetsxp, targetszp, targetexp, targetezp;
	int i, j;

	switch(wMsg)
	{
	case FM_CREATE:
		if (!EnterObject(pSender)){
			g_Log.Log(LogType::_ERROR, L"[CDNField::SendMessage] EnterObject Fail!!\r\n");
			return -1;
		}

		pSender->FieldProcess(pSender, FM_SHOW, pSenderData, pParamData);

		GetViewRange(&pSender->GetCurrentPos(), cursxp, curszp, curexp, curezp);
		for (i = curszp; i <= curezp; i++){
			for (j = cursxp; j <= curexp; j++){
				m_FieldBlocks[(i * m_nWidthNo) + j]->SendFieldMessage(pSender, wMsg, pSenderData, pParamData);
			}
		}
		break;

	case FM_DESTROY:
		GetViewRange(&pSender->GetCurrentPos(), cursxp, curszp, curexp, curezp);
		for (i = curszp; i <= curezp; i++){
			for (j = cursxp; j <= curexp; j++){
				m_FieldBlocks[(i * m_nWidthNo) + j]->SendFieldMessage(pSender, wMsg, pSenderData, pParamData);
			}
		}

		pSender->FieldProcess(pSender, FM_HIDE, pSenderData, pParamData);
		if (!LeaveObject(pSender)){
			g_Log.Log(LogType::_ERROR, L"[CDNField::SendMessage] LeaveObject Fail!!\r\n");
			return -1;
		}
		break;

	case FM_MOVE:
		CDNFieldBlock *pCurBlock, *pTargetBlock;

		pCurBlock = GetFieldBlock(&pSender->GetCurrentPos());
		pTargetBlock = GetFieldBlock(&pSender->GetTargetPos());

		if (pCurBlock && pTargetBlock){
			GetViewRange(&pSender->GetCurrentPos(), cursxp, curszp, curexp, curezp);
			GetViewRange(&pSender->GetTargetPos(), targetsxp, targetszp, targetexp, targetezp);

			if (cursxp > targetsxp) cursxp = targetsxp;
			if (curszp > targetszp) curszp = targetszp;
			if (curexp < targetexp) curexp = targetexp;
			if (curezp < targetezp) curezp = targetezp;

			for (i = curszp; i <= curezp; i++){
				for (j = cursxp; j <= curexp; j++){
					m_FieldBlocks[(i * m_nWidthNo) + j]->SendFieldMessage(pSender, wMsg, pSenderData, pParamData);
				}
			}

			pSender->SwapPosition();

			if (pCurBlock != pTargetBlock){
				if (pCurBlock->DelBaseObject(pSender) == false)
					_DANGER_POINT();		//필드블럭쪽에서 먼가 자꾸 죽어서 남겨보옴
				pTargetBlock->AddBaseObject(pSender);
			}
		}
		break;
	case FM_GUILDWARWINSKILL :
		{
			// 시야 2배 짜리로 보냄
			GetDoubleViewRange(&pSender->GetCurrentPos(), cursxp, curszp, curexp, curezp);
			for (i = curszp; i <= curezp; i++)
			{
				for (j = cursxp; j <= curexp; j++)
				{					
					m_FieldBlocks[(i * m_nWidthNo) + j]->SendFieldMessage(pSender, wMsg, pSenderData, pParamData);
				}
			}
		}
		break;
	}

	return 1;
}

void CDNField::DoUpdate(DWORD CurTick)
{

}

CDNFieldBlock* CDNField::GetFieldBlock(TPosition *pPos)
{
	if (!pPos) return NULL;

	int CurPosX, CurPosZ;
	UINT X, Z, XPoint, ZPoint, Index;

	CurPosX = (int)(pPos->nX / 1000);
	CurPosZ = (int)(pPos->nZ / 1000);
	if (CurPosX < m_nMinX || CurPosX > m_nMaxX || CurPosZ < m_nMinZ || CurPosZ > m_nMaxZ) 
	{
		if( bIsPvPLobby() )
		{
			if( m_FieldBlocks.size() > 0 )
				return m_FieldBlocks[0];
		}
		return NULL;
	}

	X = (UINT)(CurPosX - m_nMinX);
	Z = (UINT)(CurPosZ - m_nMinZ);

	XPoint = (UINT)(X / FIELDBLOCKRANGE);
	ZPoint = (UINT)(Z / FIELDBLOCKRANGE);

	Index = (ZPoint * m_nWidthNo) + XPoint;
	if (Index >= m_FieldBlocks.size()) 
	{
		if( bIsPvPLobby() )
		{
			if( m_FieldBlocks.size() > 0 )
				return m_FieldBlocks[0];
		}
		return NULL;
	}

	return m_FieldBlocks[Index];
}

void CDNField::GetViewRange(TPosition *pCurPos, int &sxp, int &szp, int &exp, int &ezp)
{
	UINT x, z, sx, sz, ex, ez;

	x = (UINT)((pCurPos->nX / 1000) - m_nMinX);
	z = (UINT)((pCurPos->nZ / 1000) - m_nMinZ);

	if (x >= VIEWRANGE) sx = x - VIEWRANGE;
	else sx = 0;

	if (z >= VIEWRANGE) sz = z - VIEWRANGE;
	else sz = 0;

	if (x + VIEWRANGE < m_nWidth) ex = x + VIEWRANGE;
	else ex = m_nWidth - 1;

	if (z + VIEWRANGE < m_nHeight) ez = z + VIEWRANGE;
	else ez = m_nHeight - 1;

	sxp = (int)(sx / FIELDBLOCKRANGE);
	szp = (int)(sz / FIELDBLOCKRANGE);
	exp = (int)(ex / FIELDBLOCKRANGE);
	ezp = (int)(ez / FIELDBLOCKRANGE);
}

void CDNField::GetDoubleViewRange(TPosition *pCurPos, int &sxp, int &szp, int &exp, int &ezp)
{
	UINT x, z, sx, sz, ex, ez;

	x = (UINT)((pCurPos->nX / 1000) - m_nMinX);
	z = (UINT)((pCurPos->nZ / 1000) - m_nMinZ);

	if (x >= (VIEWRANGE*2)) sx = x - (VIEWRANGE*2);
	else sx = 0;

	if (z >= (VIEWRANGE*2)) sz = z - (VIEWRANGE*2);
	else sz = 0;

	if (x + (VIEWRANGE*2) < m_nWidth) ex = x + (VIEWRANGE*2);
	else ex = m_nWidth - 1;

	if (z + (VIEWRANGE*2) < m_nHeight) ez = z + (VIEWRANGE*2);
	else ez = m_nHeight - 1;

	sxp = (int)(sx / FIELDBLOCKRANGE);
	szp = (int)(sz / FIELDBLOCKRANGE);
	exp = (int)(ex / FIELDBLOCKRANGE);
	ezp = (int)(ez / FIELDBLOCKRANGE);
}

void CDNField::RequestGenerationNpc(int nNpcID, TPosition *pCurPos, float fRotate)
{
	TNpcData *pNpcData = g_pDataManager->GetNpcData(nNpcID);
	if (!pNpcData) return;

	CDNNpcObject *pNpcObj = new CDNNpcObject;
	if (!pNpcObj) return;

	pNpcObj->CreateNpc(pNpcData, "", g_IDGenerator.GetNpcID(), GetChnnelID(), GetMapIndex(), pCurPos, 0);
	AddNpcObject(pNpcObj);
}

bool CDNField::AddNpcObject(CDNNpcObject *pNpcObj)
{
	return m_pNpcObjectManager->AddObject(pNpcObj);
}

CDNNpcObject* CDNField::GetNpcObjectByNpcObjID(UINT nNpcObjID)
{
	return (CDNNpcObject*)m_pNpcObjectManager->FindObject(nNpcObjID);
}

void CDNField::SendFieldNotice(int cLen, const WCHAR *pwszChatMsg, int nShowSec)
{
	if (m_FieldBlocks.empty()) return;

	for (int i = 0; i < (int)m_FieldBlocks.size(); i++){
		m_FieldBlocks[i]->SendFieldNotice(cLen, pwszChatMsg, nShowSec);
	}
}

void CDNField::SendChannelChat(int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet)
{
	if (m_FieldBlocks.empty()) return;

	for (int i = 0; i < (int)m_FieldBlocks.size(); i++){
		m_FieldBlocks[i]->SendChannelChat(cLen, pwszCharacterName, pwszChatMsg);
	}
}

void CDNField::SendDebugChat(const WCHAR* szMsg)
{
	if (m_FieldBlocks.empty()) return;

	for (int i = 0; i < (int)m_FieldBlocks.size(); i++){
		m_FieldBlocks[i]->SendDebugChat(szMsg);
	}
}
