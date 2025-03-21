#pragma once

class ConfigWork
{
public:
	void AddCommand(std::wstring& command)
	{
		std::transform(command.begin(), command.end(), command.begin(), tolower);
		m_Commands.insert(command);
	}

	void AddCommand(wchar_t* command)
	{
		AddCommand(std::wstring(command));
	}

	void RemoveCommand(std::wstring& command)
	{
		std::transform(command.begin(), command.end(), command.begin(), tolower);
		std::set<std::wstring>::iterator it = m_Commands.find(command);
		if (it == m_Commands.end())
			return;

		m_Commands.erase(it);
	}

	void RemoveCommand(wchar_t* command)
	{
		RemoveCommand(std::wstring(command));
	}

	void ClearCommand()
	{
		m_Commands.clear();
	}

	bool HasCommand(std::wstring& command) const
	{
		std::transform(command.begin(), command.end(), command.begin(), tolower);
		return (m_Commands.find(command) != m_Commands.end()) ? true : false;
	}

	bool HasCommand(wchar_t* command) const
	{
		return HasCommand(std::wstring(command));
	}

private:
	std::set<std::wstring> m_Commands;
};

extern ConfigWork g_ConfigWork;