#pragma once

#include "stdafx.h"
#include "SMConfig.h"
#include "XMLParser.h"
#include <sstream>

wstring GenerateServerReportNation()
{
#if defined (_KR)
	return L"[KOR]";
#elif (_CH)
	return L"[CHN]";
#elif (_JP)
	return L"[JPN]";
#elif (_TW)
	return L"[TWN]";
#elif (_SG)
	return L"[SIN]";
#elif (_US)
	return L"[USA]";
#elif (_TH)
	return L"[THA]";
#else
	return L"[DEV]";
#endif
}

wstring GenerateServerReportFileName()
{
	CTime time = CTime::GetCurrentTime();

	wstringstream ss;
	ss << L"server_report_" << time.Format(L"%Y%m%d%H%M%S").GetBuffer(0) << L".txt";
	return ss.str();
}

CTime ConvertStringToTime(const wstring& str)
{
	if (str.length() != 19)
		throw;

	int year = _wtoi(str.substr(0, 4).c_str());
	int month = _wtoi(str.substr(5, 2).c_str());
	int day = _wtoi(str.substr(8, 2).c_str());
	int hour = _wtoi(str.substr(11, 2).c_str());
	int minute = _wtoi(str.substr(14, 2).c_str());
	int second = _wtoi(str.substr(17, 2).c_str());

	return CTime(year, month, day, hour, minute, second);
}

CTimeSpan ConvertStringToTimeSpan(const wstring& str)
{
	if (str.length() != 8)
		throw;

	int hour = _wtoi(str.substr(0, 2).c_str());
	int minute = _wtoi(str.substr(3, 2).c_str());
	int second = _wtoi(str.substr(6, 2).c_str());

	return CTimeSpan(0, hour, minute, second);
}

// FrameConfig
FrameConfig::FrameConfig()
	: size(800, 600)
{
}

FrameConfig::~FrameConfig()
{

}

// ViewConfig
ViewConfig::ViewConfig()
	: bgColor(RGB(0, 0, 0))
{

}

ViewConfig::~ViewConfig()
{

}

COLORREF ViewConfig::GetBgColor() const
{
	return bgColor;
}

COLORREF ViewConfig::GetFontColor(const wstring& name) const
{
	map<wstring, COLORREF>::const_iterator it = fontColors.find(name);
	if (it == fontColors.end())
		return RGB(192, 192, 192);

	return it->second;
}

COLORREF ViewConfig::GetDefaultFontColor() const
{
	if (fontColors.empty())
		return RGB(192, 192, 192);

	map<wstring, COLORREF>::const_iterator it = fontColors.find(L"Default");
	if (it == fontColors.end())
		return RGB(192, 192, 192);

	return it->second;
}

// EMailConfig
void EMailConfig::Reset()
{
	serverIp.clear();
	serverPage.clear();
	boundary.clear();
	sender.clear();
	subject.clear();
	msg.clear();
	recipients.clear();
	allowance.clear();
}

// ConfigEx
ConfigEx::ConfigEx()
{
	SetDefault();
}

ConfigEx::~ConfigEx()
{
	Reset();
}

void ConfigEx::SetDefault()
{
	cmdHistoryCount = 100;
	maxLogCount = 1000;
	serverReportDays = 7;
	serverReportDetail = true;
	exceptLogSize = 1024;

	Reset();
}

void ConfigEx::Reset()
{
	for each (map<wstring, const ViewConfig*>::value_type v in views)
	{
		delete v.second;
	}
	views.clear();

	for each (map<wstring, const FrameConfig*>::value_type v in frames)
	{
		delete v.second;
	}
	frames.clear();

	for each (const ScheduleJob* v in schedules)
	{
		delete v;
	}
	schedules.clear();

	email.Reset();
}

