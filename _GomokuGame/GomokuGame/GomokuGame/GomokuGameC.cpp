#include "stdafx.h"
#include "Gomoku.h"
#include "Bitmap.h"
#include "resource.h"
#include <shellapi.h>
#include "GomokuGame.h"
#define OPTIONS_Y	1
#define OPTIONS_HIGHT	15
#define OPTIONS_MOVE	1
#define BUTTON_COUNT	8
Bitmap *bBack;
Gomoku *Gomoku_;
Bitmap *Options;
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
	if (bOldResult[4]->Visible()) {//如果在加载按钮存在的情况下点击返回按钮
		bBack->Visible(true);
		Options->Visible(false);
		Gomoku_->Historical(false);
		bOldResult[4]->Visible(false);
		bOldResult[5]->Visible(false);
		bOldResult[6]->Visible(true);//五连子按钮
		for (int i = 0; i < 4; i++) {
			bOldResult[i]->Visible(true);
		}
	}
	else{
		Options->Visible(false);
		bBack->Visible(false);
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
bool DeleteFile_MouseDown(HWND hWnd){
	int Index = Options->Y() / OPTIONS_HIGHT;
	Gomoku_->DeleFile(hWnd, Index);
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool Historical_MouseDown(HWND hWnd){
	Options->Y(OPTIONS_Y);//如果玩家（没错，说的就是你!)要求每次都停在上次选择加载的位置则只需将此代码删除即可
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

bool MouseLeft_Dowm(HWND hWnd, LPARAM lParam){
	bool(*Fun[])(HWND) = { 
		Save_MouseDown,
		Restart_MouseDown,
		Historical_MouseDown,
		Return_MouseDown,
		LoadFile_MouseDown,
		DeleteFile_MouseDown,
		Mode_MouseDown,
		Mode_MouseDown
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
	if (!Options->Visible()) {
		bBack->Visible(true);
		bOldResult[Gomoku_->ChessCount() == 5 ? 6 : 7]->Visible(true);
		for (int i = 0; i < 4; i++) {
			bOldResult[i]->Visible(true);
		}
	}
	SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
	return false;
}
bool ShowInterface(HWND hWnd, HDC hDC){
	const TCHAR *ButtonContent[] = { 
		_TEXT("保存"),_TEXT("再来一局"),_TEXT("历史对局"),
		_TEXT("返回"),_TEXT("加载") ,_TEXT("删除"),
		_TEXT("五连子"),_TEXT("四连子")
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
		if (!Options->Visible()) {
			bBack->Visible(true);
			bOldResult[Gomoku_->ChessCount() == 5 ? 6 : 7]->Visible(true);
			for (int i = 0; i < 4; i++) {
				bOldResult[i]->Visible(true);
			}
		}
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
