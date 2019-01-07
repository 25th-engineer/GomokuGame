#pragma once
class Bitmap{
	int iX;
	int iY;
	int iWide;
	int iHigh;
	HWND hWnd;
	bool bVisible;
	HDC hLoadBmpDC;
	bool May(LPARAM lParam);
	//BMP_INFO BmpInf;
public:
	Bitmap(HWND hWnd, int iX, int iY, int iWide, int iHight, int BmpID);
	Bitmap(HWND hWnd, int iX, int iY, int iWide, int iHight, const TCHAR BmpName);
	~Bitmap();
	inline int X();
	inline int Y();
	inline int Wide();
	inline int High();
	inline int X(int iX);
	inline int Y(int iY);
	inline bool Visible();
	inline int Wide(int iWide);
	inline int High(int iHigh);
	inline bool Visible(bool bVisible);
	bool Mouse_Down(LPARAM lParam, bool(*MD)(HWND hWnd));
	BOOL Show(HDC hLoadBmpDC, int iWide_, int iHigh_, UINT crTransparent = RGB(255, 255, 257));
};
inline int Bitmap::X() {
	return iX;
}

inline int Bitmap::Y() {
	return iY;
}
inline int Bitmap::Wide() {
	return iWide;
}

inline int Bitmap::High() {
	return iHigh;
}

inline int Bitmap::X(int iX) {
	return this->iX = iX;
}

inline int Bitmap::Y(int iY) {
	return this->iY = iY;
}


inline int Bitmap::Wide(int iWide) {
	return this->iWide = iWide;
}

inline int Bitmap::High(int iHigh) {
	return this->iHigh = iHigh;
}

inline bool Bitmap::Visible() {
	return bVisible;
}

inline bool Bitmap::Visible(bool bVisible) {
	return this->bVisible = bVisible;
}