bool ConfigEx::CreateServiceManagerExXML()
{
	CXMLCreater	creater;

	if(creater.InitByUtf8("DNServiceManagerEx.xml","./Config/")== false){
		return false;
	}
	creater.AddElement("SMConfigEx", true);

	creater.AddElement("Log", true);
	creater.AddElement("List");
	creater.AddAttribute("LogCount", 1000);
	creater.GoRarent();

	creater.AddElement("Command", true);
	creater.AddElement("Input");
	creater.AddAttribute("HistoryCount", 100);
	creater.GoRarent();

	creater.AddElement("ServerReport", true);
	creater.AddElement("Report");
	creater.AddAttribute("Days", 7);
	creater.AddAttribute("Detail", "true");
	creater.AddAttribute("ExceptSize", 1024);
	creater.GoRarent();

	creater.AddElement("Frame", true);
	creater.AddElement("Size", true);
	creater.AddElement("Frame", true);
	creater.AddAttribute("Name", "MainFrame");
	creater.AddElement("Size");
	creater.AddAttribute("X", 800);
	creater.AddAttribute("Y", 600);
	creater.GoRarent();
	creater.GoRarent();
	creater.GoRarent();

	creater.AddElement("View", true);
	creater.AddElement("Color", true);

	creater.AddElement("View", true);
	creater.AddAttribute("Name", "Manager");
	creater.AddElement("BG");
	creater.AddAttribute("R", 0);
	creater.AddAttribute("G", 0);
	creater.AddAttribute("B", 0);
	creater.AddElement("Font");
	creater.AddAttribute("Type", "Default");
	creater.AddAttribute("R", 192);
	creater.AddAttribute("G", 192);
	creater.AddAttribute("B", 192);
	creater.GoRarent();

	creater.AddElement("View", true);
	creater.AddAttribute("Name", "State");
	creater.AddElement("BG");
	creater.AddAttribute("R", 0);
	creater.AddAttribute("G", 0);
	creater.AddAttribute("B", 0);
	creater.AddElement("Font");
	creater.AddAttribute("Type", "Default");
	creater.AddAttribute("R", 192);
	creater.AddAttribute("G", 192);
	creater.AddAttribute("B", 192);
	creater.GoRarent();

	creater.AddElement("View", true);
	creater.AddAttribute("Name", "DefaultState");
	creater.AddElement("BG");
	creater.AddAttribute("R", 0);
	creater.AddAttribute("G", 0);
	creater.AddAttribute("B", 0);
	creater.AddElement("Font");
	creater.AddAttribute("Type", "Default");
	creater.AddAttribute("R", 192);
	creater.AddAttribute("G", 192);
	creater.AddAttribute("B", 192);
	creater.GoRarent();

	creater.AddElement("View", true);
	creater.AddAttribute("Name", "Log");
	creater.AddElement("BG");
	creater.AddAttribute("R", 0);
	creater.AddAttribute("G", 0);
	creater.AddAttribute("B", 0);
	//LogView Font는  xml에서 정의된 것만 추가
	wstring wstrLogFontName[35] = {L"_NORMAL",
		L"_ERROR",
		L"_DANGERPOINT",
		L"_FILELOG",
		L"_HACK",
		L"_HACKSHIELD",
		L"_ITEMDUPLICATE",
		L"_WEAPONTYPE",
		L"_SESSIONCRASH",
		L"_ROOMCRASH",
		L"_UNREACHABLE",
		L"_LOADINGDISCONNECT",
		L"_ROOMFINALIZECRASH",
		L"_GAMECONNECTLOG",
		L"_GAMESERVERDELAY",
		L"_ENTERGAMECHECK",
		L"_DROPITEMPENALTY",
		L"_TREASUREPENALTY",
		L"_CLEARGRADEPENALTY",
		L"_PRELOADEDDYNAMICLOAD",
		L"_DELOBJECTNAMEKEYFINDERR",
		L"_OCTREELOG",
		L"_INVALIDACTOR",
		L"_GPKERROR",
		L"_ACCEPTORERROR",
		L"_QUESTHACK",
		L"_ZOMBIEUSER",
		L"_EVENTLIST",
		L"_FARM",
		L"_GUILDWAR",
		L"_MOVEPACKETSEQ",
		L"_SKILLACTION",
		L"_FILEDBLOG",
		L"_LADDER",
		L"_PVPROOM"};
	
	for(int i = 0; i < 35; i++){
		creater.AddElement("Font");
		creater.AddAttribute("Type", wstrLogFontName[i].c_str());
		creater.AddAttribute("R", 192);
		creater.AddAttribute("G", 192);
		creater.AddAttribute("B", 192);
	}
	creater.GoRarent();
	
	creater.Create();
	return true;
}


