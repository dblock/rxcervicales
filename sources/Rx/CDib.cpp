/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#include "stdafx.h"
#include "cdib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Rx.h"
extern CRxApp theApp;

#include <math.h>
#define APPLY_MODULO_255(bv, EE) (bv+EE>255)?255:((bv+EE<0)?0:bv+EE)
#define ROOTTWOPI (sqrt(2.0*M_PI))

IMPLEMENT_SERIAL(CDib, CObject, 0);

static float l_1_4 = (float) (1.0/4.0);
static float l_1_9 = (float) (1.0/9.0);
static float l_1_8 = (float) (1.0/8.0);
static float l_1_16 = (float) (1.0/16.0);
static float l_1_10 = (float) (1.0/10.0);

CDib::CDib(){	
	m_angle = 0;
	m_hFile = NULL;
	m_hBitmap = NULL;
	m_hPalette = NULL;
	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	
	m_SaveBitmap = NULL;
	m_SaveDC = NULL;
	m_SaveAngle = 0;
	m_SaveSize = CSize(0, 0);
	m_Invalid = TRUE;

	m_Histogram.SetDib(this);
	m_DibTracker = NULL;
	
	Empty();
}

CDib::CDib(CSize size, int nBitCount) {
	m_SaveBitmap = NULL;
	m_SaveDC = NULL;
	m_SaveAngle = 0;
	m_SaveSize = CSize(0, 0);	
	m_Invalid = TRUE;
	
	m_hFile = NULL;
	m_hBitmap = NULL;
	m_hPalette = NULL;
	m_nBmihAlloc = m_nImageAlloc = noAlloc;

	m_Histogram.SetDib(this);
	m_DibTracker = NULL;

	Empty();
	ComputePaletteSize(nBitCount);
	m_lpBMIH = (LPBITMAPINFOHEADER) new 
		char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	m_nBmihAlloc = crtAlloc;
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = size.cx;
	m_lpBMIH->biHeight = size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = nBitCount;
	m_lpBMIH->biCompression = BI_RGB;
	m_lpBMIH->biSizeImage = 0;
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	ComputeMetrics();
	memset(m_lpvColorTable, 0, sizeof(RGBQUAD) * m_nColorTableEntries);
	m_lpImage = NULL;  // no data yet
	m_angle = 0;
}

BOOL CDib::SetAngle(double Value){
	if (m_angle != Value){		
		m_angle = (double) Value;
		while (m_angle > (double) 2*M_PI) m_angle-=(double) (2*M_PI);
		while (m_angle < (double) 2*M_PI) m_angle+=(double) (2*M_PI);		
		return TRUE;
	} else return FALSE;
}

double CDib::GetAngle(void){
	return m_angle;
}

int CDib::GetDegAngle(void){
	return (int) ((m_angle * 180) / M_PI)%360;
}

BOOL CDib::SetDegAngle(int Value){
	return SetAngle((M_PI*Value)/180);
}

CDib::~CDib() {
	Empty();
}

CSize CDib::GetDimensions() const {	
	if(m_lpBMIH == NULL) return CSize(0, 0);
	return CSize((int) m_lpBMIH->biWidth, (int) m_lpBMIH->biHeight);
}

BOOL CDib::AttachMapFile(const char* strPathname, BOOL bShare) // for reading 
{
	// if we open the same file twice, Windows treats it as 2 separate files
	// doesn't work with rare BMP files where # palette entries > biClrUsed
	HANDLE hFile = ::CreateFile(strPathname, GENERIC_WRITE | GENERIC_READ,
		bShare ? FILE_SHARE_READ : 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ASSERT(hFile != INVALID_HANDLE_VALUE);
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	DWORD dwErr = ::GetLastError();
	if(hMap == NULL) {
		AfxMessageBox("Empty bitmap file");
		return FALSE;
	}
	LPVOID lpvFile = ::MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0); // map whole file
	ASSERT(lpvFile != NULL);
	if(((LPBITMAPFILEHEADER) lpvFile)->bfType != 0x4d42) {
		AfxMessageBox("Invalid bitmap file");
		DetachMapFile();
		return FALSE;
	}
	AttachMemory((LPBYTE) lpvFile + sizeof(BITMAPFILEHEADER));
	m_lpvFile = lpvFile;
	m_hFile = hFile;
	m_hMap = hMap;
	return TRUE;
}

CDib& CDib::Clone(const CDib& Other){
	if (m_SaveBitmap) {
		delete m_SaveBitmap;
		m_SaveBitmap = NULL;
	}
	if (m_SaveDC) {
		delete m_SaveDC;
		m_SaveDC = NULL;
	}	
	m_SaveAngle = 0;
	m_SaveSize = CSize(0, 0);
	m_Invalid = TRUE;
	m_DibTracker = Other.m_DibTracker;

	m_dwSizeImage = Other.m_dwSizeImage;
	m_lpImage = (LPBYTE) new char[m_dwSizeImage];
	memcpy(m_lpImage, Other.m_lpImage, m_dwSizeImage);
	m_nBmihAlloc = Other.m_nBmihAlloc;	
	m_nColorTableEntries = Other.m_nColorTableEntries;
	m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	memcpy(m_lpBMIH, Other.m_lpBMIH, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries);
	m_hBitmap = Other.m_hBitmap;
	m_hFile = Other.m_hFile;
	m_hGlobal = Other.m_hGlobal;
	m_hMap = Other.m_hMap;
	m_hPalette = Other.m_hPalette;
	m_lpvColorTable = Other.m_lpvColorTable;
	m_lpvFile = Other.m_lpvFile;
	m_nImageAlloc = Other.m_nImageAlloc;
	m_angle = Other.m_angle;	
	nWidth = Other.nWidth;
	nHeight = Other.nHeight;
	m_Histogram.BuildHistogram();
	return (* this);
}

CDib::CDib(const CDib& Other){
	m_Histogram.SetDib(this);
	Clone(Other);
}

CDib& CDib::operator=(const CDib& Other){
	return Clone(Other);
}

BOOL CDib::CopyToMapFile(const char* strPathname){
	// copies DIB to a new file, releases prior pointers
	// if you previously used CreateSection, the HBITMAP will be NULL (and unusable)
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	bmfh.bfSize = m_dwSizeImage + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries + sizeof(BITMAPFILEHEADER);
	// meaning of bfSize open to interpretation
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	HANDLE hFile = ::CreateFile(strPathname, GENERIC_WRITE | GENERIC_READ, 0, NULL,	CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ASSERT(hFile != INVALID_HANDLE_VALUE);
	
	
	HANDLE hDIBResult = GetRotatedBitmap(NULL, (double)(m_angle), RGB(255,255,255), CPoint(0,0), CSize(0,0), FALSE);
	int nHeaderSize = GetSizeHeader();
	LPBITMAPINFO lpbmInfoResult = (LPBITMAPINFO)hDIBResult ;
	LPVOID lpDIBBitsResult = (LPVOID)((LPBYTE)hDIBResult + nHeaderSize);
    	
	int nSize =  sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries +  lpbmInfoResult->bmiHeader.biSizeImage;
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, nSize, NULL);
	DWORD dwErr = ::GetLastError();
	ASSERT(hMap != NULL);
	LPVOID lpvFile = ::MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0); // map whole file
	ASSERT(lpvFile != NULL);
	LPBYTE lpbCurrent = (LPBYTE) lpvFile;
	memcpy(lpbCurrent, &bmfh, sizeof(BITMAPFILEHEADER)); // file header
	lpbCurrent += sizeof(BITMAPFILEHEADER);
	LPBITMAPINFOHEADER lpBMIH = (LPBITMAPINFOHEADER) lpbCurrent;
	memcpy(lpbCurrent, lpbmInfoResult, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries); // info
	lpbCurrent += sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	memcpy(lpbCurrent, lpDIBBitsResult, lpbmInfoResult->bmiHeader.biSizeImage); // bit image	
	/*
	DWORD dwSizeImage = lpbmInfoResult->bmiHeader.biSizeImage;
	Empty();
	m_dwSizeImage = dwSizeImage;
	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	m_lpBMIH = lpBMIH;
	m_lpImage = lpbCurrent;
	m_hFile = hFile;
	m_hMap = hMap;
	m_lpvFile = lpvFile;
	ComputePaletteSize(m_lpBMIH->biBitCount);
	ComputeMetrics();
	MakePalette();
	*/
	::CloseHandle(hFile);
	GlobalFree(hDIBResult);
	return TRUE;
}

BOOL CDib::AttachMemory(LPVOID lpvMem, BOOL bMustDelete, HGLOBAL hGlobal){
	// assumes contiguous BITMAPINFOHEADER, color table, image
	// color table could be zero length
	Empty();
	m_hGlobal = hGlobal;
	if(bMustDelete == FALSE) {
		m_nBmihAlloc = noAlloc;
	}
	else {
		m_nBmihAlloc = ((hGlobal == NULL) ? crtAlloc : heapAlloc);
	}
	try {
		m_lpBMIH = (LPBITMAPINFOHEADER) lpvMem;
		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		m_lpImage = (LPBYTE) m_lpvColorTable + sizeof(RGBQUAD) * m_nColorTableEntries;
		MakePalette();		
	}
	catch(CException* pe) {
		AfxMessageBox("AttachMemory error");
		pe->Delete();
		return FALSE;
	}	
	return TRUE;
}

UINT CDib::UsePalette(CDC* pDC, BOOL bBackground /* = FALSE */)
{
	if(m_hPalette == NULL) return 0;
	HDC hdc = pDC->GetSafeHdc();
	::SelectPalette(hdc, m_hPalette, bBackground);
	return ::RealizePalette(hdc);
}

BOOL CDib::HasChanged(CSize size) {	
	if ((m_Invalid) || ((size != m_SaveSize) || (m_SaveAngle != m_angle))) {	
		if (!m_Invalid) {
			struct _timeb ch_post;
			_ftime( &ch_post );   
			if ((ch_post.time == timebuffer.time) && 
				(ch_post.millitm - timebuffer.millitm < 75)) {		// 50 ms repaint interval
				return FALSE;
			}
			timebuffer = ch_post;
		} 
		m_SaveSize = size;
		m_SaveAngle = m_angle;		
		return TRUE;
	} else return FALSE;	
}

void CDib::PrePaintPicture(BOOL Force, CSize size, CDC * pDC){	
	if (Force || HasChanged(size)) {				
		if (m_SaveDC) delete m_SaveDC;
		m_SaveDC = new CDC;		
		m_SaveDC->CreateCompatibleDC(pDC);			
		if (m_SaveBitmap) delete m_SaveBitmap;
		m_SaveBitmap = new CBitmap();		
		MakeRotateConstants(CPoint(0, 0), size);		
		if (m_angle != 0) {
			m_SaveBitmap ->CreateCompatibleBitmap(pDC, nw, nh);
			m_SaveDC->SelectObject(m_SaveBitmap);					
			m_SaveDC->PatBlt(0,0,nw,nh,WHITENESS);		
		} else {
			m_SaveBitmap ->CreateCompatibleBitmap(pDC, size.cx, size.cy);
			m_SaveDC->SelectObject(m_SaveBitmap);					
			m_SaveDC->PatBlt(0,0,size.cx,size.cy,WHITENESS);		
		}
		::SelectPalette(m_SaveDC->GetSafeHdc(), m_hPalette, TRUE);		
		if (m_Invalid) {			
			BuildHistogram();
			m_Invalid = FALSE;								
		}
		if (m_angle != 0) {			
			GlobalFree(GetRotatedBitmap(m_SaveDC, (double)(m_angle), RGB(255,255,255), CPoint(0, 0), size));
		} else {
			m_SaveDC->SetStretchBltMode(COLORONCOLOR);	
			::StretchDIBits(m_SaveDC->GetSafeHdc(), 0, 0, size.cx, size.cy,
				0, 0, m_lpBMIH->biWidth, m_lpBMIH->biHeight,		
				m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCAND);			
		}
	}
}

