#include "stdafx.h"
#include "JolJakDlg.h"

void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);

#define MAXDIFF(a,b) ((a)>(b) ? (a):(b))

void CJolJakDlg::displayImage(Mat img_arr, int item) //OpenCV의 Mat 이미지를 MFC의 DC로 그리게 하는 함수.
{
	IplImage *srcimg = &IplImage(img_arr);
	CWnd *pWnd = GetDlgItem(item);
	CDC *pDCc = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(&rect);
	IplImage *img = cvCreateImage(cvSize(rect.Width(), rect.Height()), srcimg->depth, srcimg->nChannels);
	cvResize(srcimg, img);
	uchar buffer[sizeof(BITMAPINFOHEADER) * 1024];
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	int bmp_w = 0;
	int bmp_h = 0;
	int bpp = 0;
	bmp_w = img->width;
	bmp_h = img->height;
	bpp = (img->depth & 255)*img->nChannels;
	FillBitmapInfo(bmi, bmp_w, bmp_h, bpp, img->origin);
	int from_x = 0;
	int from_y = 0;
	int sw = 0;
	int sh = 0;
	from_x = MIN(0, bmp_w - 1);
	from_y = MIN(0, bmp_h - 1);
	sw = MAXDIFF(MIN(bmp_w - from_x, rect.Width()), 0);
	sh = MAXDIFF(MIN(bmp_h - from_y, rect.Height()), 0);
	SetDIBitsToDevice(pDCc->m_hDC, rect.left, rect.top, sw, sh, from_x, from_y, 0, sh, img->imageData + from_y*img->widthStep, bmi, 0);
	cvReleaseImage(&img);
	pWnd->ReleaseDC(pDCc);
}

void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin) //DC로 이미지를 그릴때 BITMAP 헤더를 생성하는 함수.
{
	assert(bmi&&width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));
	BITMAPINFOHEADER *bmih = &(bmi->bmiHeader);
	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;

	if (bpp == 8)
	{
		RGBQUAD *palette = bmi->bmiColors;
		int i;
		for (i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}
