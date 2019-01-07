#include "stdafx.h"
#include <time.h>
#include "Gomoku.h"
#include "resource.h"
extern int WindowWide;
extern int WindowHigh;
extern HINSTANCE hInst;
bool Gomoku::RetPos(CHESS *Pos, Direction df){
	Pos->iChessCount = 0;
	switch (df){
	case UP:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.Y--;
		}
		Pos->pChessPos.Y++;
		break;
	case DOWN:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.Y++;
		}
		Pos->pChessPos.Y--;
		break;
	case LEFT:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.X--;
		}
		Pos->pChessPos.X++;
		break;
	case RIGHT:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.X++;
		}
		Pos->pChessPos.X--;
		break;
	case LEFT_UP:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.Y--;
			Pos->pChessPos.X--;
		}
		Pos->pChessPos.X++;
		Pos->pChessPos.Y++;
		break;
	case RIGHT_UP:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.Y--;
			Pos->pChessPos.X++;
		}
		Pos->pChessPos.X--;
		Pos->pChessPos.Y++;
		break;
	case LEFT_DOWN:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.Y++;
			Pos->pChessPos.X--;
		}
		Pos->pChessPos.Y--;
		Pos->pChessPos.X++;
		break;
	case RIGHT_DOWN:
		while (Later == iMap[Pos->pChessPos.Y][Pos->pChessPos.X]) {
			Pos->iChessCount++;
			Pos->pChessPos.Y++;
			Pos->pChessPos.X++;
		}
		Pos->pChessPos.X--;
		Pos->pChessPos.Y--;
		break;
	}
	return Pos;
}
Direction Gomoku::SetContraries(Direction df){
	Direction Contraries = UP;
	switch (df){
	case UP:
		Contraries = DOWN;
		break;
	case DOWN:
		break;
	case LEFT:
		Contraries = RIGHT;
		break;
	case RIGHT:
		Contraries = LEFT;
		break;
	case LEFT_UP:
		Contraries = RIGHT_DOWN;
		break;
	case RIGHT_UP:
		Contraries = LEFT_DOWN;
		break;
	case LEFT_DOWN:
		Contraries = RIGHT_UP;
		break;
	case RIGHT_DOWN:
		Contraries = LEFT_UP;
		break;
	}
	return Contraries;
}
Gomoku::Gomoku(HWND hWnd){
	init();
	memset(FileName_, 0, sizeof(FileName_));
	_tcscpy(Cursor, _TEXT("┏    ┓\n\n┗    ┛"));
	Rect.top = EDGE_HIGH + CHESSBOX_RECT_HIGH - CHESSBOX_RECT_HIGH / 3;
	Rect.bottom = EDGE_HIGH + CHESSBOX_RECT_HIGH + CHESSBOX_RECT_HIGH * 4;
	Rect.left = EDGE_WIDE + CHESSBOX_RECT_WIDE - CHESSBOX_RECT_WIDE / 3;
	Rect.right = EDGE_WIDE + CHESSBOX_RECT_WIDE + CHESSBOX_RECT_WIDE * 4;
	HDC hShowBmpDC = GetDC(hWnd);
	hChessBoxDC = CreateCompatibleDC(hShowBmpDC);
	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	if (hBmp) {
		SelectObject(hChessBoxDC, hBmp);
		DeleteObject(hBmp);
	}
	else{
		MessageBox(hWnd, _TEXT("加载棋盘图片失败"), _TEXT("错误"), MB_OK);
	}
	hBlackDC = CreateCompatibleDC(hShowBmpDC);
	hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	if (hBmp) {
		SelectObject(hBlackDC, hBmp);
		DeleteObject(hBmp);
	}
	else {
		MessageBox(hWnd, _TEXT("加载黑棋图片失败"), _TEXT("错误"), MB_OK);
	}
	hWhiteDC = CreateCompatibleDC(hShowBmpDC);
	hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3));
	if (hBmp) {
		SelectObject(hWhiteDC, hBmp);
		DeleteObject(hBmp);
	}
	else {
		MessageBox(hWnd, _TEXT("加载白棋图片失败"), _TEXT("错误"), MB_OK);
	}
	std::ifstream iFile(_TEXT("SaveFileCount"));
	iFileCount = 0;
	if (iFile.is_open()) {
		iFile >> iFileCount;
		iFile.close();
	}
	else{
		FILE *fp = _tfopen(_TEXT("SaveFileCount"), _TEXT("w"));//仅仅为了创建文件
		fwrite("0", 1, 1, fp);
		fclose(fp);
	}
}
Gomoku::~Gomoku(){

}