BOOL CDib::Draw(CDC* pDC, CPoint origin, CSize size){
	if(m_lpBMIH == NULL) return FALSE;
	if(m_hPalette != NULL) ::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	PrePaintPicture(FALSE, size, pDC);
	/*
	if (m_angle == 0){
		pDC->SetStretchBltMode(COLORONCOLOR);	
		::StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
			0, 0, m_lpBMIH->biWidth, m_lpBMIH->biHeight,		
			m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCAND);	
	} else {				
		GlobalFree(GetRotatedBitmap(pDC, (double)(m_angle), RGB(255,255,255), origin, size));	
	}
	*/
	if (m_angle == 0){
		BitBlt(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy, m_SaveDC->GetSafeHdc(), 0, 0, SRCAND);
	} else {		
		//GlobalFree(GetRotatedBitmap(pDC, (double)(m_angle), RGB(255,255,255), origin, size));
		MakeRotateConstants(origin, size);
		BitBlt(pDC->GetSafeHdc(), l, t, nw, nh, m_SaveDC->GetSafeHdc(), 0, 0, SRCAND);				
		/*
			pDC->SelectStockObject(GRAY_BRUSH);
			pDC->FrameRect(CRect(l,t,l+nw,t+nh), pDC->GetCurrentBrush());
			*/
	}	
	/*
		pDC->SelectStockObject(GRAY_BRUSH);
		CRgn pRgn;GetRotateRgn(origin, size, pRgn);
		pDC->FillRgn(&pRgn, pDC->GetCurrentBrush());	
		*/	
	return TRUE;
}

void CDib::MakeRotateConstants(CPoint origin, CSize size, BOOL Invert){
	nWidth = m_lpBMIH->biWidth;	
	nHeight = m_lpBMIH->biHeight;	
	if (Invert){
		cosine = (double)cos(-m_angle);
		sine = (double)sin(-m_angle);
	} else {
		cosine = (double)cos(m_angle);
		sine = (double)sin(m_angle);	
	}

	x1 = (int)(-nHeight * sine);	
	y1 = (int)(nHeight * cosine);
	x2 = (int)(nWidth * cosine - nHeight * sine);
	y2 = (int)(nHeight * cosine + nWidth * sine);
	x3 = (int)(nWidth * cosine);	
	y3 = (int)(nWidth * sine);

	minx = min(0,min(x1, min(x2,x3)));	
	miny = min(0,min(y1, min(y2,y3)));
	maxx = max(0,max(x1, max(x2,x3)));	
	maxy = max(0,max(y1, max(y2,y3)));	

	w = maxx - minx;	
	h = maxy - miny; 	

	nw = (int)((double)((double)w * size.cx)) / m_lpBMIH->biWidth;
	nh = (int)((double)((double)h * size.cy)) / m_lpBMIH->biHeight;
	
	l = origin.x + (size.cx-nw)/2;
	t = origin.y + (size.cy-nh)/2; 		
	
}

CRect CDib::GetRotateBounds(CPoint origin, CSize size){		
	return CRect(CPoint(l, t), CSize(nw,nh));	
	/*
	if (m_angle != 0) {		
		return CRect(CPoint(l, t), CSize(nw,nh));
	} else return CRect(CPoint(origin.x, origin.y), size);	
	*/
}

CRgn& CDib::GetRotateRgn(CPoint origin, CSize size, CRgn& Result){	
	POINT Bounds[4]; 	
	MakeRotateConstants(origin, size, TRUE);
	Bounds[0] = GetTopLeft(origin, size);
	Bounds[1] = GetBottomLeft(origin, size);
	Bounds[2] = GetBottomRight(origin, size); 
	Bounds[3] = GetTopRight(origin, size); 
	Result.CreatePolygonRgn(Bounds, 4, ALTERNATE);		
	return Result;
}

CPoint CDib::CalcRealPoint(int sourcex, int sourcey){	
	double s_pow = pow(cosine,2)+pow(sine,2);
	return CPoint((int)((-(pow(cosine,2)*minx)-minx*pow(sine,2)+cosine*sourcex-sine*sourcey)/s_pow),
                  (int)((-(pow(cosine,2)*miny)-miny*pow(sine,2)+sine*sourcex+cosine*sourcey)/s_pow));
}

CPoint CDib::CalcStretch(CPoint APoint){
	double xFactor = ((double)nw)/w;
	double yFactor = ((double)nh)/h;
	return CPoint((int) (xFactor*APoint.x), (int) (yFactor*APoint.y));
}

CPoint CDib::GetTopLeft(CPoint origin, CSize size){
	if (m_angle != 0){
		return CPoint(l,t)+CalcStretch(CalcRealPoint(0, 0));
	} else return origin;
}

CPoint CDib::GetTopRight(CPoint origin, CSize size){
	if (m_angle != 0){					
		return CPoint(l,t)+CalcStretch(CalcRealPoint(nWidth, 0));
	} else return CPoint(origin.x+size.cx, origin.y);
}

CPoint CDib::GetBottomLeft(CPoint origin, CSize size){
	if (m_angle != 0){						
		return CPoint(l,t)+CalcStretch(CalcRealPoint(0, nHeight));
	} else return CPoint(origin.x, origin.y+size.cy);
}

CPoint CDib::GetBottomRight(CPoint origin, CSize size){
	if (m_angle != 0){			
		return CPoint(l,t)+CalcStretch(CalcRealPoint(nWidth, nHeight));
	} else return CPoint(origin.x+size.cx, origin.y+size.cy);
}

DWORD CDib::HandleBackgroundColor(LPVOID lpDIBBitsResult, long len, COLORREF clrBack, int bpp, int nColors, BITMAPINFO &bmInfo) const {
	DWORD dwBackColor = clrBack;
	switch(bpp)	{	
	case 1:	//Monochrome
		if( clrBack == RGB(255,255,255) )			
			memset( lpDIBBitsResult, 0xff, len );
		break;
	case 4:	
	case 8:	//Search the color table		
		int i;
		for(i = 0; i < nColors; i++ ) {
			if( bmInfo.bmiColors[i].rgbBlue ==  GetBValue(clrBack)
				&& bmInfo.bmiColors[i].rgbGreen ==  GetGValue(clrBack)
				&& bmInfo.bmiColors[i].rgbRed ==  GetRValue(clrBack)) {
				if(bpp==4) i = i | i<<4;
				memset( lpDIBBitsResult, i, len );				
				break;			
			}
		}	
		// If not match found the color remains black		
		break;	
	case 16:
		// Windows95 supports 5 bits each for all colors or 5 bits for red & blue
		// and 6 bits for green - Check the color mask for RGB555 or RGB565
		if( *((DWORD*)bmInfo.bmiColors) == 0x7c00 )		{			// Bitmap is RGB555
			dwBackColor = ((GetRValue(clrBack)>>3) << 10) + 
					((GetRValue(clrBack)>>3) << 5) +					
					(GetBValue(clrBack)>>3) ;		
		} else	{
			// Bitmap is RGB565			
			dwBackColor = ((GetRValue(clrBack)>>3) << 11) + 
					((GetRValue(clrBack)>>2) << 5) +					
					(GetBValue(clrBack)>>3) ;		
		}		
		break;
	case 24:	
	case 32:		
		dwBackColor = (((DWORD)GetRValue(clrBack)) << 16) | 
				(((DWORD)GetGValue(clrBack)) << 8) |
				(((DWORD)GetBValue(clrBack)));
		break;	
	}	
	return dwBackColor;
}

int CDib::RowBytes(void) const {
	return ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
}

void CDib::PutPixel(int x, int y, DWORD i_Color, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) {
	if (!im_lpBMIH) im_lpBMIH = m_lpBMIH;
	if (!im_lpImage) im_lpImage = m_lpImage;
	if (nRowBytes <= 0) nRowBytes = ((((im_lpBMIH->biWidth * im_lpBMIH->biBitCount) + 31) & ~31) / 8);
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)im_lpBMIH;	
	BYTE l_Pixel;
	int nColors;
	switch(im_lpBMIH->biBitCount){
		int i;
	case 1:
		l_Pixel = (*((LPBYTE)im_lpImage + nRowBytes*(y) + (x/8))) & (~(0x80 >> x%8));
		l_Pixel = (i_Color>0)?l_Pixel|(0x80>>x%8):l_Pixel;
		*((LPBYTE)im_lpImage + nRowBytes*(y) + (x/8)) = l_Pixel;
		break;
	case 4:
		nColors = im_lpBMIH->biClrUsed ? im_lpBMIH->biClrUsed: 1 << im_lpBMIH->biBitCount;	
		for(i = 0;i<nColors;i++) {
			if( bmInfo.bmiColors[i].rgbBlue ==  GetBValue(i_Color)
				&& bmInfo.bmiColors[i].rgbGreen ==  GetGValue(i_Color)
				&& bmInfo.bmiColors[i].rgbRed ==  GetRValue(i_Color)) {
					l_Pixel = *((LPBYTE)im_lpImage + nRowBytes*(y) + (x/2));		
					l_Pixel = (x&1)?(l_Pixel&0xf)+(i<<4):l_Pixel&0xf0+i;		
					*((LPBYTE)im_lpImage + nRowBytes*(y) + (x/2)) = l_Pixel;
					break;
			}
		}
		break;
	case 8:
		nColors = im_lpBMIH->biClrUsed ? im_lpBMIH->biClrUsed: 1 << im_lpBMIH->biBitCount;	
		for(i = 0;i<nColors;i++) {
			if( bmInfo.bmiColors[i].rgbBlue ==  GetBValue(i_Color)
				&& bmInfo.bmiColors[i].rgbGreen ==  GetGValue(i_Color)
				&& bmInfo.bmiColors[i].rgbRed ==  GetRValue(i_Color)) {
				*((LPBYTE)im_lpImage + nRowBytes*(y) + (x)) = (BYTE) i;				
				break;
			}
		}
		break;
	case 16:
		*((LPWORD)((LPBYTE)im_lpImage + nRowBytes*y + x*2)) = 
			(WORD) GetBValue(i_Color) + (5 << GetGValue(i_Color)) + (10 << GetRValue(i_Color));
		break;
	case 24:
		*((LPDWORD)((LPBYTE)im_lpImage + nRowBytes*y + x*3)) = 
			*((LPDWORD)((LPBYTE)im_lpImage + nRowBytes*y + x*3)) & 0xff000000
			| (GetRValue(i_Color) << 0x10) +
			  (GetGValue(i_Color) << 0x08) +
			  GetBValue(i_Color);
		break;
	case 32:
		*((LPDWORD)((LPBYTE)im_lpImage + nRowBytes*y + x*4)) = i_Color;
		break;
	}	
}

BYTE CDib::GetNeighborPixelByte(int x, int y, int j, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	return (GetNeighborPixel(x, y, j, im_lpBMIH, im_lpImage, nRowBytes))?1:0;
}
	

DWORD CDib::GetNeighborPixel(int x, int y, int j, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	if (j==0) return GetPixel(x+1, y, im_lpBMIH, im_lpImage, nRowBytes);	
	else if (j==1) return GetPixel(x+1, y-1, im_lpBMIH, im_lpImage, nRowBytes);
	else if (j==2) return GetPixel(x, y-1, im_lpBMIH, im_lpImage, nRowBytes);
	else if (j==3) return GetPixel(x-1, y-1, im_lpBMIH, im_lpImage, nRowBytes);
	else if (j==4) return GetPixel(x-1, y, im_lpBMIH, im_lpImage, nRowBytes);
	else if (j==5) return GetPixel(x-1, y+1, im_lpBMIH, im_lpImage, nRowBytes);
	else if (j==6) return GetPixel(x, y+1, im_lpBMIH, im_lpImage, nRowBytes);
	else if (j==7) return GetPixel(x+1, y+1, im_lpBMIH, im_lpImage, nRowBytes);
	else return 0;
}

