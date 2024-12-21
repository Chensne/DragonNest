#pragma once

#include "DnActor.h"

// nTeamSetting 이 -1이 아닐 경우 nTeamSetting값으로 세팅 해주고 -1 일 경우 자동으로 세팅해준다.
// 현재는 0이 유저이고 1이 몹이다.
DnActorHandle CreateActor( CMultiRoom *pRoom, int nActorTableID, int nTeamSetting = -1 );
