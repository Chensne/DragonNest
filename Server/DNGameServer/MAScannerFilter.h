
#pragma once

class IMAScanFilter
{
public:

	enum eFilterType
	{
		SameTeam = 0,
		OpponentTeam,
		DestActorDie,
		DestActorNpc,
		MinDistance,
		EqualActor,
		Max,
	};

	virtual bool bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin=0.f ) = 0;
};

class MAScanSameTeamFilter : public IMAScanFilter
{
public:
	virtual bool bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin=0.f );
};

class MAScanOpponentTeamFilter : public IMAScanFilter
{
public:
	virtual bool bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin=0.f );
};

class MAScanDestActorDieFilter : public IMAScanFilter
{
public:
	virtual bool bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin=0.f );
};

class MAScanDestActorNpcFilter : public IMAScanFilter
{
public:
	virtual bool bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin=0.f );
};

class MAScanMinDistanceFilter : public IMAScanFilter
{
public:
	virtual bool bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin=0.f );
};

class MAScanEqualActorFilter : public IMAScanFilter
{
public:
	virtual bool bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin=0.f );
};