DWORD CDib::GetPixel(int x, int y, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	if (!im_lpBMIH) im_lpBMIH = m_lpBMIH;
	if (!im_lpImage) im_lpImage = m_lpImage;
	if (nRowBytes <= 0) nRowBytes = ((((im_lpBMIH->biWidth * im_lpBMIH->biBitCount) + 31) & ~31) / 8);
	
	BYTE l_Pixel;
	WORD l_WPixel;
	DWORD l_DWPixel;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)im_lpBMIH;	

	switch(im_lpBMIH->biBitCount){					
	case 1:
		/* 
			Each bit in the bitmap array represents a pixel
			*/		
		l_Pixel = *((LPBYTE)im_lpImage + nRowBytes*y + x/8) & (0x80 >> x%8);
		l_Pixel = l_Pixel >> (7 - x%8);
		return (DWORD) l_Pixel;
	case 4:		
		/* 
			The bitmap has a maximum of 16 colors, and the bmiColors member 
			contains up to 16 entries. Each pixel in the bitmap is represented 
			by a 4-bit index into the color table.
			*/
		l_Pixel = *((LPBYTE)im_lpImage + nRowBytes*(y) + (x/2));		
		l_Pixel = (x&1)?l_Pixel>>4:l_Pixel&0x0f;		
		return RGB(bmInfo.bmiColors[l_Pixel].rgbRed, 
				   bmInfo.bmiColors[l_Pixel].rgbGreen, 
				   bmInfo.bmiColors[l_Pixel].rgbBlue);		
	case 8:
		/*
			The bitmap has a maximum of 256 colors, and the bmiColors member 
			contains up to 256 entries. In this case, each byte in the array 
			represents a single pixel.
			*/		
		l_Pixel = *((LPBYTE)im_lpImage + nRowBytes*y + x);
		return RGB(
			bmInfo.bmiColors[l_Pixel].rgbRed, 
			bmInfo.bmiColors[l_Pixel].rgbGreen, 
			bmInfo.bmiColors[l_Pixel].rgbBlue);
	case 16:
		/*	
			The bitmap has a maximum of 2^16 colors. 
			Each WORD in the bitmap array represents a single pixel. 
			The relative intensities of red, green, and blue are represented 
			with 5 bits for each color component. The value for blue is in the 
			least significant 5 bits, followed by 5 bits each for green and red. 
			The most significant bit is not used. 
			*/		
		l_WPixel =  (WORD) *((LPWORD)((LPBYTE)im_lpImage + nRowBytes*y + x*2));
		return RGB((l_WPixel & 0x7C00) >> 0x0A, 
				   (l_WPixel & 0x03E0) >> 0x05,
				   (l_WPixel & 0x1F));
	case 24:
		/*
			The bitmap has a maximum of 2^24 colors.
			Each 3-byte triplet in the bitmap array represents the 
			relative intensities of blue, green, and red, respectively, for a pixel. 
			*/
		l_DWPixel = *((LPDWORD)((LPBYTE)im_lpImage + nRowBytes*y + x*3));
		return RGB((l_DWPixel & 0xFF0000) >> 0x10,
				   (l_DWPixel & 0xFF00) >> 0x08, 
				   (l_DWPixel & 0xFF));		
		return 0;
	case 32:
		/*
			The bitmap has a maximum of 2^32 colors. Each DWORD in the 
			bitmap array represents the relative intensities of blue, green, and red, 
			respectively, for a pixel. The high byte in each DWORD is not used. 
			*/
		l_DWPixel = *((LPDWORD)((LPBYTE)im_lpImage + nRowBytes*y + x*4));
		return RGB((l_DWPixel & 0xFF0000) >> 0x10,
				   (l_DWPixel & 0xFF00) >> 0x08, 
				   (l_DWPixel & 0xFF));		
		/*
		Tmp.Format("%d : %d %d %d", 
					l_DWPixel,					
					(l_DWPixel & 0xFF0000) >> 0x10,
					(l_DWPixel & 0xFF00) >> 0x08, 
					(l_DWPixel & 0xFF));		
		MessageBox(0, Tmp, "", MB_OK);		
		*/
	default:
		return (DWORD) 0;
	}

}


unsigned int CDib::GetRed(int x, int y, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	return GetRValue(GetPixel(x, y, im_lpBMIH, im_lpImage, nRowBytes));
}

unsigned int CDib::GetGreen(int x, int y, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	return GetGValue(GetPixel(x, y, im_lpBMIH, im_lpImage, nRowBytes));
}

unsigned int CDib::GetBlue(int x, int y, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	return GetBValue(GetPixel(x, y, im_lpBMIH, im_lpImage, nRowBytes));
}

unsigned int CDib::GetGrayLog(int x, int y, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	int l_Pix = GetPixel(x, y, im_lpBMIH, im_lpImage, nRowBytes);
	return (unsigned int) (0.3*GetRValue(l_Pix) + 0.59*GetGValue(l_Pix) + 0.11*GetBValue(l_Pix));
}

unsigned int CDib::GetGray(int x, int y, LPBITMAPINFOHEADER im_lpBMIH, LPBYTE im_lpImage, int nRowBytes) const {
	int l_Pix = GetPixel(x, y, im_lpBMIH, im_lpImage, nRowBytes);
	return (GetRValue(l_Pix) + GetGValue(l_Pix) + GetBValue(l_Pix))/3;
}


HANDLE CDib::GetRotatedBitmap(CDC* pDC, double radians, COLORREF clrBack, CPoint origin, CSize size, BOOL DoShow){
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)m_lpBMIH;	
	int bpp = m_lpBMIH->biBitCount;	
	int nColors = m_lpBMIH->biClrUsed ? m_lpBMIH->biClrUsed: 1 << bpp;	
	MakeRotateConstants(origin, size);
	int nRowBytes = ((((nWidth * bpp) + 31) & ~31) / 8);	
	DWORD &compression = bmInfo.bmiHeader.biCompression;
	if( nHeight < 0 || (compression!=BI_RGB && compression!=BI_BITFIELDS)) return NULL;	
	LPVOID lpDIBBits = m_lpImage;
	int nResultRowBytes = ((((w * bpp) + 31) & ~31) / 8);
	long len = nResultRowBytes * h;	
	int nHeaderSize = GetSizeHeader();		
	HANDLE hDIBResult = GlobalAlloc(GMEM_FIXED,len+nHeaderSize);
	// Initialize the header information
	memcpy((void*)hDIBResult, this->m_lpBMIH, nHeaderSize);
	BITMAPINFO &bmInfoResult = *(LPBITMAPINFO)hDIBResult ;
	bmInfoResult.bmiHeader.biWidth = w;	
	bmInfoResult.bmiHeader.biHeight = h;
	bmInfoResult.bmiHeader.biSizeImage = len;
	LPVOID lpDIBBitsResult = (LPVOID)((LPBYTE)hDIBResult + nHeaderSize);	
	ZeroMemory( lpDIBBitsResult, len );
	DWORD dwBackColor = HandleBackgroundColor(lpDIBBitsResult, len, clrBack, bpp, nColors, bmInfo);	
	for( int y = 0; y < h; y++ )	{
		for( int x = 0; x < w; x++ ) {			
			int sourcex = (int)((x+minx)*cosine + (y+miny)*sine);
			int sourcey = (int)((y+miny)*cosine - (x+minx)*sine);		
			if( sourcex >= 0 && sourcex < nWidth && sourcey >= 0 && sourcey < nHeight ){		
				// Set the destination pixel				
				switch(bpp){					
					BYTE mask;				
				case 1:		//Monochrome
					mask = *((LPBYTE)lpDIBBits + nRowBytes*sourcey + sourcex/8) & (0x80 >> sourcex%8);
					//Adjust mask for destination bitmap
					mask = mask ? (0x80 >> x%8) : 0;
					*((LPBYTE)lpDIBBitsResult + nResultRowBytes*(y) + (x/8)) &= ~(0x80 >> x%8);
					*((LPBYTE)lpDIBBitsResult + nResultRowBytes*(y) + (x/8)) |= mask;
					break;				
				case 4:					
					mask = *((LPBYTE)lpDIBBits + nRowBytes*sourcey + sourcex/2) & ((sourcex&1) ? 0x0f : 0xf0);
					//Adjust mask for destination bitmap					
					if( (sourcex&1) != (x&1) )
						mask = (mask&0xf0) ? (mask>>4) : (mask<<4);
						*((LPBYTE)lpDIBBitsResult + nResultRowBytes*(y) + (x/2)) &= ~((x&1) ? 0x0f : 0xf0);
					*((LPBYTE)lpDIBBitsResult + nResultRowBytes*(y) + (x/2)) |= mask;
					break;				
				case 8:				
					BYTE pixel ;
					pixel = *((LPBYTE)lpDIBBits + nRowBytes*sourcey + sourcex);
					*((LPBYTE)lpDIBBitsResult + nResultRowBytes*(y) + (x)) = pixel;
					break;
				case 16:				
					DWORD dwPixel;
					dwPixel = *((LPWORD)((LPBYTE)lpDIBBits + nRowBytes*sourcey + sourcex*2));
					*((LPWORD)((LPBYTE)lpDIBBitsResult + nResultRowBytes*y + x*2)) = (WORD)dwPixel;
					break;				
				case 24:
					dwPixel = *((LPDWORD)((LPBYTE)lpDIBBits + nRowBytes*sourcey + sourcex*3)) & 0xffffff;
					*((LPDWORD)((LPBYTE)lpDIBBitsResult + nResultRowBytes*y + x*3)) |= dwPixel;
					break;				
				case 32:
					dwPixel = *((LPDWORD)((LPBYTE)lpDIBBits + nRowBytes*sourcey + sourcex*4));
					*((LPDWORD)((LPBYTE)lpDIBBitsResult + nResultRowBytes*y + x*4)) = dwPixel;				
				}			
			} else {								
				// Draw the background color. The background color
				// has already been drawn for 8 bits per pixel and less								
				switch(bpp)	{
				case 16:	
					*((LPWORD)((LPBYTE)lpDIBBitsResult + nResultRowBytes*y + x*2)) = (WORD)dwBackColor;
					break;
				case 24:
					*((LPDWORD)((LPBYTE)lpDIBBitsResult + nResultRowBytes*y + x*3)) |= dwBackColor;
					break;				
				case 32:
					*((LPDWORD)((LPBYTE)lpDIBBitsResult + nResultRowBytes*y + x*4)) = dwBackColor;
					break;
				}				
			}		
		}	
	}
	
	if (DoShow){
		::StretchDIBits(pDC->GetSafeHdc(), 
			0, //l, //origin.x + (int)(-w + m_lpBMIH->biWidth)/2,
			0, //t, //origin.y + (int)(-h + m_lpBMIH->biHeight)/2, 
			nw, //bmInfoResult.bmiHeader.biWidth+(size.cx-m_lpBMIH->biWidth), 
			nh, //bmInfoResult.bmiHeader.biHeight+(size.cy-m_lpBMIH->biHeight),		
			0, 0, 
			bmInfoResult.bmiHeader.biWidth, 
			bmInfoResult.bmiHeader.biHeight,
			lpDIBBitsResult, &bmInfoResult, DIB_RGB_COLORS, SRCAND);
	}	

	return hDIBResult;
}