bool Gomoku::Save(HWND hWnd){
	TCHAR FileName[MAXBYTE] = { 0 };
	if (_tcscmp(FileName_, _TEXT("")) && (IDYES == MessageBox(hWnd, _TEXT("你已经加载了存档，是否覆盖该存档"), _TEXT("提示"), MB_YESNO))) {
		_tcscpy(FileName, FileName_);
	}
	else{
		_stprintf(FileName, _TEXT("SaveFile%d"), iFileCount);
		iFileCount++;
	}
	FILE *fp = _tfopen(FileName, _TEXT("w"));
	if (fp) {
		time_t a = { 0 };
		TCHAR Content[MAXBYTE] = { 0 };
		time(&a);
		tm *b = localtime(&a);
		_stprintf(Content, _TEXT("%d年%d月%d日%d:%d:%d  %s"), 1900 + b->tm_year, b->tm_mon + 1, b->tm_mday, b->tm_hour, b->tm_min, b->tm_sec, tcResult);
		fwrite(&Content, sizeof(Content), 1, fp);
		fwrite(&iChessCount_Black, sizeof(iChessCount_Black), 1, fp);
		fwrite(&iChessCount_White, sizeof(iChessCount_White), 1, fp);
		for (int i = 0; i < MAP_HIGH; i++){
			fwrite(iMap[i], sizeof(iMap[0]), 1, fp);
		}
		fclose(fp);
		MessageBox(hWnd, _TEXT("成功保存"), _TEXT("提示"), MB_OK);
		_tcscpy(FileName_, FileName);
	}
	else{
		TCHAR Content[MAXBYTE] = { 0 };
		_stprintf(Content, _TEXT("打开%s失败"), FileName);
		MessageBox(hWnd, Content, _TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
	}
	std::ofstream oFile(_TEXT("SaveFileCount"));
	if (oFile.is_open()) {
		oFile << iFileCount;
		oFile.close();
	}
	else{
		MessageBox(hWnd, _TEXT("打开SaveFileCount文件失败"), _TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
	}
	return false;
}

bool Gomoku::init(){
	Later = BLACK;
	iChessCount = 5;
	bHistorical = false;
	iChessBoxWide = 313;
	iChessBoxHigh = 313;
	iChessCount_Black = 0;//113;
	iChessCount_White = 0;//112;
	_tcscpy(tcResult, _TEXT("胜负未分"));
	for (int i = 0; i < MAP_HIGH; i++) {
		for (int j = 0; j < MAP_WIDE; j++) {
			iMap[i][j] = 0;
		}
	}
	return false;
}

int Gomoku::Mode(){
	iChessCount = (iChessCount == 5 ? 4 : 5);
	return iChessCount;
}

int Gomoku::ChessCount(){
	return iChessCount;
}

bool Gomoku::Result(CHESS Pos){
	CHESS cPos = { 0, Pos.pChessPos.X ,Pos.pChessPos.Y };
	for (int i = 0; i < 8; i++){
		cPos = Pos;
		RetPos(&cPos, (Direction)i);
		if (cPos.iChessCount >= iChessCount)return true;
		else{
			RetPos(&cPos, SetContraries((Direction)i));
			if (cPos.iChessCount >= iChessCount)return true;
		}
	}
	return false;
}

bool Gomoku::Move_Later(HWND hWnd){
	CHESS Pos = { 0 };
	int x = (Rect.left + 15) / CHESSBOX_RECT_WIDE;
	int y = (Rect.top + 15)  / CHESSBOX_RECT_HIGH;
	if (!iMap[y][x]) {
		iMap[y][x] = Later;
		Pos.pChessPos.X = x;
		Pos.pChessPos.Y = y;
		//if (BLACK == Later)iChessCount_Black--;
		//if (WHITE == Later)iChessCount_White--;
		if (BLACK == Later)iChessCount_Black++;
		if (WHITE == Later)iChessCount_White++;
		if (iChessCount_Black + iChessCount_White >= MAP_WIDE * MAP_HIGH) {
			_tcscpy(tcResult, _TEXT("平局"));
			MessageBox(hWnd, _TEXT("平局"), _TEXT("提示"), MB_OK);
			return true;
		}
		if (Result(Pos)) {
			SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
			if (WHITE == Later) {
				_tcscpy(tcResult, _TEXT("白棋胜利"));
				MessageBox(hWnd, _TEXT("白棋胜利"), _TEXT("游戏结束"), MB_OK);
			}
			else {
				_tcscpy(tcResult, _TEXT("黑棋胜利"));
				MessageBox(hWnd, _TEXT("黑棋胜利"), _TEXT("游戏结束"), MB_OK);
			}
			return true;
		}
		Later = Later == BLACK ? WHITE : BLACK;
	}
	return false;
}
bool Gomoku::DeleFile(HWND hWnd, int Index) {
	FILE *fp = nullptr;
	TCHAR FileName[MAXBYTE] = { 0 };
	TCHAR Content[MAXSHORT] = { 0 };
	_stprintf(FileName, _TEXT("SaveFile%d"), Index);
	if (DeleteFile(FileName)) {
		TCHAR Content_[MAXBYTE] = { 0 };
		_stprintf(Content_, _TEXT("删除%s成功"), FileName);
		MessageBox(hWnd, Content_, _TEXT("提示"), MB_OK);
	}
	for (int i = Index + 1; i < iFileCount; i++){
		memset(Content, 0, sizeof(Content));
		memset(FileName, 0, sizeof(FileName));
		_stprintf(FileName, _TEXT("SaveFile%d"), i);//得到删除的文件的下一个文件名
		fp = _tfopen(FileName, _TEXT("r"));
		if (fp) {
			fread(Content, sizeof(iMap) + (sizeof(TCHAR) * MAXBYTE) * 3, 1, fp);
			fclose(fp);
		}
		if (DeleteFile(FileName)) {
			memset(FileName, 0, sizeof(FileName));
			_stprintf(FileName, _TEXT("SaveFile%d"), i - 1);//得到删除的文件的下一个文件名
			fp = _tfopen(FileName, _TEXT("w"));
			if (fp) {
				fwrite(Content, sizeof(Content), 1, fp);
				fclose(fp);
			}
		}
		else{
			TCHAR Content_[MAXBYTE] = { 0 };
			_stprintf(Content_, _TEXT("删除%s失败，请手动删除"), FileName);
			MessageBox(hWnd, Content_, _TEXT("提示"), MB_OK);
		}
	}
	iFileCount--;
	std::ofstream oFile(_TEXT("SaveFileCount"));
	if (oFile.is_open()) {
		oFile << iFileCount;
	}
	return false;
}
bool Gomoku::ReadFile(HWND hWnd, TCHAR *FileName) {
	TCHAR Content[MAXBYTE] = { 0 };
	FILE *fp = _tfopen(FileName, _TEXT("r"));
	if (fp) {
		_tcscpy(FileName_, FileName);
		fread(&Content, sizeof(Content), 1, fp);
		fread(&iChessCount_Black, sizeof(iChessCount_Black), 1, fp);
		fread(&iChessCount_White, sizeof(iChessCount_White), 1, fp);
		for (int i = 0; i < MAP_HIGH; i++) {
			fread(iMap[i], sizeof(iMap[0]), 1, fp);
		}
		fclose(fp);
	}
	else {
		TCHAR Content[MAXBYTE] = { 0 };
		_stprintf(Content, _TEXT("打开%s失败"), FileName);
		MessageBox(hWnd, Content, _TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
	}
	return false;
}

bool Gomoku::MoveCursor(Direction DF){
	switch (DF){
	case UP:
		Rect.top -= CURSOR_MOVE;
		Rect.bottom -= CURSOR_MOVE;
		break;
	case DOWN:
		Rect.top += CURSOR_MOVE;
		Rect.bottom += CURSOR_MOVE;
		break;
	case LEFT:
		Rect.left -= CURSOR_MOVE;
		Rect.right -= CURSOR_MOVE;
		break;
	case RIGHT:
		Rect.left += CURSOR_MOVE;
		Rect.right += CURSOR_MOVE;
		break;
	default:
		break;
	}
	return false;
}

BOOL Gomoku::Show(HDC hLoadBmpDC){
	TCHAR Content[MAXBYTE] = { 0 };
	SetBkMode(hLoadBmpDC, TRANSPARENT);
	_stprintf(Content, _TEXT("黑棋：%d   白棋%d"), iChessCount_Black, iChessCount_White);
	TransparentBlt(hLoadBmpDC, 0, 0, iChessBoxWide, iChessBoxHigh, hChessBoxDC, 0, 0, iChessBoxWide, iChessBoxHigh, RGB(255, 255, 255));//棋盘大小被写死
	TextOut(hLoadBmpDC, EDGE_WIDE, 0, Content, _tcslen(Content));
	for (int i = 0; i < MAP_HIGH; i++){
		for (int j = 0; j < MAP_WIDE; j++) {
			if (WHITE == iMap[i][j])TransparentBlt(hLoadBmpDC, EDGE_WIDE / 2 + (j * CHESSBOX_RECT_WIDE), EDGE_HIGH / 2 + (i * CHESSBOX_RECT_HIGH), 16, 16, hWhiteDC, 0, 0, 16, 16, RGB(0, 0, 0));
			if (BLACK == iMap[i][j])TransparentBlt(hLoadBmpDC, EDGE_WIDE / 2 + (j * CHESSBOX_RECT_WIDE), EDGE_HIGH / 2 + (i * CHESSBOX_RECT_HIGH), 16, 16, hBlackDC, 0, 0, 16, 16, RGB(255, 0, 0));
		}
	}
	DrawText(hLoadBmpDC, Cursor, _tcslen(Cursor), &Rect, DT_END_ELLIPSIS);
	return true;
}

bool Gomoku::CanMoveCursor(Direction DF){
	switch (DF){
	case UP:
		if (Rect.top - CURSOR_MOVE > 0 - EDGE_HIGH / 2)return true;
		break;
	case DOWN:
		if (Rect.bottom + CURSOR_MOVE < iChessBoxHigh + EDGE_HIGH * 3)return true;
		break;
	case LEFT:
		if (Rect.left - CURSOR_MOVE > 0 - EDGE_HIGH / 2)return true;
		break;
	case RIGHT:
		if (Rect.right + CURSOR_MOVE < iChessBoxHigh + EDGE_WIDE * 3)return true;
	default:
		break;
	}
	return false;
}

bool Gomoku::ShowHistorical(HDC hLoadBmpDC){
	FILE *fp = nullptr;
	RECT rRect = { 0 };
	TCHAR Content[MAXSHORT] = { 0 };
	TCHAR Content_[MAXBYTE] = { 0 };
	TCHAR FileName[MAXBYTE] = { 0 };
	if (bHistorical) {
		rRect.top = 0;
		rRect.left = 0;
		rRect.right = WindowWide;
		rRect.bottom = WindowHigh;
		for (int i = 0; i < iFileCount; i++){
			memset(Content_, 0, sizeof(Content_));
			memset(FileName, 0, sizeof(FileName));
			_stprintf(FileName, _TEXT("SaveFile%d"), i);
			fp = _tfopen(FileName, _TEXT("r"));
			if (fp) {
				fread(Content_, sizeof(Content_), 1, fp);
				fclose(fp);
				_tcscat(Content, _tcscat(Content_, _TEXT("\n")));
			}
		}
	}
	DrawText(hLoadBmpDC, Content, _tcslen(Content), &rRect, DT_END_ELLIPSIS);
	return false;
}

bool Gomoku::Move_Later(HWND hWnd, LPARAM lParam){
	CHESS Pos = { 0 };
	int X = LOWORD(lParam);
	int Y = HIWORD(lParam);
	//在棋盘内点击
	int x = X / CHESSBOX_RECT_WIDE;
	int y = Y / CHESSBOX_RECT_HIGH;
	if (!iMap[y][x]) {
		iMap[y][x] = Later;
		Pos.pChessPos.X = x;
		Pos.pChessPos.Y = y;
		//if (BLACK == Later)iChessCount_Black--;
		//if (WHITE == Later)iChessCount_White--;
		if (BLACK == Later)iChessCount_Black++;
		if (WHITE == Later)iChessCount_White++;
		if (iChessCount_Black + iChessCount_White >= MAP_WIDE * MAP_HIGH) {
			MessageBox(hWnd, _TEXT("平局"), _TEXT("提示"), MB_OK);
			return true;
		}
		if (Result(Pos)) {
			SendMessage(hWnd, WM_ERASEBKGND, 0, 0);
			if (WHITE == Later) {
				MessageBox(hWnd, _TEXT("白棋胜利"), _TEXT("游戏结束"), MB_OK);
			}
			else{
				MessageBox(hWnd, _TEXT("黑棋胜利"), _TEXT("游戏结束"), MB_OK);
			}
			return true;
		}
		Later = Later == BLACK ? WHITE : BLACK;
	}
	return false;
}
