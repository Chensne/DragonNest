// Bluenet 블루넷
INSERT_CANDIDATE("BN00", _T("bluenetterm.exe"), _T("_"));

// Cyberria 사이버리아
INSERT_CANDIDATE("CB00", _T("client.exe"), _T("Cyberria"));
INSERT_CANDIDATE("CB99", _T("client.exe"), _T("현재사용분")); // 031218 added

// Dreamnet 드림넷
INSERT_CANDIDATE("DN00", _T("imgsaver.exe"), _T("게임시간"));

// DT2000 (http://www.dtt2000.co.kr)
INSERT_CANDIDATE("DT2S", _T("dreamto2000.exe"), _T("DT2000"));
INSERT_CANDIDATE("DT98", _T("dttcli.exe"), _T("DT2000"));
INSERT_CANDIDATE("DTXP", _T("svchost.exe"), _T("DT2000"));

// Eastec 이스텍 (http://www.eastec.net)
INSERT_CANDIDATE("EA00", _T("pcbangclient.ex"), _T("PC"));
INSERT_CANDIDATE("EA00", _T("pcbangclient.exe"), _T("PC"));

// EPocket 이포켓 (http://www.epocket.co.kr)
INSERT_CANDIDATE("EP00", _T("epclient.exe"), _T("ePocket"));

// GB2001 7.x (http://www.aci.co.kr)
INSERT_CANDIDATE("GB7S", _T("gb2001sp.exe"), _T("Gb2001sp"));

// GCSS Client
INSERT_CANDIDATE("GC00", _T("gcsc.exe"), _T("GCSS Client"));

// GameDory 게임돌이 (http://www.skydata.co.kr)
INSERT_CANDIDATE("GD00", _T("timechk.exe"), _T("TimeCheck"));
INSERT_CANDIDATE("GD01", _T("client.exe"), _T("게임돌이"));

// GameRoom Manager (http://www.estsoft.com)
INSERT_CANDIDATE("GM00", _T("peshell.exe"), _T("PESHELLF"));

// 지앤티씨 (http://www.gntc.co.kr)
INSERT_CANDIDATE("GN00", _T("client.exe"), _T("지앤티씨")); // 031218 added

// GamStar 겜스타
INSERT_CANDIDATE("GS00", _T("gamstar.exe"), _T("겜스타"));
INSERT_CANDIDATE("GS01", _T("server.exe"), _T("겜스타"));
INSERT_CANDIDATE("GS02", _T("expl.exe"), _T("겜스타"));

// Geto Plus 게토 (http://www.geto.co.kr)
INSERT_CANDIDATE("GT25", _T("report25.mod"), _T("PC방"));
INSERT_CANDIDATE("GT30", _T("agent30.mod"), _T("게토"));
INSERT_CANDIDATE("GT32", _T("agent32.mod"), _T("게토"));
INSERT_CANDIDATE("GT33", _T("agent33.mod"), _T("게토"));
INSERT_CANDIDATE("GT34", _T("agent34.mod"), _T("게토"));
INSERT_CANDIDATE("GT38", _T("agent38.bin"), _T("게토"));
INSERT_CANDIDATE("GT39", _T("agent391.bin"), _T("게토"));
INSERT_CANDIDATE("GT39", _T("agent392.bin"), _T("게토"));
INSERT_CANDIDATE("GT39", _T("agent39.bin"), _T("게토"));
INSERT_CANDIDATE("GT39", _T("getoman.exe"), _T("게토"));
INSERT_CANDIDATE("GT40", _T("agent40.bin"), _T("게토"));
INSERT_CANDIDATE("GT50", _T("dlag5x.exe"),	_T("Deluxe Client"));					// 2007.07.18 추가 - 게토 5.0 Deluxe
INSERT_CANDIDATE("GT50", _T("dlag5x.exe"),	_T(""));					// 2007.07.18 추가 - 게토 5.0 Deluxe

// 가장 높은 점유율을 가지면서 파일이름이 계속 바뀌는 게토의 경우, 
// EnumThreadWndProc 에서 따로 agent 로 시작하는 프로세스의 "게토" 윈도우를 체크하므로,
// 차후 버전을 꼭 넣어야 할 필요는 없다 (버전 구분이 필요할경우 사용)