HBITMAP CDib::CreateSection(CDC* pDC /* = NULL */){
	if(m_lpBMIH == NULL) return NULL;
	if(m_lpImage != NULL) return NULL; // can only do this if image doesn't exist	
	m_hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO) m_lpBMIH,
		DIB_RGB_COLORS,	(LPVOID*) &m_lpImage, NULL, 0);			
	ASSERT(m_lpImage != NULL);
	return m_hBitmap;
}

BOOL CDib::MakePalette(){
	// makes a logical palette (m_hPalette) from the DIB's color table
	// this palette will be selected and realized prior to drawing the DIB
	if(m_nColorTableEntries == 0) return FALSE;
	if(m_hPalette != NULL) ::DeleteObject(m_hPalette);
	//TRACE("CDib::MakePalette -- m_nColorTableEntries = %d\n", m_nColorTableEntries);
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) +
		m_nColorTableEntries * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = m_nColorTableEntries;
	LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
	for(int i = 0; i < m_nColorTableEntries; i++) {
		pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
		pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
		pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
		pLogPal->palPalEntry[i].peFlags = 0;
		pDibQuad++;
	}
	m_hPalette = ::CreatePalette(pLogPal);
	delete pLogPal;
	return TRUE;
}	

BOOL CDib::SetSystemPalette(CDC* pDC)
{
	// if the DIB doesn't have a color table, we can use the system's halftone palette
	if(m_nColorTableEntries != 0) return FALSE;
	m_hPalette = ::CreateHalftonePalette(pDC->GetSafeHdc());
	return TRUE;
}

HBITMAP CDib::CreateBitmap(CDC* pDC)
{
    if (m_dwSizeImage == 0) return NULL;
    HBITMAP hBitmap = ::CreateDIBitmap(pDC->GetSafeHdc(), m_lpBMIH,
            CBM_INIT, m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS);
    ASSERT(hBitmap != NULL);
    return hBitmap;
}

BOOL CDib::Compress(CDC* pDC, BOOL bCompress /* = TRUE */)
{
	// 1. makes GDI bitmap from existing DIB
	// 2. makes a new DIB from GDI bitmap with compression
	// 3. cleans up the original DIB
	// 4. puts the new DIB in the object
	if((m_lpBMIH->biBitCount != 4) && (m_lpBMIH->biBitCount != 8)) return FALSE;
		// compression supported only for 4 bpp and 8 bpp DIBs
	if(m_hBitmap) return FALSE; // can't compress a DIB Section!
	TRACE("Compress: original palette size = %d\n", m_nColorTableEntries); 
	HDC hdc = pDC->GetSafeHdc();
	HPALETTE hOldPalette = ::SelectPalette(hdc, m_hPalette, FALSE);
	HBITMAP hBitmap;  // temporary
	if((hBitmap = CreateBitmap(pDC)) == NULL) return FALSE;
	int nSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	LPBITMAPINFOHEADER lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
	memcpy(lpBMIH, m_lpBMIH, nSize);  // new header
	if(bCompress) {
		switch (lpBMIH->biBitCount) {
		case 4:
			lpBMIH->biCompression = BI_RLE4;
			break;
		case 8:
			lpBMIH->biCompression = BI_RLE8;
			break;
		default:
			ASSERT(FALSE);
		}
		// calls GetDIBits with null data pointer to get size of compressed DIB
		if(!::GetDIBits(pDC->GetSafeHdc(), hBitmap, 0, (UINT) lpBMIH->biHeight,
						NULL, (LPBITMAPINFO) lpBMIH, DIB_RGB_COLORS)) {
			AfxMessageBox("Unable to compress this DIB");
			// probably a problem with the color table
	 		::DeleteObject(hBitmap);
			delete [] lpBMIH;
			::SelectPalette(hdc, hOldPalette, FALSE);
			return FALSE; 
		}
		if (lpBMIH->biSizeImage == 0) {
			AfxMessageBox("Driver can't do compression");
	 		::DeleteObject(hBitmap);
			delete [] lpBMIH;
			::SelectPalette(hdc, hOldPalette, FALSE);
			return FALSE; 
		}
		else {
			m_dwSizeImage = lpBMIH->biSizeImage;
		}
	}
	else {
		lpBMIH->biCompression = BI_RGB; // decompress
		// figure the image size from the bitmap width and height
		DWORD dwBytes = ((DWORD) lpBMIH->biWidth * lpBMIH->biBitCount) / 32;
		if(((DWORD) lpBMIH->biWidth * lpBMIH->biBitCount) % 32) {
			dwBytes++;
		}
		dwBytes *= 4;
		m_dwSizeImage = dwBytes * lpBMIH->biHeight; // no compression
		lpBMIH->biSizeImage = m_dwSizeImage;
	} 
	// second GetDIBits call to make DIB
	LPBYTE lpImage = (LPBYTE) new char[m_dwSizeImage];
	VERIFY(::GetDIBits(pDC->GetSafeHdc(), hBitmap, 0, (UINT) lpBMIH->biHeight,
    		lpImage, (LPBITMAPINFO) lpBMIH, DIB_RGB_COLORS));
    TRACE("dib successfully created - height = %d\n", lpBMIH->biHeight);
	::DeleteObject(hBitmap);
	Empty();
	m_nBmihAlloc = m_nImageAlloc = crtAlloc;
	m_lpBMIH = lpBMIH;
	m_lpImage = lpImage;
	ComputeMetrics();
	ComputePaletteSize(m_lpBMIH->biBitCount);
	MakePalette();	
	::SelectPalette(hdc, hOldPalette, FALSE);
	TRACE("Compress: new palette size = %d\n", m_nColorTableEntries); 
	return TRUE;
}

