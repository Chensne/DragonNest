

#include "StdAfx.h"
#include "ConfigMngr.h"
#include "VarArg.h"

// CConfigMngr

DEFINE_SINGLETON_CLASS(CConfigMngr);

CConfigMngr::CConfigMngr()
{
	
}

VOID CConfigMngr::Reset()
{
	m_IpAddress.Reset();
	m_PortNo = 0;
	m_ID.Empty();
	::memset(&m_WindowPlacement, 0, sizeof(m_WindowPlacement));
}

DWORD CConfigMngr::Open(LPCTSTR pFileName)
{
	TCHAR aLocalPath[MAX_PATH] = { _T('\0'), };
	TCHAR aFileName[MAX_PATH] = { _T('\0'), };
	TCHAR aExecName[MAX_PATH] = { _T('\0'), };
	TCHAR aDrvName[MAX_PATH] = { _T('\0'), };
	TCHAR aDirName[MAX_PATH] = { _T('\0'), };

	{
		DWORD aRetVal1 = ::GetModuleFileName(NULL, aLocalPath, _countof(aLocalPath));
		if (!aRetVal1) 
			return (HASERROR+0);
	
		errno_t aRetVal2 = _tsplitpath_s(aLocalPath, aDrvName, _countof(aDrvName), aDirName, _countof(aDirName), aFileName, _countof(aFileName), aExecName, _countof(aExecName));
		if (aRetVal2) 
			return (HASERROR+1);
	
		_sntprintf_s(aLocalPath, _countof(aLocalPath), _T("%s%s%s"), aDrvName, aDirName, pFileName);
	}

	DWORD aRetVal = m_IniFile.Open(aLocalPath);
	if (NOERROR != aRetVal)
		return aRetVal;
	
	// Service Manager
	{
		// IP Address
		{
			TCHAR aIpAddress[DF_IPADDR_SIZE] = { _T('\0'), };
			aRetVal = m_IniFile.GetValue(_T("ServiceManager"), _T("IpAddress"), aIpAddress);
			
            // TODO(Cussrro): 修复空字符常量
            if (_T(' ') == aIpAddress[0])
				_tcsncpy_s(aIpAddress, _countof(aIpAddress), _T("0.0.0.0"), _countof(aIpAddress));
			{
				USES_CONVERSION;
				m_IpAddress.Set(T2CA(aIpAddress));
			}
			
		}

		// Port Number
		{
			INT aPortNo;
			aRetVal = m_IniFile.GetValue(_T("ServiceManager"), _T("PortNo"), &aPortNo, EV_PORTNO_DEF);
			m_PortNo = static_cast<WORD>(aPortNo);
		}

		// ID
		{
			TCHAR aID[SMIDLENMAX+1] = { _T("\0"), };
			aRetVal = m_IniFile.GetValue(_T("ServiceManager"), _T("ID"), aID);
			m_ID = aID;
		}

		
	}

	// Window Position
	{
		m_WindowPlacement.length = sizeof(m_WindowPlacement);

		aRetVal = m_IniFile.GetValue(_T("WindowPosition"), _T("PositionValue"), &m_WindowPlacement, sizeof(m_WindowPlacement));
	}

	// Wave File
	{
		m_WavFile = CVarArg<MAX_PATH>(_T("%s%s%s"), aDrvName, aDirName, _T("alert.wav"));

		errno_t aRetVal = _taccess_s(m_WavFile, 0);
		if (aRetVal) {
			m_WavFile.Empty();
		}
	}

	return NOERROR;
}

VOID CConfigMngr::Close()
{
	m_IniFile.Close();
}

VOID CConfigMngr::Flush()
{
	// Service Manager
	{
		USES_CONVERSION;

		m_IniFile.SetValue(_T("ServiceManager"), _T("IpAddress"), CA2T(m_IpAddress.GetDot()));

		m_IniFile.SetValue(_T("ServiceManager"), _T("PortNo"), m_PortNo);

		m_IniFile.SetValue(_T("ServiceManager"), _T("ID"), ((!m_ID.IsEmpty())?(m_ID.GetBuffer(0)):(_T(""))));
	}

	// Window Position
	{
		m_WindowPlacement.length = sizeof(m_WindowPlacement);
		m_WindowPlacement.flags |= (WPF_SETMINPOSITION);

		m_IniFile.SetValue(_T("WindowPosition"), _T("PositionValue"), &m_WindowPlacement, sizeof(m_WindowPlacement));
	}

	
}
