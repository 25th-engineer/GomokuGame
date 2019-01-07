#include "stdafx.h"
#include "Bitmap.h"
#include "resource.h"
extern HINSTANCE hInst;
bool Bitmap::May(LPARAM lParam){
	int X = LOWORD(lParam);
	int Y = HIWORD(lParam);
	if (X > iX && X < iX + iWide && Y > iY && Y < iY + iHigh)return true;
	return false;
}
Bitmap::Bitmap(HWND hWnd, int iX, int iY, int iWide, int iHigh, int BmpID) :hWnd(hWnd), iX(iX), iY(iY), iWide(iWide), iHigh(iHigh) {
	bVisible = false;
	hLoadBmpDC = CreateCompatibleDC(GetDC(hWnd));
	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(BmpID));
	if (hBmp) {
		SelectObject(hLoadBmpDC, hBmp);
		DeleteObject(hBmp);
	}
}

Bitmap::Bitmap(HWND hWnd, int iX, int iY, int iWide, int iHight,const TCHAR BmpName){

}


Bitmap::~Bitmap(){

}

bool Bitmap::Mouse_Down(LPARAM lParam, bool(*MD)(HWND hWnd)){
	if (May(lParam)) {
		return MD(hWnd);
	}
	return false;
}

BOOL Bitmap::Show(HDC hLoadBmpDC, int iWide_, int iHigh_, UINT crTransparent) {
	if (bVisible) {
		TransparentBlt(hLoadBmpDC, iX, iY, iWide, iHigh, this->hLoadBmpDC, 0, 0, iWide_, iHigh_, crTransparent);
	}
	return FALSE;
}
