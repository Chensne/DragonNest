extern std::string ip;
extern std::string port;
extern std::string srvPassword;


extern HINSTANCE gHInst;
extern int gCmdShow;

DWORD WINAPI InitWindow(LPVOID);
BOOL CALLBACK WindowController(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void PlayButtonPress();