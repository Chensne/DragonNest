#pragma once
class FarmPVP : public CSingleton<FarmPVP>
{
public:
	FarmPVP(void);
	~FarmPVP(void);
	void OnJoinPVP();
	void OnDie(DnActorHandle hActor, DnActorHandle hHitter);
};

