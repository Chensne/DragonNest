#pragma once

#include "DnCamera.h"
#include "DnActor.h"

class CDnNpcTalkCamera : public CDnCamera 
{

public:
	CDnNpcTalkCamera( DnCameraHandle hCurCamera, DnActorHandle hTargetActor );
	virtual ~CDnNpcTalkCamera();

	enum WorkState 
	{
		TalkNone,
		BeginTalk,
		Talking,
		EndTalk,
		RestoreCamera,
	};

protected:
	DnCameraHandle m_hSourceCamera;
	DnActorHandle m_hTargetActor;
	LOCAL_TIME m_StartTime;

	WorkState m_State;

	MatrixEx m_StartCross;
	MatrixEx m_EndCross;

public:
	virtual bool Activate();
	virtual bool DeActivate();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetWorkState( WorkState State );
	WorkState GetWorkState() { return m_State; }

	DnActorHandle GetTargetActor() { return m_hTargetActor; }
	DnCameraHandle GetSourceCamera() { return m_hSourceCamera; }

};