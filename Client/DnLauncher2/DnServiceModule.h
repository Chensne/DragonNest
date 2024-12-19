#pragma once


// IServiceModule Class
class IServiceModule
{
public:
	IServiceModule() {}
	virtual ~IServiceModule() {}

public:
	virtual BOOL Initialize() { return TRUE; }
	virtual void Destroy() {}
	virtual BOOL OnForceFullVersionPatch() { return TRUE; }
};

// ServiceModule Class
class DnServiceModule
{
public:
	static IServiceModule* CreateServiceModule();
};


extern IServiceModule*	g_pServiceModule;
