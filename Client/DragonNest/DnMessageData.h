#pragma once

#if 0

class CDnMessageData
{
public:
	struct SCommandList
	{
		std::vector<int> m_vecAddCommand;
		std::vector<int> m_vecRemoveCommand;

		void InsertAddCmd( int nCommand )
		{
			m_vecAddCommand.push_back( nCommand );
		}

		void InsertRemoveCmd( int nCommand )
		{
			m_vecRemoveCommand.push_back( nCommand );
		}

		void Clear()
		{
			m_vecAddCommand.clear();
			m_vecRemoveCommand.clear();
		}
	};

	struct SCommandSet
	{
		int m_nMainCmd;
		int m_nSubCmd;

		SCommandSet( int nMainCmd, int nSubCmd )
			: m_nMainCmd(nMainCmd), m_nSubCmd(nSubCmd)
		{
		}

		bool operator < (const SCommandSet& other) const
		{
			if( m_nMainCmd == other.m_nMainCmd )
				return m_nSubCmd < other.m_nSubCmd;

			return m_nMainCmd < other.m_nMainCmd;
		}
	};

	typedef std::map<SCommandSet, SCommandList>	COMMAND_MAP;
	typedef COMMAND_MAP::iterator				COMMAND_MAP_ITER;

protected:
	COMMAND_MAP m_mapGlobalCommand;
	COMMAND_MAP m_mapLocalCommand;

public:
	enum emCommandType
	{
		typeGlobal,
		typeLocal,
	};

public:
	CDnMessageData(void);
	virtual ~CDnMessageData(void);

protected:
	bool FindCommand( SCommandSet &commandSet, COMMAND_MAP &commandMap );
	void GetAddCommand( SCommandSet &commandSet, COMMAND_MAP &commandMap, std::vector<int> &vecCommand );
	void GetRemoveCommand( SCommandSet &commandSet, COMMAND_MAP &commandMap, std::vector<int> &vecCommand );

public:
	bool EnableCommand( emCommandType commandType, SCommandSet &commandSet );
	void GetAddCommand( emCommandType commandType, SCommandSet &commandSet, std::vector<int> &vecAddCommand );
	void GetRemoveCommand( emCommandType commandType, SCommandSet &commandSet, std::vector<int> &vecRemoveCommand );
};

#endif