/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef IVCPP_CDIB_H
#define IVCPP_CDIB_H

#include <afxole.h>

#ifndef M_PI
#define M_PI (double) 3.14159265358979
#endif

#include "defuns.h"

#include <sys/timeb.h>
#include <time.h>
#include "DibHistogram.h"

class CDibTracker;

class CDib : public CObject {
	friend class CDibHistogram;
private:	
	/*
		Redraw Double-Buffer
		*/
	struct _timeb timebuffer;
	double m_SaveAngle;
	BOOL m_Invalid;
	CDC * m_SaveDC;	
	CBitmap * m_SaveBitmap;
	CSize m_SaveSize;
	BOOL HasChanged(CSize size);
	void PrePaintPicture(BOOL Force, CSize size, CDC * pDC);
	/* 
		Rotation and Resize Constants 
		*/	
	double m_angle;
	double cosine, sine;	
	int x1, y1, x2, y2, x3, y3;	
	int minx, miny, maxx, maxy;		
	int w, h, l, t; 
	int nw, nh;
	CRect m_RotateRect;
	/*
		Rotatation and Resize Transformation Functions
		*/	
	CPoint CalcStretch(CPoint APoint);
	HANDLE GetRotatedBitmap(CDC *, double radians, COLORREF clrBack, CPoint origin, CSize size, BOOL DoShow = TRUE);	
	DWORD HandleBackgroundColor(LPVOID lpDIBBitsResult, long len, COLORREF clrBack, int bpp, int nColors, BITMAPINFO &bmInfo) const;
	/*
		Raw and Low Level Data
		*/
	enum Alloc {noAlloc, crtAlloc, heapAlloc};
	DECLARE_SERIAL(CDib)
	LPVOID m_lpvColorTable;
	HBITMAP m_hBitmap;
	LPBYTE m_lpImage;  // starting address of DIB bits
	LPBITMAPINFOHEADER m_lpBMIH; //  buffer containing the BITMAPINFOHEADER
	HGLOBAL m_hGlobal; // For external windows we need to free	
	Alloc m_nBmihAlloc;
	Alloc m_nImageAlloc;
	DWORD m_dwSizeImage; // # of bits - not BITMAPINFOHEADER or BITMAPFILEHEADER
	int m_nColorTableEntries;
	CDib& Clone(const CDib&);
	/*
		File Attachments, Maps and Palettes
		*/
	HANDLE m_hFile;
	HANDLE m_hMap;
	LPVOID m_lpvFile;
	HPALETTE m_hPalette;
	void DetachMapFile();
	void ComputePaletteSize(int nBitCount);
	void ComputeMetrics();
	LPBITMAPINFOHEADER CreateGrayscalePalette();
	LPBITMAPINFOHEADER CreateDuotonePalette();
	/*
		Histogram and Color Transforms
		*/	
	void BuildHistogram(void);		
	void Convolve(float *, unsigned int, unsigned int);
	BOOL ThinPatternMatchA(int * i_Neighborhood);
	BOOL ThinPatternMatchB(int * i_Neighborhood);
	int * ThinRotateRight(int * i_Neighborhood);
	int * ThinRotateLeft(int * i_Neighborhood);
public:
	/*
		Constructors
		*/
	CDib();
	CDib(const CDib&);
	CDib(CSize size, int nBitCount);	// builds BITMAPINFOHEADER
	~CDib();
	int nWidth, nHeight;
	COleDataSource* SaveDib();
	void CopyDibTo(void);
	CDib& operator=(const CDib&);
	BOOL isEmpty(void);
	int GetSizeImage(void) const;
	int GetSizeHeader(void) const;
	int GetDepth(void) const;
	CSize GetDimensions() const;
	BOOL AttachMapFile(const char* strPathname, BOOL bShare = FALSE);
	BOOL CopyToMapFile(const char* strPathname);
	BOOL AttachMemory(LPVOID lpvMem, BOOL bMustDelete = FALSE, HGLOBAL hGlobal = NULL);
	/*
		Double-Buffer
		*/
	void Invalidate(void);
	/*
		Elementary Modifiers
		*/
	DWORD GetPixel(int x, int y, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	DWORD GetNeighborPixel(int x, int y, int j, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	BYTE GetNeighborPixelByte(int x, int y, int j, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	void PutPixel(int x, int y, DWORD color, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1);
	void PutPixelRegion(int xmin, int ymin, int xmax, int ymax, DWORD color);
	unsigned int GetRed(int x, int y, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	unsigned int GetGreen(int x, int y, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	unsigned int GetBlue(int x, int y, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	unsigned int GetGray(int x, int y, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	unsigned int GetGrayLog(int x, int y, LPBITMAPINFOHEADER im_lpBMIH = NULL, LPBYTE im_lpImage = NULL, int nRowBytes = -1) const;
	/*
		Rotation and Transform Information
		*/
	CPoint CalcRealPoint(int sourcex, int sourcey);
	int RowBytes(void) const;
	CRgn& GetRotateRgn(CPoint origin, CSize size, CRgn&);
	CPoint GetTopLeft(CPoint origin, CSize size);
	CPoint GetTopRight(CPoint origin, CSize size);
	CPoint GetBottomLeft(CPoint origin, CSize size);
	CPoint GetBottomRight(CPoint origin, CSize size);	
	void MakeRotateConstants(CPoint origin, CSize size, BOOL Invert = FALSE);
	BOOL SetAngle(double Value);	
	BOOL SetDegAngle(int Value);
	double GetAngle(void);
	int GetDegAngle(void);
	CRect GetRotateBounds(CPoint origin, CSize size);
	double GetXScale(void);
	double GetYScale(void);
	int GetXPercent(void);
	int GetYPercent(void);
	inline int GetWidth(void) const { return m_lpBMIH->biWidth; }
	inline int GetHeight(void) const { return m_lpBMIH->biHeight; }
	/*
		Bitmap and Palette
		*/
	BOOL Draw(CDC* pDC, CPoint origin, CSize size);  // until we implemnt CreateDibSection
	HBITMAP CreateSection(CDC* pDC = NULL);
	UINT UsePalette(CDC* pDC, BOOL bBackground = FALSE);
	BOOL MakePalette();
	BOOL SetSystemPalette(CDC* pDC);
	BOOL Compress(CDC* pDC, BOOL bCompress = TRUE); // FALSE means decompress
	HBITMAP CreateBitmap(CDC* pDC);
	BOOL Read(CFile* pFile);
	BOOL ReadSection(CFile* pFile, CDC* pDC = NULL);
	BOOL Write(CFile* pFile);
	void Serialize(CArchive& ar);
	void Empty();
	/*
		Histogram
		*/
	CDibHistogram m_Histogram;
	/*
		Elementary Color Transformations
		*/
	void CnvMonochrome(void);
	void CnvGrayLevel(BOOL i_Log = FALSE);
	void Cnv24Bit();
	void FloydSteinberg(void);		
	void EqualizeHistogram(void);
	void Invert(void);
	void Blur(void);
	void BlurMore(void);
	void Sharpen(void);
	void SobelEdgeDetect(BOOL i_Overlay, unsigned int i_Threshold);
	void MedianFilter(unsigned int i_NCols, unsigned int i_NRows);
	void Thin(void);
	void ThinAsync(void);
	void RobertsEdgeDetect(void);
	void LaplacianEdgeDetect(int l_Type = 0);
	void Flip(BOOL i_Horizontal, BOOL i_Vertical);
	void Erase(void);
	/*
		Properties
		*/
	PROPERTY(CDibTracker *, DibTracker);
	PROPERTY(CString, Comment);
};
#endif