BOOL CDib::Read(CFile* pFile)
{
	// 1. read file header to get size of info hdr + color table
	// 2. read info hdr (to get image size) and color table
	// 3. read image
	// can't use bfSize in file header
	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;
	try {
		nCount = pFile->Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		if(nCount != sizeof(BITMAPFILEHEADER)) {
			throw new CException;
		}
	/*
		CString Tmp, Tmp1;
		for (int i=0;i<nCount;i++){
			Tmp1.Format("%x ", ((char *) (LPVOID) &bmfh)[i]);
			Tmp+=Tmp1;
		}
		MessageBox(0, Tmp, "", MB_OK);
		*/
		
		if(bmfh.bfType != 0x4d42) {
			throw new CException;
		}		
		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		m_nBmihAlloc = m_nImageAlloc = crtAlloc;
		nCount = pFile->Read(m_lpBMIH, nSize); // info hdr & color table
		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		MakePalette();
		m_lpImage = (LPBYTE) new char[m_dwSizeImage];
		nCount = pFile->Read(m_lpImage, m_dwSizeImage); // image only		
	}
	catch(CException* pe) {
		AfxMessageBox("DIB read error");
		pe->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CDib::ReadSection(CFile* pFile, CDC* pDC /* = NULL */)
{
	// new function reads BMP from disk and creates a DIB section
	//    allows modification of bitmaps from disk
	// 1. read file header to get size of info hdr + color table
	// 2. read info hdr (to get image size) and color table
	// 3. create DIB section based on header parms
	// 4. read image into memory that CreateDibSection allocates
	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;
	try {
		nCount = pFile->Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		if(nCount != sizeof(BITMAPFILEHEADER)) {
			throw new CException;
		}
		if(bmfh.bfType != 0x4d42) {
			throw new CException;
		}
		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		m_nBmihAlloc = crtAlloc;
		m_nImageAlloc = noAlloc;
		nCount = pFile->Read(m_lpBMIH, nSize); // info hdr & color table
		if(m_lpBMIH->biCompression != BI_RGB) {
			throw new CException;
		}
		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		MakePalette();
		UsePalette(pDC);
		m_hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO) m_lpBMIH,
			DIB_RGB_COLORS,	(LPVOID*) &m_lpImage, NULL, 0);
		ASSERT(m_lpImage != NULL);
		nCount = pFile->Read(m_lpImage, m_dwSizeImage); // image only		
	}
	catch(CException* pe) {
		AfxMessageBox("ReadSection error");
		pe->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CDib::Write(CFile* pFile){	
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	bmfh.bfSize = 0;
//	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + nSizeHdr + m_dwSizeImage;
	// meaning of bfSize open to interpretation (bytes, words, dwords?) -- we won't use it
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * m_nColorTableEntries;	
	try {
		pFile->Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		pFile->Write((LPVOID) m_lpBMIH,  nSizeHdr);
		pFile->Write((LPVOID) m_lpImage, m_dwSizeImage);		
	}
	catch(CException* pe) {
		pe->Delete();
		AfxMessageBox("write error");
		return FALSE;
	}
	return TRUE;
}

void CDib::Serialize(CArchive& ar){
	try {
		ar.Flush();
		if(ar.IsStoring()) {
			Write(ar.GetFile());
		} else {
			Read(ar.GetFile());
		}
	} catch (...) {
		MessageBox(::GetFocus(), "Error reading DIB", "DIB Error", MB_OK);
	}
}

// helper functions
void CDib::ComputePaletteSize(int nBitCount)
{
	if((m_lpBMIH == NULL) || (m_lpBMIH->biClrUsed == 0)) {
		switch(nBitCount) {
			case 1:
				m_nColorTableEntries = 2;
				break;
			case 4:
				m_nColorTableEntries = 16;
				break;
			case 8:
				m_nColorTableEntries = 256;
				break;
			case 16:
			case 24:
			case 32:
				m_nColorTableEntries = 0;
				break;
			default:
				ASSERT(FALSE);
		}
	}
	else {
		m_nColorTableEntries = m_lpBMIH->biClrUsed;
	}
	ASSERT((m_nColorTableEntries >= 0) && (m_nColorTableEntries <= 256)); 
}

void CDib::ComputeMetrics(){
	if(m_lpBMIH->biSize != sizeof(BITMAPINFOHEADER)) {
		TRACE("Not a valid Windows bitmap -- probably an OS/2 bitmap\n");
		throw new CException;
	}
	m_dwSizeImage = m_lpBMIH->biSizeImage;
	if(m_dwSizeImage == 0) {
		DWORD dwBytes = ((DWORD) m_lpBMIH->biWidth * m_lpBMIH->biBitCount) / 32;
		if(((DWORD) m_lpBMIH->biWidth * m_lpBMIH->biBitCount) % 32) {
			dwBytes++;
		}
		dwBytes *= 4;
		m_dwSizeImage = dwBytes * m_lpBMIH->biHeight; // no compression
	}
	m_lpvColorTable = (LPBYTE) m_lpBMIH + sizeof(BITMAPINFOHEADER);
}

void CDib::Empty()
{
	// this is supposed to clean up whatever is in the DIB
	DetachMapFile();
	if(m_nBmihAlloc == crtAlloc) {
		delete [] m_lpBMIH;
	}
	else if(m_nBmihAlloc == heapAlloc) {
		::GlobalUnlock(m_hGlobal);
		::GlobalFree(m_hGlobal);
	}
	if(m_nImageAlloc == crtAlloc) delete [] m_lpImage;
	if(m_hPalette != NULL) ::DeleteObject(m_hPalette);
	if(m_hBitmap != NULL) ::DeleteObject(m_hBitmap);
	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	m_hGlobal = NULL;
	m_lpBMIH = NULL;
	m_lpImage = NULL;
	m_lpvColorTable = NULL;
	m_nColorTableEntries = 0;
	m_dwSizeImage = 0;
	m_lpvFile = NULL;
	m_hMap = NULL;
	m_hFile = NULL;
	m_hBitmap = NULL;
	m_hPalette = NULL;
	if (m_SaveBitmap) delete m_SaveBitmap;
	if (m_SaveDC) delete m_SaveDC;
	m_SaveBitmap = NULL;
	m_SaveDC = NULL;
	m_SaveSize = CSize(0, 0);
	m_SaveAngle = 0;		
	
	_ftime( &timebuffer );
	timebuffer.time--;
		
	m_Invalid = TRUE;
}

void CDib::DetachMapFile()
{
	if(m_hFile == NULL) return;
	::UnmapViewOfFile(m_lpvFile);
	::CloseHandle(m_hMap);
	::CloseHandle(m_hFile);
	m_hFile = NULL;
}

BOOL CDib::isEmpty(){
	return (0);
}

COleDataSource* CDib::SaveDib(){		        
	if (GetSizeImage() > 0) {			
		try{			
			EmptyClipboard();
			HANDLE hDIBResult = GetRotatedBitmap(NULL, (double)(m_angle), RGB(255,255,255), CPoint(0,0), CSize(0,0), FALSE);
			int nHeaderSize = GetSizeHeader();
			BITMAPINFO &bmInfoResult = *(LPBITMAPINFO)hDIBResult ;
			LPVOID lpDIBBitsResult = (LPVOID)((LPBYTE)hDIBResult + nHeaderSize);
			//COleDataSource* pSource = new COleDataSource();        
			int nImageSize = bmInfoResult.bmiHeader.biSizeImage;
			HGLOBAL hHeader = ::GlobalAlloc(GMEM_SHARE, nHeaderSize + nImageSize);
			LPVOID pHeader = ::GlobalLock(hHeader);
			ASSERT(pHeader != NULL);
			LPVOID pImage = (LPBYTE) pHeader + nHeaderSize;
			memcpy(pHeader, &bmInfoResult, nHeaderSize); 
			memcpy(pImage, lpDIBBitsResult, nImageSize);                
			::GlobalUnlock(hHeader);
			//pSource->CacheGlobalData(CF_DIB, hHeader);			
			SetClipboardData(CF_DIB, hHeader);
			//return pSource;		
			return NULL;
		} catch (...) { 
			theApp.ShowError();
			return NULL;
		}	
    } else return NULL;
	
/*
	EmptyClipboard();
	CString Temp("hello");
	HGLOBAL hMem = (char *) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, Temp.GetLength());
*/

}

void CDib::CopyDibTo(void){
	static char BASED_CODE szFilter[] = "Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "bmp", "*.bmp", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter);
	if (dlg.DoModal() == IDOK) {		
		CopyToMapFile(dlg.GetPathName());		
	}
}

int CDib::GetXPercent(void){
	return (int) (((double)nw/w)*100);
}

int CDib::GetYPercent(void){
	return (int) (((double)nh/h)*100);
}

double CDib::GetXScale(void){
	return (double)((double)nw)/w;	
}

double CDib::GetYScale(void){
	return (double)((double)nh)/h;
}

void CDib::Invalidate(void) {
	m_Invalid = TRUE;
}

void CDib::BuildHistogram(void) {	
	m_Histogram.BuildHistogram();
}

LPBITMAPINFOHEADER CDib::CreateGrayscalePalette(){
	LPBITMAPINFOHEADER new_lpBMIH;	
	m_nColorTableEntries = 256;
	new_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];	
	memcpy(new_lpBMIH, m_lpBMIH, sizeof(BITMAPINFOHEADER));
	new_lpBMIH->biBitCount = 8;
	new_lpBMIH->biClrUsed = m_nColorTableEntries;
	new_lpBMIH->biClrImportant = m_nColorTableEntries;
	m_lpvColorTable = (LPBYTE) new_lpBMIH + sizeof(BITMAPINFOHEADER);
	LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
	for(int i = 0; i < m_nColorTableEntries; i++) {
		pDibQuad->rgbRed = i;
		pDibQuad->rgbGreen = i;
		pDibQuad->rgbBlue = i;
		pDibQuad->rgbReserved = 0;
		pDibQuad++;
	}
	return new_lpBMIH;	
}

LPBITMAPINFOHEADER CDib::CreateDuotonePalette(){
	if (m_lpBMIH->biBitCount != 1) {
		LPBITMAPINFOHEADER new_lpBMIH;
		m_nColorTableEntries = 2;
		new_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
		memcpy(new_lpBMIH, m_lpBMIH, sizeof(BITMAPINFOHEADER));
		new_lpBMIH->biClrUsed = m_nColorTableEntries;
		new_lpBMIH->biClrImportant = m_nColorTableEntries;
		m_lpvColorTable = (LPBYTE) new_lpBMIH + sizeof(BITMAPINFOHEADER);
		LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
		new_lpBMIH->biBitCount = 1;
		
		pDibQuad->rgbRed = 0;
		pDibQuad->rgbGreen = 0;
		pDibQuad->rgbBlue = 0;
		pDibQuad->rgbReserved = 0;

		pDibQuad++;
		
		pDibQuad->rgbRed = 255;
		pDibQuad->rgbGreen = 255;
		pDibQuad->rgbBlue = 255;
		pDibQuad->rgbReserved = 0;	

		return new_lpBMIH;
	} else return NULL;
}


void CDib::CnvGrayLevel(BOOL i_Log) {
	if (m_lpBMIH->biBitCount < 4) return;
	
	LPBITMAPINFOHEADER new_lpBMIH = CreateGrayscalePalette();	
	
	int nRowBytes = ((((m_lpBMIH->biWidth * 8) + 31) & ~31) / 8);
	int tSize = nRowBytes*m_lpBMIH->biHeight;
	LPBYTE new_Image = new BYTE[tSize];
	new_lpBMIH->biSizeImage = tSize;
	
	BYTE l_Color;
	for (int x=0;x<m_lpBMIH->biWidth;x++)
		for (int y=0;y<m_lpBMIH->biHeight;y++) {
			if (i_Log) l_Color = GetGrayLog(x, y, m_lpBMIH, m_lpImage, nRowBytes); 
			else l_Color = GetGray(x, y, m_lpBMIH, m_lpImage, nRowBytes);						
			*((LPBYTE)new_Image + nRowBytes*(y) + (x)) = (BYTE) l_Color;
		}
		
	delete m_lpBMIH;
	m_lpBMIH = new_lpBMIH;
	MakePalette();	
	delete m_lpImage;
	m_lpImage = new_Image;
	m_Invalid = TRUE;
}

int CDib::GetDepth(void) const {
	return m_lpBMIH->biBitCount;
}

int CDib::GetSizeImage(void) const {
	return m_dwSizeImage;
}

int CDib::GetSizeHeader(void) const {
	return sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
}

void CDib::FloydSteinberg(void) {
	if (m_lpBMIH->biBitCount <= 1) return;
	
	int k, i, j, e, R, ee;
		
	int nRowBytes = ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
	int tSize = nRowBytes*m_lpBMIH->biHeight*8;
	
	unsigned char * new_lpImage = new unsigned char[tSize];
	for (i = 0; i < m_lpBMIH->biHeight; i++) {
		for (j = 0; j < m_lpBMIH->biWidth; j++) {      
			k = i*m_lpBMIH->biWidth+j;
			R = GetGray(j,i, m_lpBMIH, m_lpImage, nRowBytes);
			new_lpImage[k] = (unsigned char) R;
		}
    }

	m_dwSizeImage = tSize/8;
	delete m_lpImage;
	m_lpImage = (LPBYTE) new char[m_dwSizeImage];
	m_lpBMIH->biSizeImage  = m_dwSizeImage;

	LPBITMAPINFOHEADER new_lpBMIH = CreateDuotonePalette();	
	delete m_lpBMIH;
	m_lpBMIH = new_lpBMIH;
	
	for (i = 0; i < m_lpBMIH->biHeight; ++i)
		for (j = 0; j < m_lpBMIH->biWidth; ++j) {      
			k = i*m_lpBMIH->biWidth+j;     
			if (new_lpImage[k] > m_Histogram.GetAverageLevel()) {
				PutPixel(j,i,RGB(0xFF, 0xFF, 0xFF));
				e = new_lpImage[k]-255;
			} else {
				PutPixel(j,i, 0);
				e = new_lpImage[k];
			}                  
		ee = (3*e)/8;
		if (k+1 < tSize) new_lpImage[k+1] = APPLY_MODULO_255(new_lpImage[k+1], ee);
		if (k+m_lpBMIH->biWidth < tSize) new_lpImage[k+m_lpBMIH->biWidth] = APPLY_MODULO_255(new_lpImage[k+m_lpBMIH->biWidth], ee);
		ee = (e/4);
		if (k+m_lpBMIH->biWidth+1 < tSize) new_lpImage[k+m_lpBMIH->biWidth+1] = APPLY_MODULO_255(new_lpImage[k+m_lpBMIH->biWidth+1], ee);
	}
	delete(new_lpImage);	
	m_Invalid = TRUE;
}

void CDib::CnvMonochrome(void) {
	if (m_lpBMIH->biBitCount <= 1) return;

	int k, i, j;
	int nRowBytes = ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
	int tSize = (m_lpBMIH->biWidth*m_lpBMIH->biHeight);
	unsigned char * NewBuffer = new unsigned char[tSize];
	for (i = 0; i < m_lpBMIH->biHeight; i++) {
		for (j = 0; j < m_lpBMIH->biWidth; j++) {      
			k = i*m_lpBMIH->biWidth+j;			
			NewBuffer[k] = (unsigned char) (GetGray(j,i, m_lpBMIH, m_lpImage, nRowBytes) < m_Histogram.GetAverageLevel())?0:1;
		}
    }
	
	delete m_lpImage;
	
	m_dwSizeImage = m_lpBMIH->biHeight * ((((m_lpBMIH->biWidth) + 31) & ~31) / 8);
	m_lpImage = (LPBYTE) new char[m_dwSizeImage];
	m_lpBMIH->biSizeImage  = m_dwSizeImage;

	LPBITMAPINFOHEADER new_lpBMIH = CreateDuotonePalette();
	delete m_lpBMIH;
	m_lpBMIH = new_lpBMIH;

	for (i = 0; i < m_lpBMIH->biHeight; i++)
		for (j = 0; j < m_lpBMIH->biWidth; j++) {      
			k = i*m_lpBMIH->biWidth+j;
			PutPixel(j, i, (NewBuffer[k])?RGB(255,255,255):0);
		}

	delete NewBuffer;
	m_Invalid = TRUE;
}

void CDib::EqualizeHistogram(void) {
	if (m_lpBMIH->biBitCount < 4) return;
	return;	
}

void CDib::Invert(void) {	
	if (m_lpBMIH->biBitCount > 8) {
		DWORD l_Color;
		int nRowBytes = ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
		for (int x=0;x<m_lpBMIH->biWidth;x++)
			for (int y=0;y<m_lpBMIH->biHeight;y++) {
				l_Color = GetPixel(x, y, m_lpBMIH, m_lpImage, nRowBytes);
				if (m_lpBMIH->biBitCount == 1) 
					PutPixel(x, y, 1-l_Color, m_lpBMIH, m_lpImage, nRowBytes);
				else PutPixel(x, y, RGB(0xFF-GetRValue(l_Color), 0xFF-GetGValue(l_Color), 0xFF-GetBValue(l_Color)));
			}
		m_Invalid = TRUE;			
	} else {
		BITMAPINFO &bmInfo = *(LPBITMAPINFO)m_lpBMIH;	
		int nColors = m_lpBMIH->biClrUsed ? m_lpBMIH->biClrUsed: 1 << m_lpBMIH->biBitCount;	
		for(int i = 0; i < nColors; i++ ) {
			bmInfo.bmiColors[i].rgbBlue =  0xFF - bmInfo.bmiColors[i].rgbBlue;
			bmInfo.bmiColors[i].rgbGreen = 0xFF - bmInfo.bmiColors[i].rgbGreen;
			bmInfo.bmiColors[i].rgbRed =  0xFF - bmInfo.bmiColors[i].rgbRed;
		}
		m_Invalid = TRUE;			
	}
	
}

void CDib::Convolve(float * i_Kernel, unsigned int i_MWidth, unsigned int i_MHeight){	
	/* image must be at least the size of the kernel */
	if ((unsigned int) m_lpBMIH->biWidth < i_MWidth) return;
	if ((unsigned int) m_lpBMIH->biHeight < i_MHeight) return;
	/* show progress dialog */
	theApp.ShowProgress("Performing convolve, please wait.");		
	/* allocate the output image buffer */
	int l_newSize = m_lpBMIH->biHeight * ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
	LPBYTE new_Image = new BYTE[l_newSize];
	/* fill missing pixels (convolve cuts) */
	memcpy(new_Image, m_lpImage, l_newSize);
	/* */
	unsigned int l_ColOffset = i_MWidth / 2;
	unsigned int l_RowOffset = i_MHeight / 2;
	/* compensate edge defects */
	unsigned int l_Col = l_ColOffset;
	unsigned int l_Row = l_RowOffset;
	unsigned int l_Width = m_lpBMIH->biWidth - i_MWidth + 1;
	unsigned int l_Height = m_lpBMIH->biHeight - i_MHeight + 1;
	/* row range of pixels */
	unsigned int l_colExtent = l_Col + l_Width;
	unsigned int l_rowExtent = l_Row + l_Height;
	unsigned int l_TempRow, l_TempCol;

	double l_SumR, l_SumG, l_SumB;

	float * l_KernelPtr;
	unsigned int l_KernCol, l_KernRow;
	unsigned int l_ImageRow, l_ImageCol;
	DWORD l_Pixel;
	int nRowBytes = ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
	float l_KernelByte;
	for (l_ImageRow = l_Row; l_ImageRow < l_rowExtent; l_ImageRow++) {
		l_TempRow = l_ImageRow - l_RowOffset;
		theApp.SetProgress(l_ImageRow * 100 / l_rowExtent);
		for (l_ImageCol = l_Col; l_ImageCol < l_colExtent; l_ImageCol++) {
			l_TempCol = l_ImageCol - l_ColOffset;
			
			l_SumR = 0;
			l_SumG = 0;
			l_SumB = 0;
			l_KernelPtr = i_Kernel;
			for (l_KernCol = 0; l_KernCol < i_MWidth; l_KernCol++)
				for (l_KernRow = 0; l_KernRow < i_MHeight; l_KernRow++) {
					l_Pixel = GetPixel(l_TempCol + l_KernCol, l_TempRow + l_KernRow, m_lpBMIH, m_lpImage, nRowBytes);
					l_KernelByte = *l_KernelPtr++;
					
					l_SumR += GetRValue(l_Pixel)*l_KernelByte;
					l_SumG += GetGValue(l_Pixel)*l_KernelByte;
					l_SumB += GetBValue(l_Pixel)*l_KernelByte;			

					l_SumR = (l_SumR<0)?0:(l_SumR>0xFF)?0xFF:l_SumR;
					l_SumG = (l_SumG<0)?0:(l_SumG>0xFF)?0xFF:l_SumG;
					l_SumB = (l_SumB<0)?0:(l_SumB>0xFF)?0xFF:l_SumB;

				}
			PutPixel(l_ImageCol, l_ImageRow, 
				RGB(l_SumR, l_SumG, l_SumB),
				m_lpBMIH, new_Image, nRowBytes);
		}
	}
	delete m_lpImage;
	m_lpImage = new_Image;
	theApp.HideProgress();
	m_Invalid = TRUE;
}

void CDib::Blur(void) {
	static float m_Blur[] = {
		l_1_16, l_1_8, l_1_16,
		l_1_8, l_1_4, l_1_8,
		l_1_16, l_1_8, l_1_16
	};
	Cnv24Bit();
	Convolve(m_Blur, 3, 3);
}

void CDib::BlurMore(void) {
	static float m_BlurMore[] = {
		l_1_9, l_1_9, l_1_9,
		l_1_9, l_1_9, l_1_9,
		l_1_9, l_1_9, l_1_9
	};
	Cnv24Bit();
	Convolve(m_BlurMore, 3, 3);
}

void CDib::Cnv24Bit(void) {
	if (m_lpBMIH->biBitCount >= 16) return;
	LPBITMAPINFOHEADER new_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER)];
	memcpy(new_lpBMIH, m_lpBMIH, sizeof(BITMAPINFOHEADER));
	new_lpBMIH->biBitCount = 24;
	new_lpBMIH->biClrUsed = 0;
	new_lpBMIH->biClrImportant = 0;	
	int nRowBytesOld = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;
	int nRowBytes = (((new_lpBMIH->biWidth * new_lpBMIH->biBitCount) + 31) & ~31) / 8;
	int l_newSize = new_lpBMIH->biHeight * nRowBytes;
	LPBYTE new_Image = new BYTE[l_newSize];
	for (int x=0;x<m_lpBMIH->biWidth;x++)
		for (int y=0;y<m_lpBMIH->biHeight;y++)
			PutPixel(x, y, GetPixel(x, y, m_lpBMIH, m_lpImage, nRowBytesOld), new_lpBMIH, new_Image, nRowBytes);
	delete m_lpBMIH;
	delete m_lpImage;
	m_lpBMIH = new_lpBMIH;
	m_lpImage = new_Image;
	m_Invalid = TRUE;
}

