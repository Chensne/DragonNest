#pragma once

class CDNScript
{
public:
	CDNScript();
	virtual ~CDNScript();

	bool Create(const WCHAR* szScriptFileName);
	void Destroy();

	static void ReloadAllScript();
protected:
	lua_State*	m_pLua;
	std::wstring	m_wszScriptFileName;
	static std::vector<CDNScript*>	ms_ScriptList;
};
