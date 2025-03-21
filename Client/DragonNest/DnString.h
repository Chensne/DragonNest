#ifdef _HSHIELD

#ifdef _KR
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]다른 게임이 실행중입니다.\n프로그램을 종료합니다.")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]잘못된 파일 설치되었습니다.\n프로그램을 재설치하시기 바랍니다.")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]컴퓨터에서 디버거 실행이 감지되었습니다.\n디버거의 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다.")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]Admin 권한으로 실행되어야 합니다.\n프로그램을 종료합니다.")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]호환성 모드로 프로그램이 실행중입니다.\n프로그램을 종료합니다.")
#define STR_HS_ERR_DEFAULT						_T("[H:06]해킹방지 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다.")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]드래곤네스트를 재설치 해주시기 바랍니다.\nHShield Make Response Error : %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]HackShield를 통해 비정상적인 동작을 감지하였습니다!\n 프로그램을 종료하신 후 다시 게임에 접속하시기 바랍니다.\n경고: 비공식 프로그램을 사용하면, 서비스 계약을 종료(계정 블럭) 처리할 것입니다!")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]현재 이 PC에서 SpeedHack으로 의심되는 동작이 감지되었습니다.")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]프로그램에 대하여 디버깅 시도가 발생하였습니다. (Code = %x)\n프로그램을 종료합니다.")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]매크로 기능으로 의심되는 동작이 감지되었습니다.. (Code = %x)\n프로그램을 종료합니다.")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]다음 위치에서 메모리 접근이 감지되어 프로그램을 종료시켰습니다.\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]보호 모듈에 대해서 메모리 변조가 감지되었습니다.")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]모니터링 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다.")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]핵쉴드를 정상적으로 초기화 하지 못하였습니다.\n프로그램을 종료합니다.")
#define STR_HS_UPDATE_FAIL						_T("[H:16]핵쉴드를 정상적으로 업데이트 하지 못하였습니다.\n프로그램을 종료합니다.")
#define STR_HS_START_FAIL						_T("[H:17]핵쉴드를 정상적으로 시작 하지 못하였습니다.\n프로그램을 종료합니다.")
#elif _JP 
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]他にゲームが起動しています。 \nプログラムを終了します。 ")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]間違ったファイルがインストールされています。\n再インストールの後、再度接続してください。")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]PCからデバッガの起動が感知されました。 \nデバッガの起動を停止し再度接続してください。 ")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]Adminの権限で実行しなければなりません。 \nプログラムを終了します。 ")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]プログラムが互換モードで実行中です。 \nプログラムを終了します。")
#define STR_HS_ERR_DEFAULT						_T("[H:06]不正防止機能にエラーが発生しました。 (Error Code = %x)\nプログラムを終了します。")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]ドラゴンネストを再度インストールしてください。 \nHShield Make Response Error : %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]ハックシールドを通じて非正常的な動作を感知しました。\nプログラムを終了後、再度アクセスしてください。\n警告：非公式プログラムを使用するとアカウントがブロックされます。")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]現在このPCからSpeedHackが感知されました。")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]プログラムに対するデバッギングを感知しました。(Code = %x)\nプログラムを終了します。")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]マクロ機能による不正行為が感知されました。(Code = %x)\nプログラムを終了します。")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]次の位置からメモリへの接近が感知されプログラムを終了しました。\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]保護されているモジュールに対し不正変更が感知されました。 ")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]モニターリングの機能にエラーが発生しました。 (Error Code = %x)\nプログラムを終了します。")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]HackShieldの初期化にエラーが発生しました。\nプログラムを終了します。")
#define STR_HS_UPDATE_FAIL						_T("[H:16]HackShieldのアップデートにエラーが発生しました。\nプログラムを終了します。")
#define STR_HS_START_FAIL						_T("[H:17]HackShieldの起動にエラーが発生しました。\nプログラムを終了します。")
#elif _TW
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]已有其他遊戲執行中...\n須結束目前的程式。")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]檔案安裝有誤！\n請重新安裝程式。")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]檢測到您的電腦有偵錯器正在執行除錯模式！\n請終止偵錯器後再執行。")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]該動作須以管理者(Admin)權限執行！\n須結束程式運作。")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]目前程式跟作業系統相容性有問題！\n需要把相容性設定關閉後再執行。")
#define STR_HS_ERR_DEFAULT						_T("[H:06]防外掛功能發生異常！(Error Code = %x)\n須結束程式運作。")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]請重新安裝龍之谷遊戲程式！\nHShield Make Response Error : %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]HackShield偵測到異常程式運作中！請先結束該程式後再次啟動遊戲。\n警告：使用非官方程式進行遊戲，將立即終止遊戲服務合約！")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]偵測到您的電腦有執行疑似加速器的的動作！")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]針對遊戲程式，偵測到偵錯器試圖進行除錯模式的動作(Code = %x)。\n結束該程式運作。")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]偵測到有不正常的動作進行中！(Code = %x)\n結束該程式運作。")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]在下列位置偵測到試圖接近記憶體！已結束該程式的運作。\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]偵測到對防護模組試圖進行記憶體變更的動作！")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]監控功能發生異常！(Error Code = %x)\n須結束程式運作。")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]HShield初始化失敗！\n須結束程式。")
#define STR_HS_UPDATE_FAIL						_T("[H:16]HShield更新失敗！\n須結束程式。")
#define STR_HS_START_FAIL						_T("[H:17]Hshield啟動失敗！\n須結束程式。")
#elif _US
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]Another instance of the game is already running.\nThe program will be terminated.")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]Incorrect file installed.\nPlease reinstall the game.")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]A debugger has been detected on your computer.\nPlease stop the debugger and try again.")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]Admin privileges required.\nThe program will be terminated.")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]The game is running in compatibility mode.\nThe program will be terminated.")
#define STR_HS_ERR_DEFAULT						_T("[H:06]An error occured in the hack prevention feature. (Error Code = %x)\nThe program will be terminated.")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]Please reinstall Dragon Nest.\nHShield Make Response Error: %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]Irregular activity has been detected through Hack Shield.\n Please shut down the program and restart the game.\n Warning: Any use of unauthorized programs by users may result in termination (or block) of user account!")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]A SpeedHack has been detected on this PC.")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]Debugging attempt detected. (Code = %x)\nThe program will be terminated.")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]Macro function detected. (Code = %x)\nThe program will be terminated.")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]The program will be terminated. Memory access detected in the following location:\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]Memory corruption detected in the protection module.")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]An error occurred in the monitoring feature. (Error Code = %x)\nThe program will be terminated.")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]HackShield could not be initalized properly.\nThe program will be terminated.")
#define STR_HS_UPDATE_FAIL						_T("[H:16]HackShield could not be updated properly.\nThe program will be terminated.")
#define STR_HS_START_FAIL						_T("[H:17]HackShield could not be started properly.\nThe program will be terminated.")
#elif _TH
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]เกมอื่นกำลังทำงานอยู่\nยุติโปรแกรม")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]ติดตั้งไฟล์ไม่ถูกต้อง\nกรุณาติดตั้งโปรแกรมใหม่")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]ตรวจพบการทำงาน Debugger พบคอมพิวเตอร์\nกรุณาสั่งการทำงานใหม่อีกครั้ง หลังจากหยุดการทำงานของ Debugger")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]จะต้องทำงานด้วยสิทธิ์ของ Admin\nยุติโปรแกรม")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]โปรแกรมกำลังทำงานด้วย Compatibility mode\nยุติโปรแกรม")
#define STR_HS_ERR_DEFAULT						_T("[H:06]เกิดปัญหากับฟังค์ชั่น Anti-Hacking (Error Code = %x)\nยุติโปรแกรม")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]กรุณาติดตั้ง Dragonnest ใหม่\nHShield Make Response Error : %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]ยุติโปรแกรมเนื่องจากตรวจพบ Hacking Tool ในตำแหน่งดังต่อไปนี้\n%s")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]ตรวจพบการเคลื่อนไหวที่น่าสงสัยว่าจะเป็น SpeedHack ใน PC นี้")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]เกิดความพยายามที่จะ Debugger โปรแกรม (Code = %x)\nยุติโปรแกรม")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]ตรวจพบการเคลื่อนไหวที่น่าสงสัยว่าจะเป็นฟังค์ชั่น Macro.. (Code = %x)\nยุติโปรแกรม")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]ยุติโปรแกรมเนื่องจากตรวจพบการเข้าถึงหน่วยความจำในตำแหน่งดังต่อไปนี้\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]ตรวจพบการปรับหน่วยความจำเกี่ยวกับ Protection module")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]เกิดปัญหาเกี่ยวกับฟังค์ชั่น Monitoring (Error Code = %x)\nยุติโปรแกรม")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]ไม่สามารถรีเซ็ท Hackshield ได้ตามปกติ\nยุติโปรแกรม")
#define STR_HS_UPDATE_FAIL						_T("[H:16]ไม่สามารถอัพเดท Hackshield ได้ตามปกติ\nยุติโปรแกรม")
#define STR_HS_START_FAIL						_T("[H:17]ไม่สามารถเริ่มโปรแกรม Hackshield ได้ตามปกติ\nยุติโปรแกรม")
#elif _ID
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]Game lain sedang berjalan.\nProgram akan dimatikan.")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]File yang salah terinstal.\nSilahkan instal ulang program.")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]Telah terdeteksi debugger dari komputer.\nHentikan sistem debugger dan cobalah lagi nanti.")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]Hak Admin harus dijalankan.\nProgram akan dimatikan.")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]Program sedang dijalankan dengan Compatibility Mode.\nProgram akan dimatikan.")
#define STR_HS_ERR_DEFAULT						_T("[H:06]Terjadi error pada sistem Hack Shield.(Error Code = %x)\nProgram akan dimatikan.")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]Harap instal ulang Dragon Nest.\nHShield Make Response Error : %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]Program akan dimatikan karena telah ditemukan Hacking Tool.\nLokassi : %s")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]Telah terdeteksi gerakan mencurigakan(SpeedHack) di komputer ini.")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]Telah terdeteksi percobaan debugging dalam program. (Code = %x)\nProgram akan dimatikan.")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]Telah terdeteksi gerakan mencurigakan(Macro). (Code = %x)\nProgram akan dimatikan.")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]Program akan dimatikan karena telah ditemukan gerakan mencurigakan di Memori.\nLokasi : %s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]Telah terdeteksi korupsi pada memori di Protection Module")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]Terjadi error pada sistem monitoring.(Error Code = %x)\nProgram akan dimatikan.")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]Hack Shield tidak dapat direset dengan normal.\nProgram akan dimatikan.")
#define STR_HS_UPDATE_FAIL						_T("[H:16]Hack Shield tidak dapat update dengan normal.\nProgram akan dimatikan.")
#define STR_HS_START_FAIL						_T("[H:17]Hack Shield tidak dapat dimulai dengan normal.\nProgram akan dimatikan.")
#elif _RU
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]Игра уже запущена.\nПрограмма будет закрыта.")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]Ошибка установки.\nПожалуйста, переустановите игру.")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]На вашем компьютере запущена программа отладки.\nПожалуйста, закройте ее.")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]Необходимы права администратора.\nПрограмма будет закрыта.")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]Программа запущена в режиме совместимости.\nЗавершение работы.")
#define STR_HS_ERR_DEFAULT						_T("[H:06]Ошибка программы защиты от взлома (код ошибки = %x).\nПрограмма будет закрыта.")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]Пожалуйста, переустановите игру Dragon Nest.\nОшибка программы защиты от взлома: %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]Программа защиты от взлома зафиксировала подозрительную активность.\n Пожалуйста, перезапустите программу и запустите игру заново.\n Внимание: любое использование запрещенных программ может стать причиной блокировки вашей учетной записи!")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09] На вашем компьютере обнаружена программа изменения скорости (SpeedHack).")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]Попытка запуска программы отладки (код = %x)\nПрограмма будет закрыта.")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]Обнаружено использование макросов (код = %x)\nПрограмма будет закрыта.")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]Программа будет закрыта. Обнаружено изменение памяти:\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]Изменение памяти в защищенном модуле.")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]Ошибка программы мониторинга (код ошибки = %x)\nПрограмма будет закрыта.")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]Ошибка инициализации программы защиты от взлома.\nПрограмма будет закрыта.")
#define STR_HS_UPDATE_FAIL						_T("[H:16]Ошибка обновления программы защиты от взлома.\nПрограмма будет закрыта.")
#define STR_HS_START_FAIL						_T("[H:17]Ошибка запуска программы защиты от взлома.\nПрограмма будет закрыта.")
#elif _EU
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]Another instance of the game is already running.\nThe program will be terminated.")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]Incorrect file installed.\nPlease reinstall the game.")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]A debugger has been detected on your computer.\nPlease stop the debugger and try again.")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]Admin privileges required.\nThe program will be terminated.")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]The game is running in compatibility mode.\nThe program will be terminated.")
#define STR_HS_ERR_DEFAULT						_T("[H:06]An error occured in the hack prevention feature. (Error Code = %x)\nThe program will be terminated.")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]Please reinstall Dragon Nest.\nHShield Make Response Error: %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]Irregular activity has been detected through Hack Shield.\n Please shut down the program and restart the game.\n Warning: Any use of unauthorized programs by users may result in termination (or block) of user account!")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]A SpeedHack has been detected on this PC.")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]Debugging attempt detected. (Code = %x)\nThe program will be terminated.")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]Macro function detected. (Code = %x)\nThe program will be terminated.")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]The program will be terminated. Memory access detected in the following location:\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]Memory corruption detected in the protection module.")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]An error occurred in the monitoring feature. (Error Code = %x)\nThe program will be terminated.")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]HackShield could not be initalized properly.\nThe program will be terminated.")
#define STR_HS_UPDATE_FAIL						_T("[H:16]HackShield could not be updated properly.\nThe program will be terminated.")
#define STR_HS_START_FAIL						_T("[H:17]HackShield could not be started properly.\nThe program will be terminated.")
#else 
#define STR_HS_ERR_ANOTHER_SERVICE_RUNNING		_T("[H:01]다른 게임이 실행중입니다.\n프로그램을 종료합니다.")
#define STS_HS_ERR_INVALID_FILES				_T("[H:02]잘못된 파일 설치되었습니다.\n프로그램을 재설치하시기 바랍니다.")
#define STR_HS_ERR_DEBUGGER_DETECT				_T("[H:03]컴퓨터에서 디버거 실행이 감지되었습니다.\n디버거의 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다.")
#define STR_HS_ERR_NEED_ADMIN_RIGHTS			_T("[H:04]Admin 권한으로 실행되어야 합니다.\n프로그램을 종료합니다.")
#define STR_HS_ERR_COMPATIBILITY_MODE_RUNNING	_T("[H:05]호환성 모드로 프로그램이 실행중입니다.\n프로그램을 종료합니다.")
#define STR_HS_ERR_DEFAULT						_T("[H:06]해킹방지 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다.")
#define STR_HS_DRAGONNEST_REINSTALL				_T("[H:07]드래곤네스트를 재설치 해주시기 바랍니다.\nHShield Make Response Error : %u")
#define STR_AHNHS_ENGINE_DETECT_GAME_HACK		_T("[H:08]HackShield를 통해 비정상적인 동작을 감지하였습니다!\n 프로그램을 종료하신 후 다시 게임에 접속하시기 바랍니다.\n경고: 비공식 프로그램을 사용하면, 서비스 계약을 종료(계정 블럭) 처리할 것입니다!")
#define STR_AHNHS_ACTAPC_DETECT_SPEEDHACK		_T("[H:09]현재 이 PC에서 SpeedHack으로 의심되는 동작이 감지되었습니다.")
#define STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED _T("[H:10]프로그램에 대하여 디버깅 시도가 발생하였습니다. (Code = %x)\n프로그램을 종료합니다.")
#define STR_AHNHS_ACTAPC_DETECT_AUTOMACRO		_T("[H:11]매크로 기능으로 의심되는 동작이 감지되었습니다.. (Code = %x)\n프로그램을 종료합니다.")
#define STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS _T("[H:12]다음 위치에서 메모리 접근이 감지되어 프로그램을 종료시켰습니다.\n%s")
#define STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP _T("[H:13]보호 모듈에 대해서 메모리 변조가 감지되었습니다.")
#define STR_HS_SHIELD_MONITER_ERROR				_T("[H:14]모니터링 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다.")
#define STR_HS_INITIALIZED_FAIL					_T("[H:15]핵쉴드를 정상적으로 초기화 하지 못하였습니다.\n프로그램을 종료합니다.")
#define STR_HS_UPDATE_FAIL						_T("[H:16]핵쉴드를 정상적으로 업데이트 하지 못하였습니다.\n프로그램을 종료합니다.")
#define STR_HS_START_FAIL						_T("[H:17]핵쉴드를 정상적으로 시작 하지 못하였습니다.\n프로그램을 종료합니다.")
#endif 