// Grms 4.0 (http://www.leader-com.co.kr)
INSERT_CANDIDATE("GR4S", _T("grms.exe"), _T("Grms"));
INSERT_CANDIDATE("GR40", _T("gclient.exe"), _T("GClient"));

// I-wink iwink i윙크
INSERT_CANDIDATE("IW00", _T("client.exe"), _T("Main_f"));
INSERT_CANDIDATE("IW01", _T("client.exe"), _T("Info_f"));
INSERT_CANDIDATE("IW02", _T("client.exe"), _T("Sharky"));
INSERT_CANDIDATE("IW03", _T("client.exe"), _T("main_f")); // 040116 added
INSERT_CANDIDATE("IW99", _T("client.exe"), _T("간단한 볼륨조절기")); // 031218 added

// IPOClient
INSERT_CANDIDATE("IP00", _T("ipoclient.exe"), _T("Ipoclient"));

// ITM
INSERT_CANDIDATE("IT00", _T("run.exe"), _T("(ITM)")); // 031218 added

// LoginGame 로긴게임 (http://www.logingame.co.kr)			
INSERT_CANDIDATE("LG1S", _T("igc_gglm.exe"), _T("LoginGAME"));
INSERT_CANDIDATE("LG16", _T("igc_gglc.exe"), _T("LoginGAME"));
INSERT_CANDIDATE("LG00", _T("igc_gglc.exe"), _T("GGLogin"));

// Lycos 라이코스		
INSERT_CANDIDATE("LY00", _T("ticclient.exe"), _T("Ticclient"));
INSERT_CANDIDATE("LY01", _T("tmsclient.exe"), _T("TMS Client"));

// MediaDesk 미디어데스크 (http://www.mediaweb.co.kr)
INSERT_CANDIDATE("MD70", _T("clmanager.exe"), _T("CLManager"));
INSERT_CANDIDATE("MD61", _T("pbclient.exe"), _T("MD CLIENT"));
INSERT_CANDIDATE("MD60", _T("pbclient.exe"), _T("클라이언트"));
INSERT_CANDIDATE("MD62", _T("pbclient.exe"), _T("MDCLIENT"));
INSERT_CANDIDATE("MD63", _T("pbclient.exe"), _T("MDClient"));

// Mammoth 맘모스
INSERT_CANDIDATE("MM00", _T("ex.exe"), _T("FormTime"));

// MNET?
INSERT_CANDIDATE("MN00", _T("mnetc.exe"), _T("사용자화면")); // 031218 added

// Net Commander 넷커맨더 (http://www.nio.co.kr)
INSERT_CANDIDATE("NCCL", _T("ncclient.exe"), _T("Commander"));
INSERT_CANDIDATE("NCHC", _T("ncclient.exe"), _T("커맨더"));
INSERT_CANDIDATE("NC00", _T("ncclient.exe"), _T("NCClient")); // 040804 added
INSERT_CANDIDATE("NC30", _T("ncclient.exe"),		_T("MCI command handling window"));		// 2007.07.18 추가 - 넷커맨더 3.0 ??
INSERT_CANDIDATE("NC31", _T("ncclient.exe"),		_T("IMEKR2002_MAIN"));					// 2007.07.18 추가 - 넷커맨더 3.0 ??
INSERT_CANDIDATE("NC40", _T("netcmdclient.exe"),	_T(""));					// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("netcmdclient.exe"),	_T("IMEKR2002_MAIN"));					// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("netcmdclient.exe"),	_T("MCI command handling window"));		// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("ncrs.exe"),		_T("NCRT"));							// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("ncrs.exe"),		_T(""));								// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("NetCmdSys.exe"),		_T("NetCmdSys"));					// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("NetCmdSys.exe"),		_T("IMEKR2002_MAIN"));				// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("NetCmdSys.exe"),		_T(""));							// 2007.10.26 추가 - 넷커맨더 4.0 - 캡션이 없을수도
INSERT_CANDIDATE("NC40", _T("NCClien.exe"),			_T("넷-커맨더"));					// 2009.2.26 추가  - 넷커맨더 4.0 
INSERT_CANDIDATE("NC40", _T("NCClien.exe"),			_T("M"));					// 2009.2.26 추가  - 넷커맨더 4.0 
INSERT_CANDIDATE("NC40", _T("netcmdguard.exe"),		_T("IMEKR2002_MAIN"));					// 2009.2.26 추가  - 넷커맨더 4.0 


