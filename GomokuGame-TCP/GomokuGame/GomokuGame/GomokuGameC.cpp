#include "stdafx.h"
#include "Gomoku.h"
#include "Bitmap.h"
#include "resource.h"
#include <shellapi.h>
#include "GomokuGame.h"
#define DEBUG_	1
#if !DEBUG_
#include <io.h>
#endif
#define OPTIONS_Y	1
#define OPTIONS_HIGHT	15
#define OPTIONS_MOVE	1
#define BUTTON_COUNT	9
Bitmap *bBack;
Gomoku *Gomoku_;
Bitmap *Options;
CLIENT_DATA Client;
Bitmap *bOldResult[BUTTON_COUNT];//再来一局，保存，以前的对局，以前的结果，返回
extern int WindowWide;
extern int WindowHigh;
bool Save_MouseDown(HWND hWnd){
	return Gomoku_->Save(hWnd);
}
bool Mode_MouseDown(HWND hWnd){
	int a = Gomoku_->Mode();
	if (a == 5) {
		bOldResult[6]->Visible(true);
		bOldResult[7]->Visible(false);
	}
	else{
		bOldResult[7]->Visible(true);
		bOldResult[6]->Visible(false);
	}
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return true;
}
bool Return_MouseDown(HWND hWnd){
	char Content[MAXBYTE] = { 0 };
	sprintf(Content, "%d\n%d\n", CLIENT_CLOSE_, Client.Pos);
	if (bOldResult[4]->Visible() || (Options->Visible() && !bOldResult[4]->Visible())) {//如果在加载按钮存在的情况下点击返回按钮
		if (bOldResult[4]->Visible() || (IDYES == MessageBox(hWnd, _TEXT("返回后将退出等待"), _TEXT("提示"), MB_YESNO) && send(Client.Socket, Content, strlen(Content), 0))) {
			bBack->Visible(true);
			Options->Visible(false);
			Gomoku_->Historical(false);
			bOldResult[4]->Visible(false);
			bOldResult[5]->Visible(false);
			bOldResult[8]->Visible(true);
			bOldResult[Gomoku_->ChessCount() == 5 ? 6 : 7]->Visible(true);
			for (int i = 0; i < 4; i++) {
				bOldResult[i]->Visible(true);
			}
		}
	}
	else{
		bBack->Visible(false);
		Options->Visible(false);
		for (int i = 0; i < BUTTON_COUNT; i++) {
			bOldResult[i]->Visible(false);
		}
	}
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool Restart_MouseDown(HWND hWnd){
	if (IDYES == MessageBox(hWnd, _TEXT("是否保存当前对局"), _TEXT("提示"), MB_YESNO)) {
		Gomoku_->Save(hWnd);
	}
	Gomoku_->init();
	bBack->Visible(false);
	for (int i = 0; i < BUTTON_COUNT; i++) {
		bOldResult[i]->Visible(false);
	}
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool LoadFile_MouseDown(HWND hWnd){
	bBack->Visible(false);
	Options->Visible(false);
	Gomoku_->Historical(false);
	for (int i = 0; i < BUTTON_COUNT; i++) {
		bOldResult[i]->Visible(false);
	}
	int Index = Options->Y() / OPTIONS_HIGHT;
	TCHAR FileName[MAXBYTE] = { 0 };
	_stprintf(FileName, _TEXT("SaveFile%d"), Index);
	Gomoku_->ReadFile(hWnd, FileName);
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool ConnectServer_MouseDown(HWND hWnd){
	WSADATA WSAData;
	int SockError = 0;
	char ServerIP[MAXCHAR] = { 0 };
#if DEBUG_
	char *Content = nullptr;
	system("ipconfig > a");
	FILE *fp = fopen("a", "r");
	if (fp) {
		int FileSize = GetFileLen("a");
		Content = (char *)malloc(FileSize + 1);
		memset(Content, 0, FileSize + 1);
		fread(Content, FileSize, 1, fp);
		fclose(fp);
		strcpy(ServerIP, GetLine(Content, 10));
		char *p = nullptr;
		char *lpStart = ServerIP;
		MovePointer_Char(&lpStart, ':', TRUE); lpStart += 2;
		p = lpStart;
		MovePointer_Char(&p, '\n', TRUE);
		*p = 0;
		strcpy(ServerIP, lpStart);
		system("del a");
	}
	free(Content);
#else
	if (-1 == _access("ServerIP", 0)) {
		//文件不存在
		MessageBox(hWnd, TEXT("缺少ServerIP文件"), TEXT("错误"), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	else {
		if (!GetFileContent(ServerIP, "ServerIP")) {
			MessageBox(hWnd, TEXT("ServerIP文件中没有数据"), TEXT("错误"), MB_ICONEXCLAMATION | MB_OK);
			return false;
		}
	}
#endif
	SOCKADDR_IN ServerAddr = { 0 };
	SockError = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (-1 == SockError) {
		ShowError(hWnd, WSAGetLastError(), _TEXT("初始化Socket时:"));
		return false;
	}
	Client.Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(10086);
	ServerAddr.sin_addr.s_addr = inet_addr(ServerIP);
	if (SOCKET_ERROR == connect(Client.Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr))) {
		ShowError(hWnd, WSAGetLastError(), _TEXT("连接服务器时:"));
		return false;
	}
	CreateThread(nullptr, 0, Respond, hWnd, 0, 0);
	bBack->Visible(true);
	for (int i = 0; i < BUTTON_COUNT; i++) {
		bOldResult[i]->Visible(false);
	}
	bOldResult[3]->Visible(true);
	Options->Visible(true);
	return true;
}
void ShowError(HWND hWnd, DWORD dwErrNo, const TCHAR *lpszFunction) {
	LPVOID lpMsgBuf;
	//	memset(lpMsgBuf, 0, sizeof(lpMsgBuf));
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   //   Default   language   
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	wchar_t Message[500] = { 0 };
	memset(Message, 0, sizeof(Message));
	wcscat(wcscat(Message, lpszFunction), (LPTSTR)lpMsgBuf);
	MessageBox(NULL, Message, L"Error", MB_OK);
}
const char *GetFileContent(char *Content, const char *FileName) {
	FILE *fp = fopen(FileName, "r");
	if (!fp)return nullptr;
	fread(Content, 1, GetFileLen(FileName), fp);
	fclose(fp);
	return Content;
}
BOOL MovePointer_Char(char **lpStart, char ch, BOOL df) {
	if (!lpStart || !(*lpStart))return FALSE;
	if (df) {
		while (*(*lpStart) && *(*lpStart) != ch && (*lpStart)++);
	}
	else {
		while (*(*lpStart) && *(*lpStart) != ch && (*lpStart)--);
	}
	if (*lpStart)
		return TRUE;
	return FALSE;
}
char *GetLine(char *FileContent, int Line) {
	int iLine = 1;
	char *p = nullptr;
	char *lpStart = nullptr;
	for (lpStart = FileContent; *lpStart;) {
		p = lpStart;
		MovePointer_Char(&p, '\n', TRUE);
		if (*p)iLine++;
		*p++ = 0;
		if (Line == iLine)break;
		lpStart = p;
	}
	return lpStart;
}
int GetFileLen(const char *FileName) {
	int Size = 0;
	FILE *fp = fopen(FileName, "r");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		Size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	return Size;
}
bool DeleteFile_MouseDown(HWND hWnd){
	int Index = Options->Y() / OPTIONS_HIGHT;
	Gomoku_->DeleFile(hWnd, Index);
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool Historical_MouseDown(HWND hWnd){
	Options->Y(OPTIONS_Y);//如果用户要求每次都停在上次选择加载的位置则只需将此代码删除即可
	Options->Visible(true);
	bOldResult[4]->Visible(true);
	bOldResult[5]->Visible(true);
	for (int i = 6; i < 8; i++){
		bOldResult[i]->Visible(false);
	}
	for (int i = 0; i < 3; i++){
		bOldResult[i]->Visible(false);
	}
	Gomoku_->Historical(true);
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool Menu(){
	if (!Options->Visible()) {
		bBack->Visible(true);
		if(!Client.Socket)bOldResult[8]->Visible(true);
		bOldResult[Gomoku_->ChessCount() == 5 ? 6 : 7]->Visible(true);
		for (int i = 0; i < 4; i++)bOldResult[i]->Visible(true);
	}
	return false;
}
bool WindowCreate(HWND hWnd){
	Gomoku_ = new Gomoku(hWnd);
	Options = new Bitmap(hWnd, 0, OPTIONS_Y, WindowWide, OPTIONS_HIGHT, IDB_BITMAP6);
	bBack = new Bitmap(hWnd, 0, 0, WindowWide, WindowHigh, IDB_BITMAP4);
	bOldResult[0] = new Bitmap(hWnd, 5, 5, _tcslen(_TEXT("保存")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[1] = new Bitmap(hWnd, 5, 45, _tcslen(_TEXT("再来一局")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[2] = new Bitmap(hWnd, 5, 80, _tcslen(_TEXT("历史对局")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[3] = new Bitmap(hWnd, 5, WindowHigh - 70, _tcslen(_TEXT("返回")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[4] = new Bitmap(hWnd, 60, WindowHigh - 70, _tcslen(_TEXT("加载")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[5] = new Bitmap(hWnd, 115, WindowHigh - 70, _tcslen(_TEXT("删除")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[6] = new Bitmap(hWnd, 5, 115, _tcslen(_TEXT("五连子")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[7] = new Bitmap(hWnd, 5, 115, _tcslen(_TEXT("四连子")) * 15 + 20, 30, IDB_BITMAP5);
	bOldResult[8] = new Bitmap(hWnd, 5, 150, _tcslen(_TEXT("连接服务器")) * 15 + 20, 30, IDB_BITMAP5);
	DragAcceptFiles(hWnd, TRUE);
	return false;
}
bool WindowClose(HWND hWnd){
	if (IDYES == MessageBox(hWnd, _TEXT("是否退出游戏？"), _TEXT("提示"), MB_YESNO)) {
		if (IDYES == MessageBox(hWnd, _TEXT("是否存档"), _TEXT("提示"), MB_YESNO)) {
			Gomoku_->Save(hWnd);
		}
		delete bBack;
		delete Gomoku_;
		delete Options;
		for (int i = 0; i < BUTTON_COUNT; i++){
			delete bOldResult[i];
		}
		SendMessage(hWnd, WM_DESTROY, 0, 0);
	}
	return false;
}
char *Analysis(char *Buff, char ch) {
	if (!Buff)return nullptr;
	char *lpEnd = strchr(Buff, ch);
	if (lpEnd) {
		char *lpStart = Buff;
		char *Buf = (char *)malloc(strlen(Buff) + 1);
		memset(Buf, 0, strlen(Buff) + 1);
		*lpEnd = 0;
		strcpy(Buf, lpStart);
		lpEnd++;
		strcat(strcat(strcpy(Buff, lpEnd), "\\"), Buf);
		//到这里说明已经将我们需要的字符串放到最后面了
		lpStart = strchr(Buff, '\\');
		*lpStart = 0;
		free(Buf);
		return ++lpStart;
	}
	return Buff;
}
bool Process(HWND hWnd, char *Content) {
	int a = atoi(Analysis(Content, '\n'));
	if (a == GAME_START) {
		Gomoku_->init();
		bBack->Visible(false);
		Options->Visible(false);
		for (int i = 0; i < BUTTON_COUNT; i++) {
			bOldResult[i]->Visible(false);
		}
		SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	}
	else if (a == PLAYER_ID) {
		Client.Pos = atoi(Analysis(Content, '\n'));
	}
	else if (a == CLIENT_CLOSE_) {
		MessageBox(hWnd, _TEXT("游戏结束"), _TEXT("提示"), MB_OK);
		SendMessage(hWnd, WM_DESTROY, 0, 0);
	}
	else{
		int Pos = atoi(Analysis(Content, '\n'));
		int X = atoi(Analysis(Content, '\n'));
		int Y = atoi(Analysis(Content, '\n'));
		if (Gomoku_->Move_Later(hWnd, Pos, X, Y)) {
			SendMessage(hWnd, WM_DESTROY, 0, 0);
		}
		SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	}
	return true;
}
DWORD WINAPI Respond(LPVOID Parameter) {
	HWND hWnd = (HWND)Parameter;
	char Content[MAXWORD] = { 0 };
	while (recv(Client.Socket, Content, sizeof(Content), 0) > 0) {
		if (!Process(hWnd, Content)) {
			break;
		}
		_strset(Content, 0);
	}
	ShowError(hWnd, WSAGetLastError(), L"接收客户端数据时:");
	return TRUE;
}
bool MouseLeft_Dowm(HWND hWnd, LPARAM lParam){
	bool(*Fun[])(HWND) = { 
		Save_MouseDown,
		Restart_MouseDown,
		Historical_MouseDown,
		Return_MouseDown,
		LoadFile_MouseDown,
		DeleteFile_MouseDown,
		Mode_MouseDown,
		Mode_MouseDown,
		ConnectServer_MouseDown
	};
	if (!bBack->Visible()) {
		if (Gomoku_->Move_Later(hWnd, lParam)) {
			Gomoku_->Save(hWnd);
			if (IDYES == MessageBox(hWnd, _TEXT("是否在来一局"), _TEXT("提示"), MB_YESNO)) {
				Gomoku_->init();
				SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
			}
			//else {
			//	SendMessage(hWnd, WM_DESTROY, 0, 0);
			//}
		}
	}
	else{
		for (int i = 0; i < BUTTON_COUNT; i++){
			if (bOldResult[i]->Mouse_Down(lParam, Fun[i]))break;
		}
	}
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool MouseRight_Dowm(HWND hWnd, LPARAM lParam){
	Menu();
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool ShowInterface(HWND hWnd, HDC hDC){
	const TCHAR *ButtonContent[] = { 
		_TEXT("保存"),_TEXT("再来一局"),_TEXT("历史对局"),
		_TEXT("返回"),_TEXT("加载") ,_TEXT("删除"),
		_TEXT("五连子"),_TEXT("四连子"),_TEXT("连接服务器")
	};
	HDC hLoadBmpDC = CreateCompatibleDC(hDC);
	HBITMAP hBmp = CreateCompatibleBitmap(hDC, WindowWide, WindowHigh);
	SelectObject(hLoadBmpDC, hBmp);
	DeleteObject(hBmp);
	Gomoku_->Show(hLoadBmpDC);
	bBack->Show(hLoadBmpDC, 6, 6);//后面的参数为图片实际大小,只要是位图类成员函数都是如此
	Options->Show(hLoadBmpDC, 6, 6);
	Gomoku_->ShowHistorical(hLoadBmpDC);
	for (int i = 0; i < BUTTON_COUNT; i++){
		bOldResult[i]->Show(hLoadBmpDC, 442, 162);
		if (bOldResult[i]->Visible()) {
			TextOut(hLoadBmpDC, bOldResult[i]->X() + 10, bOldResult[i]->Y() + 5, ButtonContent[i], _tcslen(ButtonContent[i]));
		}
	}
	if (Options->Visible() && !bOldResult[4]->Visible()) {
		TextOut(hLoadBmpDC, 0, 0, _TEXT("服务器正在等待另一名玩家加入"), _tcslen(_TEXT("服务器正在等待另一名玩家加入")));
		TextOut(hLoadBmpDC, 0, 15, _TEXT("另一名玩家加入后将自动开始游戏"), _tcslen(_TEXT("另一名玩家加入后将自动开始游戏")));
	}
	BitBlt(hDC, 0, 0, WindowWide, WindowHigh, hLoadBmpDC, 0, 0, SRCCOPY);
	DeleteDC(hLoadBmpDC);
	ReleaseDC(hWnd, hDC);
	return false;
}

bool Key_Down(HWND hWnd, WPARAM wParam){
	switch (wParam){//由于windows没有定义字母与数字的键值，所以目前先用数字
	case 0x57://w
		if (Gomoku_->CanMoveCursor(UP)) {
			Gomoku_->MoveCursor(UP);
		}
		break;
	case 0x53://s
		if (Gomoku_->CanMoveCursor(DOWN)) {
			Gomoku_->MoveCursor(DOWN);
		}
		break;
	case 0x41://a
		if (Gomoku_->CanMoveCursor(LEFT)) {
			Gomoku_->MoveCursor(LEFT);
		}
		break;
	case 0x44://d
		if (Gomoku_->CanMoveCursor(RIGHT)) {
			Gomoku_->MoveCursor(RIGHT);
		}
		break;
	//case 0x5a://z
	//	if ((wParam & MK_CONTROL) == VK_CONTROL) {
	//		MessageBox(hWnd, _TEXT("Ctrl + z"), _TEXT("提示"), MB_OK);
	//	}
	//	break;
	case 0x52://r
		Menu();
		break;
	case VK_ESCAPE:
		if (bBack->Visible())Return_MouseDown(hWnd);
		break;
	case VK_BACK:
		if (bBack->Visible())Return_MouseDown(hWnd);
		break;
	case VK_UP:
		if (Options->Visible()) {
			if (Options->Y() - OPTIONS_HIGHT > -5)Options->Y(Options->Y() - OPTIONS_HIGHT - OPTIONS_MOVE);
		}
		else{
			if (Gomoku_->CanMoveCursor(UP)) {
				Gomoku_->MoveCursor(UP);
			}
		}
		break;
	case VK_DOWN:
		if (Options->Visible()) {
			if(Options->Y() + OPTIONS_HIGHT <= Gomoku_->FileCount() * OPTIONS_HIGHT) Options->Y(Options->Y() + OPTIONS_HIGHT + OPTIONS_MOVE);
		}
		else {
			if (Gomoku_->CanMoveCursor(DOWN)) {
				Gomoku_->MoveCursor(DOWN);
			}
		}
		break;
	case VK_LEFT:
		if (Gomoku_->CanMoveCursor(LEFT)) {
			Gomoku_->MoveCursor(LEFT);
		}
		break;
	case VK_RIGHT:
		if (Gomoku_->CanMoveCursor(RIGHT))Gomoku_->MoveCursor(RIGHT);
		break;
	case VK_DELETE:
		if (Options->Visible())DeleteFile_MouseDown(hWnd);
		break;
	case VK_SPACE:
		if (!bBack->Visible() && Gomoku_->Move_Later(hWnd)) {
			Gomoku_->Save(hWnd);
			if (IDYES == MessageBox(hWnd, _TEXT("是否在来一局"), _TEXT("提示"), MB_YESNO)) {
				Gomoku_->init();
				SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
			}
			//else {
			//	SendMessage(hWnd, WM_DESTROY, 0, 0);
			//}
		}
		break;
	case VK_RETURN:
		if (Options->Visible()) {
			LoadFile_MouseDown(hWnd);
		}
		break;
	default:
		break;
	}
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}

bool DropFiles(HWND hWnd, WPARAM wParam){
	HDROP hDrop = (HDROP)wParam;
	TCHAR FileName[MAXBYTE] = { 0 };
	DragQueryFile(hDrop, 0, FileName, sizeof(FileName));
	DragFinish(hDrop);
	Gomoku_->ReadFile(hWnd, FileName);
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
