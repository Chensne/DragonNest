#pragma once

#include "DnActor.h"

// nTeamSetting �� -1�� �ƴ� ��� nTeamSetting������ ���� ���ְ� -1 �� ��� �ڵ����� �������ش�.
// ����� 0�� �����̰� 1�� ���̴�.
DnActorHandle CreateActor( CMultiRoom *pRoom, int nActorTableID, int nTeamSetting = -1 );