void CDib::Sharpen(void) {
	/*
	static float m_Sharpen[] = {
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	};
	*/
	/*
	static float m_Sharpen[] = {
		0, -1, 0,
		-1, 5, -1,
		0, -1, 0
	};
	*/
	static float m_Sharpen[] = {
		1, -2, 1,
		-2, 5, -2,
		1, -2, 1
	};
	Cnv24Bit();
	Convolve(m_Sharpen, 3, 3);
}

void CDib::SobelEdgeDetect(BOOL i_Overlay, unsigned int i_Threshold) {
	theApp.ShowProgress("Sobel edge detect, please wait.");
	/* allocate the output image buffer */
	int l_newSize = m_lpBMIH->biHeight * ((((m_lpBMIH->biWidth) + 31) & ~31) / 8);
	LPBYTE new_Image = new BYTE[l_newSize];
	/* create a new duotone palette */
	LPBITMAPINFOHEADER new_lpBMIH = CreateDuotonePalette();	
	new_lpBMIH->biSizeImage  = l_newSize;
	/* pre-calculate row byte count */
	int nRowBytesOld = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;
	int nRowBytes = (((new_lpBMIH->biWidth * new_lpBMIH->biBitCount) + 31) & ~31) / 8;
	/* clear the output buffer (fill with background) */
	unsigned int l_Col = 0, l_Row = 0;
	register unsigned int l_ImageRow, l_ImageCol;
	unsigned int l_Width = m_lpBMIH->biWidth;
	unsigned int l_Height = m_lpBMIH->biHeight;
	for (l_ImageRow = 0; l_ImageRow < l_Width; l_ImageRow++)
		for (l_ImageCol = 0; l_ImageCol < l_Height; l_ImageCol++)
			PutPixel(l_ImageRow, l_ImageCol, 1, new_lpBMIH, new_Image, nRowBytes);	
	/* compensate for edge effets of 3x3 pixel neighborhood */
	l_Col++;
	l_Row++;
	l_Width -= 2;
	l_Height -= 2;
	unsigned int l_ColExtent = l_Col + l_Width;
	unsigned int l_RowExtent = l_Row + l_Height;
	int l_Pt[9]; /* A=0 B=1 C=2 D=3 E=4 F=5 G=6 H=7 I=8 J=9 */
	int l_Line[12];
		/*
			LineAEIAveAbove=0 LineAEIAveBelow=1 LineAEIMaxDiff=2
			LineBEHAveAbove=3 LineBEHAveBelow=4 LineBEHMaxDiff=5
			LineCEGAveAbove=6 LineCEGAveBelow=7 LineCEGMaxDiff=8
			LineDEFAveAbove=9 LineDEFAveBelow=10 LineDEFMaxDiff=11
			*/
	int l_x, l_y, z;
	unsigned int l_MaxDif;
	for (l_ImageRow = l_Row; l_ImageRow < l_RowExtent; l_ImageRow++) {
		theApp.SetProgress(l_ImageRow * 100 / l_RowExtent);
		for (l_ImageCol = l_Col; l_ImageCol < l_ColExtent; l_ImageCol++) {
			
			/* pixel neighborhood */
			z = 0;
			for (l_x = -1;l_x <= 1;l_x++) {
				for (l_y = -1; l_y <= 1; l_y++) {
					l_Pt[z++] = GetGray((int)l_x+(int)l_ImageCol, (int)l_y+(int)l_ImageRow, m_lpBMIH, m_lpImage, nRowBytesOld);
				}
			}		
			/* average above and below the line
				absolute value of the difference */

			l_Line[0] = (l_Pt[1]+l_Pt[2]+l_Pt[5])/3;
			l_Line[1] = (l_Pt[3]+l_Pt[6]+l_Pt[7])/3;
			l_Line[2] = abs(l_Line[0] - l_Line[1]);

			l_Line[3] = (l_Pt[2]+l_Pt[5]+l_Pt[8])/3;
			l_Line[4] = (l_Pt[0]+l_Pt[3]+l_Pt[6])/3;
			l_Line[5] = abs(l_Line[3] - l_Line[4]);

			l_Line[6] = (l_Pt[0]+l_Pt[1]+l_Pt[3])/3;
			l_Line[7] = (l_Pt[5]+l_Pt[7]+l_Pt[8])/3;
			l_Line[8] = abs(l_Line[7] - l_Line[6]);

			l_Line[9] = (l_Pt[0]+l_Pt[1]+l_Pt[2])/3;
			l_Line[10] = (l_Pt[6]+l_Pt[7]+l_Pt[8])/3;
			l_Line[11] = abs(l_Line[9] - l_Line[10]);

			l_MaxDif = max(max(max(l_Line[2], l_Line[5]), l_Line[8]), l_Line[11]);
			if (l_MaxDif > i_Threshold)
				PutPixel(l_ImageCol, l_ImageRow, 0, new_lpBMIH, new_Image, nRowBytes);
			else if (i_Overlay)
				PutPixel(l_ImageCol, l_ImageRow, 0xFF - l_Pt[4], new_lpBMIH, new_Image, nRowBytes);
		}
	}
	delete m_lpImage;
	m_lpImage = new_Image;
	delete m_lpBMIH;
	m_lpBMIH = new_lpBMIH;
	theApp.HideProgress();
	m_Invalid = TRUE;
}	

int DwordCompare(const void * E1, const void * E2) {
	if (* (DWORD *) E1 < * (DWORD *) E2) return -1;
	else if (* (DWORD *) E1 > * (DWORD *) E2) return 1;
	else return 0;
}

