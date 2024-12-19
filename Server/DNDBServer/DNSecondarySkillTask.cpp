#include "StdAfx.h"
#include "DNSecondarySkillTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNSecondarySkillTask::CDNSecondarySkillTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNSecondarySkillTask::~CDNSecondarySkillTask(void)
{
}

#if defined( PRE_ADD_SECONDARY_SKILL )
void CDNSecondarySkillTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLWorld *pWorldDB = NULL;

	switch( nSubCmd )
	{
	case QUERY_ADD_SECONDARYSKILL:
		{
			TQAddSecondarySkill* pPacket = reinterpret_cast<TQAddSecondarySkill*>(pData);

			int iRet = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				iRet = pWorldDB->QueryAddSecondarySkill( pPacket );

			if( iRet != ERROR_NONE )
				m_pConnection->QueryResultError( pPacket->nAccountDBID, iRet, nMainCmd, nSubCmd );
			break;
		}
	case QUERY_DELETE_SECONDARYSKILL:
		{
			TQDelSecondarySkill* pPacket = reinterpret_cast<TQDelSecondarySkill*>(pData);

			int iRet = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				iRet = pWorldDB->QueryDelSecondarySkill( pPacket );

			if( iRet != ERROR_NONE )
				m_pConnection->QueryResultError( pPacket->nAccountDBID, iRet, nMainCmd, nSubCmd );
			break;
		}
	case QUERY_GETLIST_SECONDARYSKILL:
		{
			TQGetListSecondarySkill* pPacket = reinterpret_cast<TQGetListSecondarySkill*>(pData);

			TAGetListSecondarySkill Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListSecondarySkill( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%d] [QueryGetListSecondarySkill:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.SkillList)+(Packet.cCount*sizeof(Packet.SkillList[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );

			if( Packet.nRetCode == ERROR_NONE )
			{
				TQGetListManufactureSkillRecipe RecipeQ;
				RecipeQ.cThreadID		= pPacket->cThreadID;
				RecipeQ.cWorldSetID		= pPacket->cWorldSetID;
				RecipeQ.nAccountDBID	= pPacket->nAccountDBID;
				RecipeQ.biCharacterDBID	= pPacket->biCharacterDBID;

				TAGetListManufactureSkillRecipe RecipeA;
				memset( &RecipeA, 0, sizeof(RecipeA) );

				RecipeA.nRetCode		= ERROR_DB;
				RecipeA.nAccountDBID	= RecipeQ.nAccountDBID;

				if (pWorldDB)
					RecipeA.nRetCode = pWorldDB->QueryGetListManufactureSkillRecipe( &RecipeQ, &RecipeA );

				int iSize = sizeof(RecipeA)-sizeof(RecipeA.RecipeList)+(RecipeA.cCount*sizeof(RecipeA.RecipeList[0]));
				m_pConnection->AddSendData( nMainCmd, QUERY_GETLIST_SECONDARYSKILL_RECIPE, reinterpret_cast<char*>(&RecipeA), iSize );
			}

			break;
		}
	case QUERY_MOD_SECONDARYSKILL_EXP:
		{
			TQModSecondarySkillExp* pPacket = reinterpret_cast<TQModSecondarySkillExp*>(pData);

			int iRet = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				iRet = pWorldDB->QueryModSecondarySkillExp( pPacket );

			if( iRet != ERROR_NONE )
				m_pConnection->QueryResultError( pPacket->nAccountDBID, iRet, nMainCmd, nSubCmd );
			break;
		}
	case QUERY_ADD_SECONDARYSKILL_RECIPE:
		{
			TQSetManufactureSkillRecipe* pPacket = reinterpret_cast<TQSetManufactureSkillRecipe*>(pData);

			int iRet = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				iRet = pWorldDB->QuerySetManufactureSkillRecipe( pPacket );

			if( iRet != ERROR_NONE )
				m_pConnection->QueryResultError( pPacket->nAccountDBID, iRet, nMainCmd, nSubCmd );
			break;
		}
	case QUERY_DELETE_SECONDARYSKILL_RECIPE:
		{
			TQDelManufactureSkillRecipe* pPacket = reinterpret_cast<TQDelManufactureSkillRecipe*>(pData);

			int iRet = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				iRet = pWorldDB->QueryDelManufactureSkillRecipe( pPacket );

			if( iRet != ERROR_NONE )
				m_pConnection->QueryResultError( pPacket->nAccountDBID, iRet, nMainCmd, nSubCmd );
			break;
		}
	case QUERY_EXTRACT_SECONDARYSKILL_RECIPE:
		{
			TQExtractManufactureSkillRecipe* pPacket = reinterpret_cast<TQExtractManufactureSkillRecipe*>(pData);

			int iRet = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				iRet = pWorldDB->QueryExtractManufactureSkillRecipe( pPacket );

			if( iRet != ERROR_NONE )
				m_pConnection->QueryResultError( pPacket->nAccountDBID, iRet, nMainCmd, nSubCmd );
			break;
		}
	case QUERY_MOD_SECONDARYSKILL_RECIPE_EXP:
		{
			TQModManufactureSkillRecipe* pPacket = reinterpret_cast<TQModManufactureSkillRecipe*>(pData);

			int iRet = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				iRet = pWorldDB->QueryModManufactureSkillRecipe( pPacket );

			if( iRet != ERROR_NONE )
				m_pConnection->QueryResultError( pPacket->nAccountDBID, iRet, nMainCmd, nSubCmd );
			break;
		}
	}
}
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
