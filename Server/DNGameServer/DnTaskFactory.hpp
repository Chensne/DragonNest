
#pragma once

#include "DnFarmGameTask.h"

class CDnTaskFactory
{
public:

	static CDnGameTask* CreateGameTask( GameTaskType::eType GameTaskType, CDNGameRoom* pRoom  )
	{
		switch( GameTaskType )
		{
			case GameTaskType::PvP:
			{
				return new CDnPvPGameTask( pRoom ); 
			}
			case GameTaskType::DarkLair:
			{
				return new CDnDLGameTask( pRoom ); 
			}
			case GameTaskType::Farm:
			{
				return new CDnFarmGameTask( pRoom ); 
			}
		}

		return new IBoostPoolDnGameTask( pRoom );;
	}

	static CDnPartyTask* CreatePartyTask( GameTaskType::eType GameTaskType, CDNGameRoom* pRoom  )
	{
		switch( GameTaskType )
		{
			case GameTaskType::DarkLair:
			{
				return new CDnDLPartyTask( pRoom );
			}
			case GameTaskType::PvP:
			{
				return new CDnPvPPartyTask( pRoom ); 
			}
		}

		return new IBoostPoolDnPartyTask( pRoom ); 
	}
};
