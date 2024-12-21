
#pragma once

class CMAAiProcessor
{
public:

	CMAAiProcessor():m_dwBeginTime(0)
	{
	}

	virtual void OnBegin()
	{
		m_dwBeginTime = timeGetTime();
	}
	virtual void Process( DnActorHandle hActor ) = 0;
	virtual void OnEnd()
	{
		m_dwBeginTime = 0;
	}

protected:

	DWORD m_dwBeginTime;
};

class CMAAiChangeTargetByAggroProcessorByAggro:public CMAAiProcessor
{
public:
	CMAAiChangeTargetByAggroProcessorByAggro( int iInterval ):m_uiInterval(iInterval){}
	virtual void Process( DnActorHandle hActor );
private:
	UINT m_uiInterval;
};

class CMAAiChangeTargetProcessorByNearDiatance:public CMAAiProcessor
{
public:
	CMAAiChangeTargetProcessorByNearDiatance( int iInterval ):m_uiInterval(iInterval){}
	virtual void Process( DnActorHandle hActor );
private:
	UINT m_uiInterval;
};

class CMAAiProcessorManager
{
public:
	
	~CMAAiProcessorManager();

	void Begin();
	void End();
	void Process( DnActorHandle hActor );
	bool bLoad( lua_tinker::table& t );

private:

	std::vector<CMAAiProcessor*> m_vProcessor;
};
