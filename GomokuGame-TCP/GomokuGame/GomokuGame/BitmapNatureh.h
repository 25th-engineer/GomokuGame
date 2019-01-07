#ifndef _GETBITMAPNATUREH_H_
#define _GETBITMAPNATUREH_H_
#pragma pack(2)
//#include"stdafx.h"
#ifdef __cplusplus
extern "C" {
#endif
	///*定义WORD为两个字节的类型*/
	//typedef unsigned short WORD;
	///*定义DWORD为e四个字节的类型*/
	//typedef unsigned long DWORD;

	/*位图文件头*/
	typedef struct BMP_FILE_HEADER{
		WORD bType; /* 文件标识符 */
		DWORD bSize; /* 文件的大小 */
		WORD bReserved1; /* 保留值,必须设置为0 */
		WORD bReserved2; /* 保留值,必须设置为0 */
		DWORD bOffset; /* 文件头的最后到图像数据位开始的偏移量 */
	} BMPFILEHEADER;

	/*位图信息头*/
	typedef struct BMP_INFO{
		DWORD bInfoSize; /* 信息头的大小 */
		DWORD bWidth; /* 图像的宽度 */
		DWORD bHeight; /* 图像的高度 */
		WORD bPlanes; /* 图像的位面数 */
		WORD bBitCount; /* 每个像素的位数 */
		DWORD bCompression; /* 压缩类型 */
		DWORD bmpImageSize; /* 图像的大小,以字节为单位 */
		DWORD bXPelsPerMeter; /* 水平分辨率 */
		DWORD bYPelsPerMeter; /* 垂直分辨率 */
		DWORD bClrUsed; /* 使用的色彩数 */
		DWORD bClrImportant; /* 重要的颜色数 */
	} BMPINF;

	/*彩色表*/
	typedef struct RGB_QUAD {
		WORD rgbBlue; /* 蓝色强度 */
		WORD rgbGreen; /* 绿色强度 */
		WORD rgbRed; /* 红色强度 */
		WORD rgbReversed; /* 保留值 */
	} RGBQUAD_MY;
	int __stdcall GetBitmapNature(const char *BmpName, BMPINF *lpBmpInf, int iBmpInfSize);
	int __stdcall GetBitmapRGB(const char *BmpName, RGBQUAD_MY *lpRgbQuad, int lpRgbQuadSize);
#ifdef __cplusplus
}
#endif
#endif