void CDib::MedianFilter(unsigned int i_NCols, unsigned int i_NRows) {
	if ((unsigned int) m_lpBMIH->biWidth < i_NCols) return;
	if ((unsigned int)m_lpBMIH->biHeight < i_NRows) return;

	theApp.ShowProgress("Median filter, please wait.");

	register unsigned int l_ImageRow, l_ImageCol, l_NeighborCol, l_NeighborRow;
	unsigned int l_ColOffset, l_RowOffset, l_TempCol, l_TempRow, l_PixelIndex;
	
	int nRowBytes = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;	
	int l_newSize = m_lpBMIH->biHeight * nRowBytes;
	LPBYTE new_Image = new BYTE[l_newSize];
	/* fill missing pixels (convolve cuts) */
	memcpy(new_Image, m_lpImage, l_newSize);
	
	/* calculate border pixel to miss */
	l_ColOffset = i_NCols / 2;
	l_RowOffset = i_NRows / 2;
	/* compensate for edge defects */
	unsigned int l_Col = l_ColOffset;
	unsigned int l_Row = l_RowOffset;
	unsigned int l_Width = m_lpBMIH->biWidth - i_NCols + 1;
	unsigned int l_Height = m_lpBMIH->biHeight - i_NRows + 1;
	/* calcualte new range of pixels */
	register unsigned int l_ColExtent = l_Col + l_Width;
	register unsigned int l_RowExtent = l_Row + l_Height;

	unsigned int l_TotalPixels = i_NCols * i_NRows;
	unsigned int l_MedianIndex = l_TotalPixels / 2;

	DWORD * l_PixelValues = (DWORD *) calloc(l_TotalPixels, (unsigned)sizeof(DWORD));
	
	for (l_ImageRow = l_Row; l_ImageRow < l_RowExtent; l_ImageRow++) {
		theApp.SetProgress(l_ImageRow * 100 / l_RowExtent);
		l_TempRow = l_ImageRow - l_RowOffset;
		for (l_ImageCol = l_Col; l_ImageCol < l_ColExtent; l_ImageCol++){
			l_TempCol = l_ImageCol - l_ColOffset;
			l_PixelIndex = 0;
			for (l_NeighborCol=0; l_NeighborCol < i_NCols; l_NeighborCol++)
				for (l_NeighborRow=0; l_NeighborRow < i_NRows; l_NeighborRow++)
					l_PixelValues[l_PixelIndex++] =
						GetPixel(l_TempCol + l_NeighborCol, l_TempRow + l_NeighborRow, m_lpBMIH, m_lpImage, nRowBytes);
			qsort(l_PixelValues, l_TotalPixels, sizeof(DWORD), &DwordCompare);
			PutPixel(l_ImageCol, l_ImageRow, l_PixelValues[l_MedianIndex], m_lpBMIH, new_Image, nRowBytes);
		}
	}
	delete m_lpImage;
	m_lpImage = new_Image;
	m_Invalid = TRUE;
	theApp.HideProgress();
	free(l_PixelValues);
}

/*
	Thinning
	*/

BOOL CDib::ThinPatternMatchA(int * i_Neighborhood) {
	/*
		A A A 0 P 0 B B B
		at least one of the groups must be non-zero
		*/
	if (i_Neighborhood[3] || i_Neighborhood[5]) return FALSE;
	if (i_Neighborhood[0] || i_Neighborhood[1] || i_Neighborhood[2]) return TRUE;
	if (i_Neighborhood[6] || i_Neighborhood[7] || i_Neighborhood[8]) return TRUE;
	return FALSE;
}

BOOL CDib::ThinPatternMatchB(int * i_Neighborhood) {
	/*
		A A A A P 0 A 0 2
		*/
	if (i_Neighborhood[5] || i_Neighborhood[7]) return FALSE;
	if (i_Neighborhood[8] != 2) return FALSE;
	if (i_Neighborhood[0] || i_Neighborhood[1] || i_Neighborhood[2] || i_Neighborhood[3] || i_Neighborhood[6]) return TRUE;
	return FALSE;
}

int * CDib::ThinRotateRight(int * i_Neighborhood) {
	int l_T = i_Neighborhood[6];
	i_Neighborhood[6] = i_Neighborhood[8];
	i_Neighborhood[8] = i_Neighborhood[2];
	i_Neighborhood[2] = i_Neighborhood[0];
	i_Neighborhood[0] = l_T;
	
	l_T = i_Neighborhood[3];
	i_Neighborhood[3] = i_Neighborhood[7];
	i_Neighborhood[7] = i_Neighborhood[5];
	i_Neighborhood[5] = i_Neighborhood[1];
	i_Neighborhood[1] = l_T;

	return i_Neighborhood;
}

int * CDib::ThinRotateLeft(int * i_Neighborhood) {
	int l_T = i_Neighborhood[0];
	i_Neighborhood[0] = i_Neighborhood[2];
	i_Neighborhood[2] = i_Neighborhood[8];
	i_Neighborhood[8] = i_Neighborhood[6];
	i_Neighborhood[6] = l_T;
	
	l_T = i_Neighborhood[1];
	i_Neighborhood[1] = i_Neighborhood[5];
	i_Neighborhood[5] = i_Neighborhood[7];
	i_Neighborhood[7] = i_Neighborhood[3];
	i_Neighborhood[3] = l_T;

	return i_Neighborhood;
}

void CDib::RobertsEdgeDetect(void) {	
	CnvMonochrome();
	theApp.ShowProgress("Roberts edge detect, please wait.");
	int nRowBytes = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;	
	int l_DiffDiag, l_DiagDiff;
	DWORD l_Pixel, s_Pixel, t_Pixel, d_Pixel;
	for (int y = 1; y < m_lpBMIH->biHeight-1; y++) {
		theApp.SetProgress(y * 100 / m_lpBMIH->biHeight);
		for (int x = 0; x < m_lpBMIH->biWidth-1; x++) {    
			//k = y*m_lpBMIH->biWidth+x;
			l_Pixel = (GetPixel(x, y, m_lpBMIH, m_lpImage, nRowBytes))?1:0;
			s_Pixel = (GetPixel(x+1, y+1, m_lpBMIH, m_lpImage, nRowBytes))?1:0;
			t_Pixel = (GetPixel(x+1, y, m_lpBMIH, m_lpImage, nRowBytes))?1:0;
			d_Pixel = (GetPixel(x, y+1, m_lpBMIH, m_lpImage, nRowBytes))?1:0;

			l_DiffDiag = l_Pixel - s_Pixel;
			l_DiagDiff = t_Pixel - d_Pixel;
			PutPixel(x, y, (abs(l_DiffDiag)+abs(l_DiagDiff))?0:1, m_lpBMIH, m_lpImage, nRowBytes);
		}
	}
	theApp.HideProgress();
	m_Invalid = TRUE;
}

void CDib::LaplacianEdgeDetect(int l_Type) {
	CnvMonochrome();
	theApp.ShowProgress("Laplacian edge detect, please wait.");
	static int l_Maskl[3][3][3];
	l_Maskl[0][0][0]= 0;
	l_Maskl[0][1][0]= -1;
	l_Maskl[0][2][0]= 0;
	l_Maskl[1][0][0]= -1;
	l_Maskl[1][1][0]= 4;
	l_Maskl[1][2][0]= -1;
	l_Maskl[2][0][0]= 0;
	l_Maskl[2][1][0]= -1;
	l_Maskl[2][2][0]= 0;
	l_Maskl[0][0][1]= -1;
	l_Maskl[0][1][1]= -1;
	l_Maskl[0][2][1]= -1;
	l_Maskl[1][0][1]= -1;
	l_Maskl[1][1][1]= 8;
	l_Maskl[1][2][1]= -1;
	l_Maskl[2][0][1]= -1;
	l_Maskl[2][1][1]= -1;
	l_Maskl[2][2][1]= -1;
	l_Maskl[0][0][2]= 1;
	l_Maskl[1][0][2]= -2;
	l_Maskl[2][0][2]= 1;
	l_Maskl[0][1][2]= -2;
	l_Maskl[1][1][2]= 4;
	l_Maskl[2][1][2]= -2;
	l_Maskl[0][2][2]= 1;
	l_Maskl[1][2][2]= -2;
	l_Maskl[2][2][2]= 1;
	
	int l_ValSum, xx, yy, x, y, l_Pixel;
	int nRowBytes = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;	
	for (y = 1; y < m_lpBMIH->biHeight-1; y++) {
		theApp.SetProgress(y * 100 / m_lpBMIH->biHeight);
		for (x = 0; x < m_lpBMIH->biWidth-1; x++) {    
			l_ValSum = 0;
			for (xx = -1; xx < 2; xx++)
				for (yy = -1; yy < 2; yy++) {
					l_Pixel = (GetPixel(x+xx, y+yy, m_lpBMIH, m_lpImage, nRowBytes))?1:0;
					l_Pixel *= l_Maskl[xx+1][yy+1][l_Type];
					l_ValSum += l_Pixel;					
				}
			PutPixel(x, y, (l_ValSum)?0:1, m_lpBMIH, m_lpImage, nRowBytes);
		}
	}
	for (x = 0; x < m_lpBMIH->biWidth; x++) {		
		PutPixel(x, 0, 0, m_lpBMIH, m_lpImage, nRowBytes);
		PutPixel(x, m_lpBMIH->biHeight-1, 0, m_lpBMIH, m_lpImage, nRowBytes);
	}
	for (y = 0; y < m_lpBMIH->biHeight; y++) {		
		PutPixel(0, y, 0, m_lpBMIH, m_lpImage, nRowBytes);
		PutPixel(m_lpBMIH->biWidth-1, y, 0, m_lpBMIH, m_lpImage, nRowBytes);
	}
	theApp.HideProgress();
	Invert();	
	m_Invalid = TRUE;
}

void CDib::Thin(void){
	CnvMonochrome();
	theApp.ShowProgress("Thinning, please wait.");
	int k, x, y;
	int l_Pixel;
	int j_Neighbor;
	int l_tmpSize = (m_lpBMIH->biWidth*m_lpBMIH->biHeight);
	unsigned char * l_Buffer = new unsigned char[l_tmpSize];
	int nRowBytes = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;	

	for (y = 0; y < m_lpBMIH->biHeight; y++) {
		for (x = 0; x < m_lpBMIH->biWidth; x++) {    
			k = y*m_lpBMIH->biWidth+x;
			l_Buffer[k] = (GetPixel(x, y, m_lpBMIH, m_lpImage, nRowBytes))?0:1;
		}
	}		

	int * l_Pattern = new int[9];
	int z, xx, yy;
	BOOL l_Remain = TRUE;
	BOOL l_Skel;
	int l_Step = 0;
	CString l_StepMsg;
	while (l_Remain){
		l_Remain = FALSE;
		for (int j=0;j<=6;j+=2){
			l_StepMsg.Format("Thinning, step %d, please wait.", l_Step++);			
			theApp.SetProgress(l_StepMsg);
			for (y = 0; y < m_lpBMIH->biHeight; y++) {
				theApp.SetProgress(y * 100 / m_lpBMIH->biHeight);
				for (x = 0; x < m_lpBMIH->biWidth; x++) {    
					k = y*m_lpBMIH->biWidth+x;
					l_Pixel = l_Buffer[k];
					if (l_Pixel == 1){
						/* get the j-neighbour */
						if (j==0) j_Neighbor = (x < m_lpBMIH->biWidth-1)?GetPixel(x+1,y, m_lpBMIH, m_lpImage, nRowBytes):0;
						else if (j==2) j_Neighbor = (y)?GetPixel(x,y-1, m_lpBMIH, m_lpImage, nRowBytes):0;
						else if (j==4) j_Neighbor = (x)?GetPixel(x-1,y, m_lpBMIH, m_lpImage, nRowBytes):0;
						else if (j==6) j_Neighbor = (y < m_lpBMIH->biHeight-1)?GetPixel(x,y+1, m_lpBMIH, m_lpImage, nRowBytes):0;
						if (j_Neighbor) {
							l_Skel = FALSE;
							z = 0;
							for (xx=-1;xx<=1;xx++)
								for (yy=-1;yy<=1;yy++){
									if ((x + xx > 0)&&(y + yy > 0)&&(x + xx < m_lpBMIH->biWidth)&&(y+yy < m_lpBMIH->biHeight))
										l_Pattern[z++] = GetPixel(x+xx,y+yy, m_lpBMIH, m_lpImage, nRowBytes);
									else l_Pattern[z++] = 0;
								}

							/* attempt to match patterns */
							if (ThinPatternMatchA(l_Pattern)) l_Skel = TRUE;
							else if (ThinPatternMatchB(l_Pattern)) l_Skel = TRUE;
							else if (ThinPatternMatchA(ThinRotateLeft(l_Pattern))) l_Skel = TRUE;
							else if (ThinPatternMatchB(l_Pattern)) l_Skel = TRUE;
							else if (ThinPatternMatchB(ThinRotateLeft(l_Pattern))) l_Skel = TRUE;
							else if (ThinPatternMatchB(ThinRotateLeft(l_Pattern))) l_Skel = TRUE;
							
							if (l_Skel) {
							    l_Buffer[k] = 2;
							} else {
								l_Remain = TRUE;
								l_Buffer[k] = 3;
							}
						}
					}
				}
			}
			for (xx = 0; xx < l_tmpSize; xx++) if (l_Buffer[xx] == 3) l_Buffer[xx] = 0;
		}
	}
	
	for (y = 0; y < m_lpBMIH->biHeight; y++) {
		for (x = 0; x < m_lpBMIH->biWidth; x++) {    
			k = y*m_lpBMIH->biWidth+x;
			PutPixel(x, y, 1-l_Buffer[k], m_lpBMIH, m_lpImage, nRowBytes);
		}
	}
	delete l_Pattern;
	theApp.HideProgress();	
	m_Invalid = TRUE;
	
}

