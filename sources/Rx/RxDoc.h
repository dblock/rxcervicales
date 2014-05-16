/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#if !defined(AFX_RXDOC_H__F481A56B_ABBA_11D2_9BFD_000001352962__INCLUDED_)
#define AFX_RXDOC_H__F481A56B_ABBA_11D2_9BFD_000001352962__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRxDoc;

#include "CDib.h"
#include "DibTracker.h"
#include "DibTrackerManager.h"
#include "AVectorManager.h"
#include "AnnotationManager.h"
#include "ToolCervicales.h"
#include "DibInformation.h"

class CRxDoc : public CDocument {
	friend class CDibInformation;
protected:
	CRxDoc();
	CDibTrackerManager m_DibManager;
	CAVectorManager m_VectorManager;
	CAnnotationManager m_AnnotationManager;
	DECLARE_DYNCREATE(CRxDoc)
public:
	//{{AFX_VIRTUAL(CRxDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();	
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL
public:	
	/*
		DIB operators
		*/
	char * m_Cursor;
	CSize m_paperSize;	
	UINT UsePalette(CDC* pDC, BOOL bBackground = FALSE);	
	void OnViewPaletteChanged(CClientDC& dc, UINT wParam);	
	void Add(const CDib&);
	void Add(const CAnnotation&);
	COleDataSource* SaveDib(void);
	void CopyDibTo(void);
	void EditPastefrom(CClientDC&);	
	void EditPaste(CClientDC&);
	CAVector * GetSelectedVector(void);
	CAVector * GetTailVector(void);
	CDib * GetSelectedDIB(void);
	int GetSelectedDIBIndex(void) const;
	/*
		VECTOR operators
		*/
	void Add(const CAVector&);	
	BOOL Select(const CPoint&);
	BOOL CanSelect(const CPoint&);
	BOOL Deselect(void);
	BOOL DeselectVector(void);	
	BOOL DeselectDib(void);
	BOOL DeselectAnnotation(void);
	BOOL DeleteSelected(void);
	BOOL MouseMove(const CPoint&);
	BOOL OnMouseMove(const CPoint& point);
	CRect GetBoundMove();
	COLORREF GetColor() const;
	BOOL SetColor(COLORREF);	
	BOOL SetSelectedColor(COLORREF);		
	void OnEditClearSelected();
	void MouseUp(const CPoint&);
	/*
		Other
		*/
	virtual ~CRxDoc();	
	void Paint(CDC *, const CRedimension& Redimension);
	inline int GetTool() const;	
	BOOL SetTool(int Tool);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL SelectNextElement(void);
	BOOL SelectPrevElement(void);
	BOOL OnTimer(UINT);
	/*
		Tool Operators
		*/
	BOOL GetShowContents(void) const;	
private:
	/*
		Tool Properties
		*/
	CToolCervicales m_ToolCervicales;
private:	
	CPageSetupDialog PSDialog;	
	int CurrentST;	
	void OnUpdateSTLine(CCmdUI*);
	void OnSTLine();	
	void OnUpdateSTNode(CCmdUI*);
	void OnSTNode();	
	void OnSTVector();
	void OnUpdateSTVector(CCmdUI*);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif	
public:
	//{{AFX_MSG(CRxDoc)
    afx_msg void OnEditClearAll();
    afx_msg void OnUpdateEditClearAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditOSize(CCmdUI * pCmdUI);
	afx_msg void OnUpdateEditOZero(CCmdUI * pCmdUI);
	afx_msg void OnEditOSize(void);
	afx_msg void OnEditOZero(void);
	afx_msg void OnDtoolSym();
	afx_msg void OnUpdateDtoolSym(CCmdUI* pCmdUI);
	afx_msg void OnFilePageSetup();
	afx_msg void OnUpdateFilePageSetup(CCmdUI* pCmdUI);
	afx_msg void OnDtoolText();
	afx_msg void OnUpdateDtoolText(CCmdUI* pCmdUI);
	afx_msg void OnDtoolFont();
	afx_msg void OnUpdateDtoolFont(CCmdUI* pCmdUI);
	afx_msg void OnDtoolDistance();
	afx_msg void OnUpdateDtoolDistance(CCmdUI* pCmdUI);
	afx_msg void OnDtoolMdist();
	afx_msg void OnUpdateDtoolMdist(CCmdUI* pCmdUI);
	afx_msg void OnToolsAngletracerDefineextension();
	afx_msg void OnUpdateToolsAngletracerDefineextension(CCmdUI* pCmdUI);
	afx_msg void OnToolsAngletracerDefineflexion();
	afx_msg void OnUpdateToolsAngletracerDefineflexion(CCmdUI* pCmdUI);
	afx_msg void OnToolsAngletracerTracefront();
	afx_msg void OnUpdateToolsAngletracerTracefront(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDtoolColors(CCmdUI* pCmdUI);
	afx_msg void OnDtoolRotate();
	afx_msg void OnUpdateDtoolRotate(CCmdUI* pCmdUI);
	afx_msg void OnDtoolAlpha();
	afx_msg void OnUpdateDtoolAlpha(CCmdUI* pCmdUI);
	afx_msg void OnOptionShowangles();
	afx_msg void OnUpdateOptionShowangles(CCmdUI* pCmdUI);
	afx_msg void OnOptionShowrot();
	afx_msg void OnUpdateOptionShowrot(CCmdUI* pCmdUI);
	afx_msg void OnImagesModeMonochrome();
	afx_msg void OnUpdateImagesModeMonochrome(CCmdUI* pCmdUI);
	afx_msg void OnImagesModeGrayscale();
	afx_msg void OnUpdateImagesModeGrayscale(CCmdUI* pCmdUI);
	afx_msg void OnImagesModeLogarithmicgrayscale();
	afx_msg void OnUpdateImagesModeLogarithmicgrayscale(CCmdUI* pCmdUI);
	afx_msg void OnImagesModeFloydsteinberg();
	afx_msg void OnUpdateImagesModeFloydsteinberg(CCmdUI* pCmdUI);
	afx_msg void OnImagesTransformEqualize();
	afx_msg void OnUpdateImagesTransformEqualize(CCmdUI* pCmdUI);
	afx_msg void OnImagesInformation();
	afx_msg void OnUpdateImagesInformation(CCmdUI* pCmdUI);
	afx_msg void OnImagesTransformInvert();
	afx_msg void OnUpdateImagesTransformInvert(CCmdUI* pCmdUI);
	afx_msg void OnImagesTransformBlur();
	afx_msg void OnUpdateImagesTransformBlur(CCmdUI* pCmdUI);
	afx_msg void OnImagesTransformBlurmore();
	afx_msg void OnUpdateImagesTransformBlurmore(CCmdUI* pCmdUI);
	afx_msg void OnImagesTransformSharpen();
	afx_msg void OnUpdateImagesTransformSharpen(CCmdUI* pCmdUI);
	afx_msg void OnImagesTransformSobeledgedetect();
	afx_msg void OnUpdateImagesTransformSobeledgedetect(CCmdUI* pCmdUI);
	afx_msg void OnImagesTransformMedianfilter();
	afx_msg void OnUpdateImagesTransformMedianfilter(CCmdUI* pCmdUI);
	afx_msg void OnImagesAdjustconvolveThin();
	afx_msg void OnUpdateImagesAdjustconvolveThin(CCmdUI* pCmdUI);
	afx_msg void OnImagesAdjustconvolveRobertsedgedetect();
	afx_msg void OnUpdateImagesAdjustconvolveRobertsedgedetect(CCmdUI* pCmdUI);
	afx_msg void OnImagesAdjustconvolveLaplacianedgedetect();
	afx_msg void OnUpdateImagesAdjustconvolveLaplacianedgedetect(CCmdUI* pCmdUI);
	afx_msg void OnImagesSizerotateFliphorizontal();
	afx_msg void OnUpdateImagesSizerotateFliphorizontal(CCmdUI* pCmdUI);
	afx_msg void OnImagesSizerotateFlipvertical();
	afx_msg void OnUpdateImagesSizerotateFlipvertical(CCmdUI* pCmdUI);
	afx_msg void OnMedicCervicalesangletracerAssistedtracer();
	afx_msg void OnUpdateMedicCervicalesangletracerAssistedtracer(CCmdUI* pCmdUI);
	afx_msg void OnMedicCervicalesangletracerDefineflexionvector();
	afx_msg void OnUpdateMedicCervicalesangletracerDefineflexionvector(CCmdUI* pCmdUI);
	afx_msg void OnMedicCervicalesangletracerDefineextensionvector();
	afx_msg void OnUpdateMedicCervicalesangletracerDefineextensionvector(CCmdUI* pCmdUI);
	afx_msg void OnDtoolErase();
	afx_msg void OnUpdateDtoolErase(CCmdUI* pCmdUI);
	afx_msg void OnImagesAdjustconvolveContour();
	afx_msg void OnUpdateImagesAdjustconvolveContour(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsDefinemetric(CCmdUI* pCmdUI);
	afx_msg void OnToolsDefinemetric();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline int CRxDoc::GetTool() const { 
	return CurrentST;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RXDOC_H__F481A56B_ABBA_11D2_9BFD_000001352962__INCLUDED_)
