#pragma once

#include "stdafx.h"

struct ImmigratorUserEntity
{
	WORD	AType;
	WORD	 Sex;
	WORD	Age;
	WORD	Status;
	DWORD	No;

	char	Id[36];
	char   Email[128];
	char	Birthday[12];
	char	RegDay[12];

	ImmigratorUserEntity()
	{
		ZeroMemory( Id , 36 );
		ZeroMemory( Email , 128 );
		ZeroMemory( Birthday , 12 );
		ZeroMemory( RegDay , 12 );
		AType = Sex = Age= Status = 0;
		No =  0;
	}
};