void ConfigEx::LoadConfig()
{
	try
	{
		CXMLParser parser;
		if (!parser.Open("./Config/DNServiceManagerEx.xml"))
		{
			//XML 생성
			if(!CreateServiceManagerExXML()){
				AfxMessageBox(L"[CreateServiceManagerExXML] CXMLCreater fail to create DNServiceManagerEx.xml", MB_ICONASTERISK);	
				return;
			}
			if (!parser.Open("./Config/DNServiceManagerEx.xml")){
				AfxMessageBox(L"Please check [Config/DNServiceManagerEx.xml]", MB_ICONASTERISK);	
				return;
			}
		}

		if (parser.FirstChildElement("SMConfigEx", true))
		{
			if (parser.FirstChildElement("Log", true))
			{
				if (parser.FirstChildElement("List"))
				{
					maxLogCount = ::_wtoi(parser.GetAttribute("LogCount"));
				}
				parser.GoParent();
			}

			if (parser.FirstChildElement("Command", true))
			{
				if (parser.FirstChildElement("Input"))
				{
					cmdHistoryCount = ::_wtoi(parser.GetAttribute("HistoryCount"));
				}
				parser.GoParent();
			}

			if (parser.FirstChildElement("ServerReport", true))
			{
				if (parser.FirstChildElement("Report"))
				{
					serverReportDays = ::_wtoi(parser.GetAttribute("Days"));
					serverReportDetail = (::_wcsicmp(parser.GetAttribute("Detail"), L"true") == 0) ? true : false;
					exceptLogSize = ::_wtoi(parser.GetAttribute("ExceptSize"));
				}
				parser.GoParent();
			}


			if (parser.FirstChildElement("Frame", true))
			{
				if (parser.FirstChildElement("Size", true))
				{
					if (parser.FirstChildElement("Frame", true))
					{
						do 
						{
							FrameConfig* pFrameConfig = NULL;

							try
							{
								pFrameConfig = new FrameConfig();
								wstring name = parser.GetAttribute("Name");

								if (parser.FirstChildElement("Size"))
								{
									int x = (int)::_wtoi(parser.GetAttribute("X"));
									int y = (int)::_wtoi(parser.GetAttribute("Y"));
									pFrameConfig->size.SetPoint(x, y);
								}

								frames.insert(make_pair(name, pFrameConfig));
							}
							catch (...)
							{
								delete pFrameConfig;
							}
						} while (parser.NextSiblingElement("Frame"));

						parser.GoParent();
					}
					parser.GoParent();
				}
				parser.GoParent();
			}

			if (parser.FirstChildElement("View", true))
			{
				if (parser.FirstChildElement("Color", true))
				{
					if (parser.FirstChildElement("View", true))
					{
						do
						{
							ViewConfig* pViewConfig = NULL;
							
							try
							{
								pViewConfig = new ViewConfig();
								wstring name = parser.GetAttribute("Name");

								if (parser.FirstChildElement("BG"))
								{
									BYTE r = (BYTE)::_wtoi(parser.GetAttribute("R"));
									BYTE g = (BYTE)::_wtoi(parser.GetAttribute("G"));
									BYTE b = (BYTE)::_wtoi(parser.GetAttribute("B"));
									pViewConfig->bgColor = RGB(r, g, b);
								}

								if (parser.FirstChildElement("Font"))
								{
									do
									{
										wstring type = parser.GetAttribute("Type");
										BYTE r = (BYTE)::_wtoi(parser.GetAttribute("R"));
										BYTE g = (BYTE)::_wtoi(parser.GetAttribute("G"));
										BYTE b = (BYTE)::_wtoi(parser.GetAttribute("B"));
										pViewConfig->fontColors.insert(make_pair(type, RGB(r, g, b)));
									} while (parser.NextSiblingElement("Font"));
								}

								views.insert(make_pair(name, pViewConfig));
							}
							catch (...)
							{
								delete pViewConfig;
							}
						} while (parser.NextSiblingElement("View"));

						parser.GoParent();
					}
					parser.GoParent();
				}
				parser.GoParent();
			}

			if (parser.FirstChildElement("EMail", true))
			{
				if (parser.FirstChildElement("Server"))
				{
					email.serverIp = parser.GetAttribute("Ip");
					email.serverPage = parser.GetAttribute("Page");
					email.boundary = parser.GetAttribute("Boundary");
				}

				if (parser.FirstChildElement("SendInfo", true))
				{
					email.sender = parser.GetAttribute("Sender");
					email.subject = parser.GetAttribute("Subject");

					if (parser.FirstChildElement("AddMsg"))
					{
						do 
						{
							email.msg.push_back(parser.GetAttribute("Msg"));
						} while (parser.NextSiblingElement("AddMsg"));
					}
					parser.GoParent();
				}

				if (parser.FirstChildElement("Recipients", true))
				{
					if (parser.FirstChildElement("Recipient"))
					{
						do 
						{
							email.recipients.push_back(parser.GetAttribute("EMail"));
							const WCHAR * wszAllowance = parser.GetAttribute("Allowance");
							if (wszAllowance)
								email.allowance.push_back(wszAllowance);
						} while (parser.NextSiblingElement("Recipient"));
					}
					parser.GoParent();
				}
				parser.GoParent();
			}

			if (parser.FirstChildElement("Schedule", true))
			{
				if (parser.FirstChildElement("Job"))
				{
					do 
					{
						ScheduleJob* pScheduleJob = new ScheduleJob();
						pScheduleJob->type = parser.GetAttribute("Type");
						pScheduleJob->name = parser.GetAttribute("Name");
						pScheduleJob->beginTime = ConvertStringToTime(parser.GetAttribute("BeginTime"));
						pScheduleJob->endTime = ConvertStringToTime(parser.GetAttribute("EndTime"));
						pScheduleJob->interval = ConvertStringToTimeSpan(parser.GetAttribute("Interval"));
						pScheduleJob->repeat = (::_wcsicmp(parser.GetAttribute("Repeat"), L"true") == 0) ? true : false;
						schedules.push_back(pScheduleJob);
					} while (parser.NextSiblingElement("Job"));
				}
				parser.GoParent();
			}
		}
	}
	catch (...)
	{
		AfxMessageBox(L"Please check [Config/DNServiceManagerEx.xml]", MB_ICONASTERISK);		
		return;
	}
}

const FrameConfig* ConfigEx::GetFrameConfig(const wstring& name) const
{
	map<wstring, const FrameConfig*>::const_iterator it = frames.find(name);
	if (it == frames.end())
		return NULL;

	return it->second;
}

const ViewConfig* ConfigEx::GetViewConfig(const wstring& name) const
{
	map<wstring, const ViewConfig*>::const_iterator it = views.find(name);
	if (it == views.end())
		return NULL;

	return it->second;
}

const EMailConfig& ConfigEx::GetEMailConfig() const
{
	return email;
}