#define BITSET(X,I) (X&(I<<0x01))?1:0
#define BITCOUNT(X) BITSET(X,0)+BITSET(X,1)+BITSET(X,2)+BITSET(X,3)+BITSET(X,4)+BITSET(X,5)+BITSET(X,6)+BITSET(X,7)
#define BITCOUNTONE(X) ((X == 0x01)||(X == 0x02)||(X == 0x04)||(X == 0x08)||(X == 0x10)||(X == 0x20)||(X == 0x40)||(X == 0x60)||(X == 0x80)) 
#define MAKEBDR \
	l_Pixel = l_Buffer[k];  \
	a_bdr = 0; \
	a_bdr += (l_Buffer[k+1] == 1)?0x80:0; \
	a_bdr += (l_Buffer[k-m_lpBMIH->biWidth+1] == 1)?0x40:0; \
	a_bdr += (l_Buffer[k-m_lpBMIH->biWidth] == 1)?0x20:0; \
	a_bdr += (l_Buffer[k-m_lpBMIH->biWidth-1] == 1)?0x10:0; \
	a_bdr += (l_Buffer[k-1] == 1)?0x08:0; \
	a_bdr += (l_Buffer[k+m_lpBMIH->biWidth-1] == 1)?0x04:0; \
	a_bdr += (l_Buffer[k+m_lpBMIH->biWidth] == 1)?0x02:0; \
	a_bdr += (l_Buffer[k+m_lpBMIH->biWidth+1] == 1)?0x01:0; \
	a_bdr = ~a_bdr; \
	memset(bdr, 0, sizeof(BYTE)*4); \
	for (l_bdr = 0;l_bdr < l_Pixel; l_bdr++) bdr[l_bdr] = a_bdr;

void CDib::ThinAsync(void){
	CnvMonochrome();
	
	int k, x, y;
	int l_Pixel;
	int l_tmpSize = (m_lpBMIH->biWidth*m_lpBMIH->biHeight);
	unsigned char * l_Buffer = new unsigned char[l_tmpSize];
	unsigned char * l_BufferFinal = new unsigned char[l_tmpSize];
	int nRowBytes = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;	

	int l_C = 5, l_bdr;
	BOOL l_Erase = TRUE;
	BYTE bdr[4], a_bdr;	
	while (l_Erase) {
		for (y = 0; y < m_lpBMIH->biHeight; y++) {
			for (x = 0; x < m_lpBMIH->biWidth; x++) {    
				k = y*m_lpBMIH->biWidth+x;
				l_BufferFinal[k] = (GetPixel(x, y, m_lpBMIH, m_lpImage, nRowBytes) > 0)?0:1;
				l_Buffer[k] = l_BufferFinal[k];
			}
		}
		l_C++;
		for (y = 1; y < m_lpBMIH->biHeight-1; y++) {
			for (x = 1; x < m_lpBMIH->biWidth-1; x++) {    
				k = y*m_lpBMIH->biWidth+x;
				MAKEBDR
				/* pixel belongs to the contour? */
				if ((bdr[0] && (bdr[0]&0xAA)!= 0xAA)) {
					if (BITCOUNTONE(bdr[0])) {
						l_BufferFinal[k] = 3;
					} else if (((bdr[0] & 0x70) != 0)&&((bdr[0] & 0x7) != 0)&&((bdr[1] & 0x88) == 0)) {
						l_BufferFinal[k] = 3;
					} else if (((bdr[0] & 0x82) == 0)&&((bdr[0] & 0x01) != 0)) {
						l_BufferFinal[k] = 3;
					}
				}
			}
		} // for each contour pixel
		memcpy(l_Buffer, l_BufferFinal, l_tmpSize);
		for (y = 1; y < m_lpBMIH->biHeight-1; y++) {
			for (x = 1; x < m_lpBMIH->biWidth-1; x++) {    
				k = y*m_lpBMIH->biWidth+x;
				MAKEBDR
				if (bdr[0] == bdr[1]) l_BufferFinal[k] = 4;
				else if ((bdr[0] & 0xAA) != 0xAA) {
					if (((bdr[1] & 0x80) != 0) && ((bdr[0] & 0x08) == 0)) {
							if (((bdr[0] & 0x40) > 0) && ((bdr[0] & 0x01) > 0)) l_BufferFinal[k] = 4;
							else if (((bdr[0] & 0x30) > 0) && ((bdr[0] & 6) > 0)) {
								if (((bdr[0] & 0x40) > 0) || ((bdr[0] & 0x01) > 0)) l_BufferFinal[k] = 4;
							}
					}
				}
			}
		}
		memcpy(l_Buffer, l_BufferFinal, l_tmpSize);
		for (y = 1; y < m_lpBMIH->biHeight-1; y++) {
			for (x = 1; x < m_lpBMIH->biWidth-1; x++) {    
				k = y*m_lpBMIH->biWidth+x;
				MAKEBDR
				if ((bdr[3] & 0x08) == 0) l_BufferFinal[k] = 5;
				else if (((bdr[3] & 0x20) == 0) && (bdr[4] == 0)) l_BufferFinal[k] = 5;
				if ((l_Buffer[k] == 3) || (l_Buffer[k] == 4)) l_BufferFinal[k] = l_C;
			}
		}
		l_Erase = FALSE;
		for (y = 1; y < m_lpBMIH->biHeight-1; y++) {
			for (x = 1; x < m_lpBMIH->biWidth-1; x++) {    
				k = y*m_lpBMIH->biWidth+x;
				if ((l_BufferFinal[k] == 2) || (l_BufferFinal[k] == 5)) {
					PutPixel(x, y, 1, m_lpBMIH, m_lpImage, nRowBytes);
					l_Erase = TRUE;
				} 
			}
		}
	}
	
	delete l_Buffer;
	delete l_BufferFinal;
	m_Invalid = TRUE;
}



void CDib::Flip(BOOL i_Horizontal, BOOL i_Vertical) {
	int nRowBytes = (((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8;	
	int l_newSize = m_lpBMIH->biHeight * ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
	LPBYTE new_Image = new BYTE[l_newSize];
	int x,y;
	for (y = 0; y < m_lpBMIH->biHeight; y++) {
		for (x = 0; x < m_lpBMIH->biWidth; x++) {
			if (i_Horizontal && i_Vertical) {
				PutPixel(x, y, GetPixel(m_lpBMIH->biWidth-x-1,m_lpBMIH->biHeight-y-1, m_lpBMIH, m_lpImage, nRowBytes), m_lpBMIH, new_Image, nRowBytes);
			} else if (i_Horizontal) {
				PutPixel(x, y, GetPixel(m_lpBMIH->biWidth-x-1,y, m_lpBMIH, m_lpImage, nRowBytes), m_lpBMIH, new_Image, nRowBytes);
			} else if (i_Vertical) {
				PutPixel(x, y, GetPixel(x,m_lpBMIH->biHeight-y-1, m_lpBMIH, m_lpImage, nRowBytes), m_lpBMIH, new_Image, nRowBytes);
			}
		}
	}
	delete m_lpImage;
	m_lpImage = new_Image;
	m_Invalid = TRUE;
}

void CDib::Erase(void) {
	register int i,j;
	for (i = 0; i < m_lpBMIH->biHeight; ++i) {
		for (j = 0; j < m_lpBMIH->biWidth; ++j) {      			
			PutPixel(j,i,RGB(0xFF, 0xFF, 0xFF));
		}
	}
	m_Invalid = TRUE;	
}

void CDib::PutPixelRegion(int xmin, int ymin, int xmax, int ymax, DWORD i_Color) {
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)m_lpBMIH;		
	WORD Color_16;
	DWORD Color_32;
	int nColors, x, y;
	int nRowBytes = ((((m_lpBMIH->biWidth * m_lpBMIH->biBitCount) + 31) & ~31) / 8);
	switch(m_lpBMIH->biBitCount){
		int i;
	case 1:
	case 4:
		for (x = xmin; x < xmax; x++)
			for (y = ymin; y < ymax; y++){
				PutPixel(x, y, 0xFFFFFFFF, m_lpBMIH, m_lpImage, nRowBytes);
		}
	case 8:
		nColors = m_lpBMIH->biClrUsed ? m_lpBMIH->biClrUsed: 1 << m_lpBMIH->biBitCount;	
		for(i = 0;i<nColors;i++) {
			if( bmInfo.bmiColors[i].rgbBlue ==  GetBValue(i_Color)
				&& bmInfo.bmiColors[i].rgbGreen ==  GetGValue(i_Color)
				&& bmInfo.bmiColors[i].rgbRed ==  GetRValue(i_Color)) {
				for (x = xmin; x < xmax; x++)
					for (y = ymin; y < ymax; y++){						
						*((LPBYTE)m_lpImage + nRowBytes*(y) + (x)) = (BYTE) i;						
				}				
				break;
			}
		}
		break;
	case 16:
		Color_16 = (WORD) GetBValue(i_Color) + (5 << GetGValue(i_Color)) + (10 << GetRValue(i_Color));
		for (x = xmin; x < xmax; x++)
			for (y = ymin; y < ymax; y++){						
				*((LPWORD)((LPBYTE)m_lpImage + nRowBytes*y + x*2)) = Color_16;
			}				
		break;
	case 24:
		Color_32 = (GetRValue(i_Color) << 0x10) + (GetGValue(i_Color) << 0x08) + GetBValue(i_Color);
		for (x = xmin; x < xmax; x++)
			for (y = ymin; y < ymax; y++){								
				*((LPDWORD)((LPBYTE)m_lpImage + nRowBytes*y + x*3)) = 
					*((LPDWORD)((LPBYTE)m_lpImage + nRowBytes*y + x*3)) & 0xff000000
					| Color_32;
			}
		break;
	case 32:
		for (x = xmin; x < xmax; x++)
			for (y = ymin; y < ymax; y++)
				*((LPDWORD)((LPBYTE)m_lpImage + nRowBytes*y + x*4)) = i_Color;
		break;
	}	
}