#endif //_HSHIELD


#ifdef _US
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"You have encountered an error in Dragon Nest, and the game must be closed. We apologize for the inconvenience.";
#define STR_DUMP_NOTICE_MSG						L"You can submit an error report to help us improve the quality of Dragon Nest.\n\nThe data you send will be kept confidential and will not include any of your personal information.";
#define STR_DUMP_SEND_BUTTON					L"Send Report";
#define STR_DUMP_NOT_SEND_BUTTON				L"Don't Send Report";
#elif _CH
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"%s 出错，而需要关闭客户端。由此给您带来麻烦，很抱歉。";
#define STR_DUMP_NOTICE_MSG						L"您提供的报错信息非常有助于提高 %s 的产品质量。\n\n您所提供的报错信息属于机密，因此我们会一律对此保密。\n\n此外，在此信息内不包括个人信息。";
#define STR_DUMP_SEND_BUTTON					L"报错";
#define STR_DUMP_NOT_SEND_BUTTON				L"取消报错";
#elif _JP
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"%s の問題でプログラムを終了します。お手数をお掛けして大変申し訳ございません。";
#define STR_DUMP_NOTICE_MSG						L"エラーの報告は%sの性能改善に役立ちます。\n\n報告なさった情報は全て秘密を保持されます。\n\n尚、お客様の個人情報は含まれて降りません。";
#define STR_DUMP_SEND_BUTTON					L"エラー報告";
#define STR_DUMP_NOT_SEND_BUTTON				L"キャンセル";
#elif _TW
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"%s 出错，而需要关闭客户端。由此给您带来麻烦，很抱歉。";
#define STR_DUMP_NOTICE_MSG						L"您提供的报错信息非常有助于提高 %s 的产品质量。\n\n您所提供的报错信息属于机密，因此我们会一律对此保密。\n\n此外，在此信息内不包括个人信息。";
#define STR_DUMP_SEND_BUTTON					L"报错";
#define STR_DUMP_NOT_SEND_BUTTON				L"取消报错";
#elif _TH
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"มีปัญหาที่ %s จะต้องยุติโปรแกรม ขออภัยที่ทำให้ไม่สะดวก";
#define STR_DUMP_NOTICE_MSG						L"การรายงานความผิดพลาดนี้จะช่วยเพิ่มคุณภาพของ %s\n\nข้อมูลที่คุณส่งมาเราจะถือว่าเป็นความลับ และจะเก็บรักษาเป็นอย่างดี\n\nนอกจากนี้เราจะไม่ใส่ข้อมูลส่วนบุคคลของคุณด้วย";
#define STR_DUMP_SEND_BUTTON					L"ส่งรายงานความผิดพลาด";
#define STR_DUMP_NOT_SEND_BUTTON				L"ไม่ส่งรายงานความผิดพลาด";
#elif _ID
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"Program akan dimatikan karena terjadi error pada %s. Mohon maaf atas ketidaknyamanan ini.";
#define STR_DUMP_NOTICE_MSG						L"Laporan %s berguna untuk meningkatkan kualitas.\n\nInformasi yang dikirim akan dirahasiahkan untuk melindungi keamanan.\n\nDan juga informasi pribadi anda tidak termasuk.";
#define STR_DUMP_SEND_BUTTON					L"Kirim laporan";
#define STR_DUMP_NOT_SEND_BUTTON				L"Cancel";
#elif _RU
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"Произошла ошибка выполнения программы. Необходимо перезапустить игру Dragon Nest. Приносим извинения за доставленные неудобства.";
#define STR_DUMP_NOTICE_MSG						L"Вы можете отправить отчет об ошибке. Это поможет нам улучшить качество предоставления услуг.\n\nВся отправленная информация конфиденциальна и не содержит ваших персональных данных.";
#define STR_DUMP_SEND_BUTTON					L"Отправить отчет";
#define STR_DUMP_NOT_SEND_BUTTON				L"Не отправлять отчет";
#elif _SG
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"You have encountered an error in Dragon Nest, and the game must be closed. We apologize for the inconvenience.";
#define STR_DUMP_NOTICE_MSG						L"Please send the error report to increase the standard of Dragon Nest.\n\nThe report will be kept confidential.\n\nYour personal information will not be included.";
#define STR_DUMP_SEND_BUTTON					L"Send error report";
#define STR_DUMP_NOT_SEND_BUTTON				L"Do not send";
#elif _EU
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"You have encountered an error in Dragon Nest, and the game must be closed. We apologize for the inconvenience.";
#define STR_DUMP_NOTICE_MSG						L"You can submit an error report to help us improve the quality of Dragon Nest.\n\nThe data you send will be kept confidential and will not include any of your personal information.";
#define STR_DUMP_SEND_BUTTON					L"Send Report";
#define STR_DUMP_NOT_SEND_BUTTON				L"Don't Send Report";
#else
#define STR_APP_TITLE							L"DragonNest"
#define STR_DUMP_NOTICE_TITLE					L"%s 에 문제가 있어 프로그램을 종료해야 합니다. 불편을 끼쳐드려서 죄송합니다.";
#define STR_DUMP_NOTICE_MSG						L"이 오류 보고는 %s 의 품질을 향상 시키는데 큰 도움이 됩니다.\n\n전달해 주신 정보는 기밀로 간주되며 보안유지를 원칙으로 합니다.\n\n또한 고객님의 개인정보는 포함 되어 있지 않습니다.";
#define STR_DUMP_SEND_BUTTON					L"오류 보고 보냄";
#define STR_DUMP_NOT_SEND_BUTTON				L"보내지 않음";
#endif

#define STR_DRAGONNEST							L"DragonNest"