// Netimo 네띠모 (http://www.netimo.net)
INSERT_CANDIDATE("NECL", _T("ntmcli.exe"), _T("ntmcli"));
INSERT_CANDIDATE("NE01", _T("ismak32.exe"), _T("ismak32"));
INSERT_CANDIDATE("NEPK", _T("pkcnt.exe"), _T("Client"));

INSERT_CANDIDATE("NECL", _T("ntmccc.exe"), _T("ntmcli"));				// 2009.02.26 추가
INSERT_CANDIDATE("NECL", _T("ntmccc.exe"), _T("IMEKR2002_MAIN"));


// NewTime 뉴타임 (http://www.storymap.com/game)
INSERT_CANDIDATE("NT8S", _T("??).exe"), _T("NewTime"));
INSERT_CANDIDATE("NT87", _T("game98c.exe"), _T("NewTime"));

// Promedia 프로미디어
INSERT_CANDIDATE("PM00", _T("client.exe"), _T("ITM")); // 031218 added

// SmartNet client
INSERT_CANDIDATE("SN00", _T("netmon.exe"), _T("SmartNet")); // 031218 added

// 세종 IC 시스템
INSERT_CANDIDATE("SJ00", _T("pcmanager.exe"), _T("게임방")); // 040116 added

// Synaps 게임방관리 (www.synaps.co.kr)
INSERT_CANDIDATE("SY00", _T("pcagent.exe"), _T("Synaps")); // 031218 added
INSERT_CANDIDATE("SY01", _T("pcagent.exe"), _T("PcAgent")); // 040116 added

// Yeca 예카
INSERT_CANDIDATE("YC00", _T("yecamanagerc.exe"), _T("예카"));

// Unknown / Tiny
INSERT_CANDIDATE("Z000", _T("gamec.exe"), _T("게임방관리")); // 031218 added, 대구 겜방
INSERT_CANDIDATE("Z001", _T("pgb_client_mngr.exe"), _T("ClientManager")); // 031218 added
INSERT_CANDIDATE("Z002", _T("easyclient.exe"), _T("FormTime")); // 031218 added
INSERT_CANDIDATE("Z003", _T("systemcn.exe"), _T("Client")); // 040116 added

// ezClient
INSERT_CANDIDATE("Z003", _T("ezclick.exe"),			_T("ezClick"));							// 2007.07.18 추가 - ezClick



// 2007.07.18 추가분
INSERT_CANDIDATE("Z003", _T("pmclient.exe"),		_T("PicaClientFamily"));				// 2007.07.18 추가 - 피카 메니져
INSERT_CANDIDATE("Z003", _T("pmclient.exe"),		_T("피카에어"));						// 2009.02.20 추가 - 피카 메니져
INSERT_CANDIDATE("Z003", _T("netmon.exe"),			_T("IMEKR2002_MAIN"));					// 2007.07.18 추가 - ??
INSERT_CANDIDATE("Z003", _T("wmclt.exe"),			_T("WithManager Program"));				// 2007.07.18 추가 - ??

// 2008.01.29
INSERT_CANDIDATE("Z003", _T("pmclientmainUI.exe"),		_T("PicaClientFamily"));				// 2008.01.29 추가 - 피카 메니져
INSERT_CANDIDATE("Z003", _T("pmclientmainui.exe"),		_T("MCI command handling window"));			// 2008.01.29 추가 - 피카 메니져
INSERT_CANDIDATE("Z003", _T("pmclientmainUI.exe"),		_T("M"));						// 2008.01.29 추가 - 피카 메니져
INSERT_CANDIDATE("Z003", _T("pmccnntt.exe"),			_T("PicaClientFamily"));				// 2009.03.02 피카 추가
INSERT_CANDIDATE("Z003", _T("gal.exe"),				_T("스마트넷 (C)스마트시스템(주)"));			// 2009.03.02 스마트넷 추가
INSERT_CANDIDATE("Z003", _T("megalogon.exe"),			_T("사용정보창"));					// 2009.03.02 스마트넷 추가
INSERT_CANDIDATE("Z003", _T("megalogon.exe"),			_T("MCI command handling window"));			// 2009.03.02 스마트넷 추가

