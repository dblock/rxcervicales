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
#include "Rx.h"
#include "winspool.h"

#include "RxDoc.h"
#include "MetricDialog.h"
#include "MainFrm.h"
#include "DibInformation.h"
#include "RxView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CRxApp theApp;

IMPLEMENT_DYNCREATE(CRxDoc, CDocument)

BEGIN_MESSAGE_MAP(CRxDoc, CDocument)
	//{{AFX_MSG_MAP(CRxDoc)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, OnUpdateEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OSIZE, OnUpdateEditOSize)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OZERO, OnUpdateEditOZero)
	ON_COMMAND(ID_EDIT_OSIZE, OnEditOSize)
	ON_COMMAND(ID_EDIT_OZERO, OnEditOZero)
	ON_COMMAND(ID_DTOOL_SYM, OnDtoolSym)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_SYM, OnUpdateDtoolSym)	
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnFilePageSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_PAGE_SETUP, OnUpdateFilePageSetup)
	ON_COMMAND(ID_DTOOL_TEXT, OnDtoolText)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_TEXT, OnUpdateDtoolText)
	ON_COMMAND(ID_DTOOL_FONT, OnDtoolFont)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_FONT, OnUpdateDtoolFont)
	ON_COMMAND(ID_DTOOL_DISTANCE, OnDtoolDistance)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_DISTANCE, OnUpdateDtoolDistance)
	ON_COMMAND(ID_DTOOL_MDIST, OnDtoolMdist)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_MDIST, OnUpdateDtoolMdist)
	ON_COMMAND(ID_TOOLS_ANGLETRACER_DEFINEEXTENSION, OnToolsAngletracerDefineextension)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ANGLETRACER_DEFINEEXTENSION, OnUpdateToolsAngletracerDefineextension)
	ON_COMMAND(ID_TOOLS_ANGLETRACER_DEFINEFLEXION, OnToolsAngletracerDefineflexion)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ANGLETRACER_DEFINEFLEXION, OnUpdateToolsAngletracerDefineflexion)
	ON_COMMAND(ID_TOOLS_ANGLETRACER_TRACEFRONT, OnToolsAngletracerTracefront)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ANGLETRACER_TRACEFRONT, OnUpdateToolsAngletracerTracefront)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_COLORS, OnUpdateDtoolColors)
	ON_COMMAND(ID_DTOOL_ROTATE, OnDtoolRotate)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_ROTATE, OnUpdateDtoolRotate)
	ON_COMMAND(ID_DTOOL_ALPHA, OnDtoolAlpha)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_ALPHA, OnUpdateDtoolAlpha)
	ON_COMMAND(ID_OPTION_SHOWANGLES, OnOptionShowangles)
	ON_UPDATE_COMMAND_UI(ID_OPTION_SHOWANGLES, OnUpdateOptionShowangles)
	ON_COMMAND(ID_OPTION_SHOWROT, OnOptionShowrot)
	ON_UPDATE_COMMAND_UI(ID_OPTION_SHOWROT, OnUpdateOptionShowrot)
	ON_COMMAND(ID_IMAGES_MODE_MONOCHROME, OnImagesModeMonochrome)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_MODE_MONOCHROME, OnUpdateImagesModeMonochrome)
	ON_COMMAND(ID_IMAGES_MODE_GRAYSCALE, OnImagesModeGrayscale)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_MODE_GRAYSCALE, OnUpdateImagesModeGrayscale)
	ON_COMMAND(ID_IMAGES_MODE_LOGARITHMICGRAYSCALE, OnImagesModeLogarithmicgrayscale)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_MODE_LOGARITHMICGRAYSCALE, OnUpdateImagesModeLogarithmicgrayscale)
	ON_COMMAND(ID_IMAGES_MODE_FLOYDSTEINBERG, OnImagesModeFloydsteinberg)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_MODE_FLOYDSTEINBERG, OnUpdateImagesModeFloydsteinberg)
	ON_COMMAND(ID_IMAGES_TRANSFORM_EQUALIZE, OnImagesTransformEqualize)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_TRANSFORM_EQUALIZE, OnUpdateImagesTransformEqualize)
	ON_COMMAND(ID_IMAGES_INFORMATION, OnImagesInformation)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_INFORMATION, OnUpdateImagesInformation)
	ON_COMMAND(ID_IMAGES_TRANSFORM_INVERT, OnImagesTransformInvert)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_TRANSFORM_INVERT, OnUpdateImagesTransformInvert)
	ON_COMMAND(ID_IMAGES_TRANSFORM_BLUR, OnImagesTransformBlur)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_TRANSFORM_BLUR, OnUpdateImagesTransformBlur)
	ON_COMMAND(ID_IMAGES_TRANSFORM_BLURMORE, OnImagesTransformBlurmore)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_TRANSFORM_BLURMORE, OnUpdateImagesTransformBlurmore)
	ON_COMMAND(ID_IMAGES_TRANSFORM_SHARPEN, OnImagesTransformSharpen)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_TRANSFORM_SHARPEN, OnUpdateImagesTransformSharpen)
	ON_COMMAND(ID_IMAGES_TRANSFORM_SOBELEDGEDETECT, OnImagesTransformSobeledgedetect)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_TRANSFORM_SOBELEDGEDETECT, OnUpdateImagesTransformSobeledgedetect)
	ON_COMMAND(ID_IMAGES_TRANSFORM_MEDIANFILTER, OnImagesTransformMedianfilter)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_TRANSFORM_MEDIANFILTER, OnUpdateImagesTransformMedianfilter)
	ON_COMMAND(ID_IMAGES_ADJUSTCONVOLVE_THIN, OnImagesAdjustconvolveThin)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_ADJUSTCONVOLVE_THIN, OnUpdateImagesAdjustconvolveThin)
	ON_COMMAND(ID_IMAGES_ADJUSTCONVOLVE_ROBERTSEDGEDETECT, OnImagesAdjustconvolveRobertsedgedetect)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_ADJUSTCONVOLVE_ROBERTSEDGEDETECT, OnUpdateImagesAdjustconvolveRobertsedgedetect)
	ON_COMMAND(ID_IMAGES_ADJUSTCONVOLVE_LAPLACIANEDGEDETECT, OnImagesAdjustconvolveLaplacianedgedetect)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_ADJUSTCONVOLVE_LAPLACIANEDGEDETECT, OnUpdateImagesAdjustconvolveLaplacianedgedetect)
	ON_COMMAND(ID_IMAGES_SIZEROTATE_FLIPHORIZONTAL, OnImagesSizerotateFliphorizontal)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_SIZEROTATE_FLIPHORIZONTAL, OnUpdateImagesSizerotateFliphorizontal)
	ON_COMMAND(ID_IMAGES_SIZEROTATE_FLIPVERTICAL, OnImagesSizerotateFlipvertical)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_SIZEROTATE_FLIPVERTICAL, OnUpdateImagesSizerotateFlipvertical)
	ON_COMMAND(ID_MEDIC_CERVICALESANGLETRACER_ASSISTEDTRACER, OnMedicCervicalesangletracerAssistedtracer)
	ON_UPDATE_COMMAND_UI(ID_MEDIC_CERVICALESANGLETRACER_ASSISTEDTRACER, OnUpdateMedicCervicalesangletracerAssistedtracer)
	ON_COMMAND(ID_MEDIC_CERVICALESANGLETRACER_DEFINEFLEXIONVECTOR, OnMedicCervicalesangletracerDefineflexionvector)
	ON_UPDATE_COMMAND_UI(ID_MEDIC_CERVICALESANGLETRACER_DEFINEFLEXIONVECTOR, OnUpdateMedicCervicalesangletracerDefineflexionvector)
	ON_COMMAND(ID_MEDIC_CERVICALESANGLETRACER_DEFINEEXTENSIONVECTOR, OnMedicCervicalesangletracerDefineextensionvector)
	ON_UPDATE_COMMAND_UI(ID_MEDIC_CERVICALESANGLETRACER_DEFINEEXTENSIONVECTOR, OnUpdateMedicCervicalesangletracerDefineextensionvector)
	ON_COMMAND(ID_DTOOL_ERASE, OnDtoolErase)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_ERASE, OnUpdateDtoolErase)
	ON_COMMAND(ID_IMAGES_ADJUSTCONVOLVE_CONTOUR, OnImagesAdjustconvolveContour)
	ON_UPDATE_COMMAND_UI(ID_IMAGES_ADJUSTCONVOLVE_CONTOUR, OnUpdateImagesAdjustconvolveContour)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_DEFINEMETRIC, OnUpdateToolsDefinemetric)
	ON_COMMAND(ID_DTOOL_LINE, OnSTLine)	
	ON_UPDATE_COMMAND_UI(ID_DTOOL_LINE, OnUpdateSTLine)
	ON_COMMAND(ID_DTOOL_NODE, OnSTNode)	
	ON_COMMAND(ID_DTOOL_VECTOR, OnSTVector)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_NODE, OnUpdateSTNode)
	ON_UPDATE_COMMAND_UI(ID_DTOOL_VECTOR, OnUpdateSTVector)
	ON_COMMAND(ID_TOOLS_DEFINEMETRIC, OnToolsDefinemetric)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CRxDoc::CRxDoc() : PSDialog(PSD_DISABLEMARGINS | PSD_NOWARNING){	
	/* A4 */
	m_paperSize.cx = 826;
	m_paperSize.cy = 1168;
	m_Cursor = NULL;
	m_ToolCervicales.SetDocument(this);
}

CRxDoc::~CRxDoc(){

}

BOOL CRxDoc::OnNewDocument(){
	if (!CDocument::OnNewDocument()) return FALSE;
	m_ToolCervicales.RemoveAll();
	m_DibManager.RemoveAll();
	m_VectorManager.RemoveAll();
	m_AnnotationManager.RemoveAll();
	OnSTNode();
	m_VectorManager.SetColor(0);	
	m_AnnotationManager.SetColor(0);
	POSITION pos = GetFirstViewPosition();
    while (pos!=NULL) {
		GetNextView(pos)->Invalidate();
		POSITION pos = GetFirstViewPosition();
	}
	return TRUE;
}

void CRxDoc::Serialize(CArchive& ar){	
	m_DibManager.Serialize(ar);
	m_VectorManager.Serialize(ar);
	m_AnnotationManager.Serialize(ar);
	if (ar.IsLoading()){		
		OnSTNode();
		UpdateAllViews(NULL);
	}
}

void CRxDoc::OnEditClearSelected(){
	if (m_DibManager.DeleteSelected()){
		UpdateAllViews(NULL);
		SetModifiedFlag();
	}	
}

void CRxDoc::OnEditClearAll() {
    m_ToolCervicales.RemoveAll();
	m_DibManager.RemoveAll();
	m_VectorManager.RemoveAll();
	m_AnnotationManager.RemoveAll();
	UpdateAllViews(NULL);
    SetModifiedFlag();
}

void CRxDoc::Add(const CAVector& Elt){
	m_VectorManager.Add(Elt);
	SetModifiedFlag();	
}

void CRxDoc::Paint(CDC * memDC, const CRedimension& Redimension){
	m_VectorManager.Paint(memDC, Redimension);
	m_DibManager.Paint(memDC, Redimension);
	m_AnnotationManager.Paint(memDC, Redimension);
}

void CRxDoc::OnUpdateSTLine(CCmdUI* pCmdUI) {		
	pCmdUI->SetCheck(CurrentST == ID_DTOOL_LINE);
}

void CRxDoc::OnSTLine(){
	CurrentST = ID_DTOOL_LINE;
}

void CRxDoc::OnUpdateSTNode(CCmdUI* pCmdUI) {		
	pCmdUI->SetCheck(CurrentST == ID_DTOOL_NODE);
}

void CRxDoc::OnUpdateSTVector(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(CurrentST == ID_DTOOL_VECTOR);
}

void CRxDoc::OnSTNode(){
	CurrentST = ID_DTOOL_NODE;
}

void CRxDoc::OnSTVector(){
	CurrentST = ID_DTOOL_VECTOR;
}

#ifdef _DEBUG
void CRxDoc::AssertValid() const {
	CDocument::AssertValid();
}

void CRxDoc::Dump(CDumpContext& dc) const {
	CDocument::Dump(dc);
}
#endif 

BOOL CRxDoc::CanSelect(const CPoint& APoint){	
	return m_VectorManager.CanSelect(APoint);
}

BOOL CRxDoc::Select(const CPoint& APoint){
	if (GetTool() == ID_DTOOL_ROTATE) {
		if (m_DibManager.GetSelectedIndex() != -1) 
			m_DibManager.RotateSelect(APoint);
		else if (m_VectorManager.GetSelectedIndex() != -1)
			m_VectorManager.RotateSelect(APoint);
		return FALSE;
	} else {
		BOOL Result = FALSE;
		if (m_VectorManager.Select(APoint)) {
			m_DibManager.Deselect();
			m_AnnotationManager.Deselect();
			return TRUE;
		}
		if (m_DibManager.Select(APoint)) {
			m_AnnotationManager.Deselect();
			return TRUE;
		}	
		if (m_AnnotationManager.Select(APoint)) 
			return TRUE;
		return FALSE;
	}	
}

void CRxDoc::EditPaste(CClientDC& dc){
	if (m_DibManager.EditPaste()){
        m_VectorManager.Deselect();
		m_AnnotationManager.Deselect();
		OnSTNode();
		UsePalette(&dc);
		SetModifiedFlag();
		UpdateAllViews(NULL);	
	}
}

void CRxDoc::EditPastefrom(CClientDC& dc){
	if (m_DibManager.EditPastefrom()){		
		m_VectorManager.Deselect();
		m_AnnotationManager.Deselect();
		OnSTNode();
		m_DibManager.SetSystemPalette(&dc);
		m_DibManager.UsePalette(&dc);
		SetModifiedFlag();
		UpdateAllViews(NULL);
	} 
}

BOOL CRxDoc::Deselect(void){
	BOOL Result = m_VectorManager.Deselect();
	Result = Result || m_DibManager.Deselect();
	Result = Result || m_AnnotationManager.Deselect();
	return Result;
}

BOOL CRxDoc::DeselectVector(void){
	return m_VectorManager.Deselect();
}

BOOL CRxDoc::DeselectDib(void){
	return m_DibManager.Deselect();
}

BOOL CRxDoc::DeselectAnnotation(void){
	return m_AnnotationManager.Deselect();
}

void CRxDoc::MouseUp(const CPoint& APoint){
	if (GetTool() == ID_DTOOL_ROTATE) {
		m_VectorManager.SetRotating(FALSE);
		m_DibManager.SetRotating(FALSE);
	} else {
		if (m_VectorManager.EndMove()) 
			SetModifiedFlag();		
		m_DibManager.EndMove();
	}
}

BOOL CRxDoc::OnMouseMove(const CPoint& APoint){
	if (GetTool() == ID_DTOOL_NODE) {		
		if (m_VectorManager.OnMouseMove(APoint)){
			m_Cursor = m_VectorManager.GetCursor();
			return TRUE;
		} else if (m_DibManager.OnMouseMove(APoint)){
			m_Cursor = m_DibManager.GetCursor();
			return TRUE;
		} else if (m_AnnotationManager.OnMouseMove(APoint)){
			m_Cursor = m_AnnotationManager.GetCursor();
			return TRUE;
		} else return FALSE;
	} else return FALSE;
}

BOOL CRxDoc::MouseMove(const CPoint& APoint){
	if (GetTool() == ID_DTOOL_ERASE) {
		return m_DibManager.MouseMoveErase(APoint);
	} else if (GetTool() == ID_DTOOL_ROTATE) {
		if (m_DibManager.GetSelectedIndex() != -1) 
			return m_DibManager.Rotate(APoint);
		else if (m_VectorManager.GetSelectedIndex() != -1)
			return m_VectorManager.Rotate(APoint);
		return FALSE;
	} else {
		if (m_DibManager.MouseMove(APoint)) {
			m_Cursor = m_DibManager.GetCursor();
			SetModifiedFlag();
			return TRUE;
		} else if (m_VectorManager.MouseMove(APoint)){
			m_Cursor = m_VectorManager.GetCursor();
			return TRUE;
		} else if (m_AnnotationManager.MouseMove(APoint)){
			m_Cursor = m_AnnotationManager.GetCursor();
			return TRUE;		
		} else return FALSE;
	}
}

CRect CRxDoc::GetBoundMove() {		
	CRect Rect = m_VectorManager.GetBoundMove();
	Rect |= m_DibManager.GetBoundMove();
	Rect |= m_AnnotationManager.GetBoundMove();
	return Rect;
}

COLORREF CRxDoc::GetColor() const { 
	return m_VectorManager.GetColor();
}

BOOL CRxDoc::SetColor(COLORREF newColor) { 
	BOOL Result = m_AnnotationManager.SetColor(newColor);
	Result |= m_VectorManager.SetColor(newColor);
	return Result;
}

BOOL CRxDoc::DeleteSelected(void){
	m_ToolCervicales.OnRemoveDIB(m_DibManager.GetSelected());
	m_ToolCervicales.OnRemoveVector(m_VectorManager.GetSelected());
	if (m_DibManager.DeleteSelected() || 
		m_VectorManager.DeleteSelected() ||
		m_AnnotationManager.DeleteSelected()){
		SetModifiedFlag();
		return TRUE;
	} else return FALSE;
	
}

BOOL CRxDoc::SetSelectedColor(COLORREF newColor){
	BOOL Result = m_AnnotationManager.SetSelectedColor(newColor);
	Result |= m_VectorManager.SetSelectedColor(newColor);
	return Result;
}

void CRxDoc::OnUpdateEditClearAll(CCmdUI* pCmdUI) {
    pCmdUI->Enable((m_DibManager.GetCount() +
		m_VectorManager.GetCount() +
		m_AnnotationManager.GetCount())>0);	
}

void CRxDoc::Add(const CAnnotation& Annotation){
	m_AnnotationManager.Deselect();
	m_AnnotationManager.Add(Annotation);
	SetModifiedFlag();
}

void CRxDoc::Add(const CDib& ADib){		
	m_DibManager.Add(ADib);	
	SetModifiedFlag();    
}

void CRxDoc::OnViewPaletteChanged(CClientDC& dc, UINT wParam){
	m_DibManager.ViewPaletteChanged(dc, wParam);
}


UINT CRxDoc::UsePalette(CDC* pDC, BOOL bBackground){
	m_DibManager.UsePalette(pDC, bBackground);
	return 0;
}

COleDataSource* CRxDoc::SaveDib(void){	
	return m_DibManager.SaveDibSelected();    
}

void CRxDoc::CopyDibTo(void){
	m_DibManager.CopyDibSelectedTo();
}

int CRxDoc::GetSelectedDIBIndex(void) const {
	return m_DibManager.GetSelectedIndex();
}

void CRxDoc::OnUpdateEditOSize(CCmdUI* pCmdUI){
	pCmdUI->Enable((m_DibManager.GetSelectedIndex() != -1)&&((m_DibManager.GetSelected()->GetXScale() != 1)||(m_DibManager.GetSelected()->GetYScale() != 1)));	
}

void CRxDoc::OnEditOSize(void){
	if (m_DibManager.EditOSize()){
		SetModifiedFlag();
		UpdateAllViews(NULL);		
	}
}

void CRxDoc::OnUpdateEditOZero(CCmdUI* pCmdUI){
	pCmdUI->Enable((m_DibManager.GetSelectedIndex() != -1)&&(m_DibManager.GetSelected()->GetAngle()));	
}

void CRxDoc::OnEditOZero(void){
	if (m_DibManager.EditOZero()){
		SetModifiedFlag();
		UpdateAllViews(NULL);		
	}
}


void CRxDoc::DeleteContents() {
	m_ToolCervicales.RemoveAll();
	m_DibManager.RemoveAll();
	m_VectorManager.RemoveAll();
	m_AnnotationManager.RemoveAll();
	OnSTNode();
	m_VectorManager.SetColor(0);
	CDocument::DeleteContents();
}

void CRxDoc::OnDtoolSym() {
	CurrentST = ID_DTOOL_SYM;	
}

void CRxDoc::OnUpdateDtoolSym(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(CurrentST == ID_DTOOL_SYM);	
}

void CRxDoc::OnFilePageSetup() {	
	if (PSDialog.DoModal() == IDOK){
		m_paperSize.cx = PSDialog.GetPaperSize().cx / 10;
		m_paperSize.cy = PSDialog.GetPaperSize().cy / 10;	
		if ((m_paperSize.cx > 2000)) {
			m_paperSize.cx = (int) ((float) m_paperSize.cx / 2.5);
			m_paperSize.cy = (int) ((float) m_paperSize.cy / 2.5);
		}
		UpdateAllViews(NULL);
	}
}

void CRxDoc::OnUpdateFilePageSetup(CCmdUI* pCmdUI) {
	pCmdUI->Enable();	
}

void CRxDoc::OnDtoolText() {
	CurrentST = ID_DTOOL_TEXT;
}

void CRxDoc::OnUpdateDtoolText(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(CurrentST == ID_DTOOL_TEXT);	
}

void CRxDoc::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags){
	switch(CurrentST) {
	case ID_DTOOL_NODE:
		if (m_AnnotationManager.GetSelectedIndex() != -1) {
			OnDtoolText();
			m_AnnotationManager.OnChar(nChar, nRepCnt, nFlags);
		}
		break;
	case ID_DTOOL_TEXT:
		m_AnnotationManager.OnChar(nChar, nRepCnt, nFlags);
		break;
	}
}

void CRxDoc::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	/*
	if (GetKeyState(VK_CONTROL) & 128 > 0) {
		switch(nChar){
		case 37:			
			break;
		case 39:
			break;
		case 38:
			break;
		case 40:
			break;
		}
	} else {
	*/
	switch(CurrentST) {
	case ID_DTOOL_NODE:
		if (m_AnnotationManager.GetSelectedIndex() != -1) {
			OnDtoolText();
			m_AnnotationManager.OnKeyDown(nChar, nRepCnt, nFlags);
		}
		break;
	case ID_DTOOL_TEXT:
		m_AnnotationManager.OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}	
}

void CRxDoc::OnDtoolFont() {
	CAnnotation * CurrentAnnotation = m_AnnotationManager.GetSelected();
	if (CurrentAnnotation) {
		LOGFONT LGFDialog;
		LGFDialog.lfHeight = -MulDiv(CurrentAnnotation->GetFontSize(), 
			GetDeviceCaps(::GetDC(NULL), LOGPIXELSY), 72); 
		LGFDialog.lfWidth = 0;
		LGFDialog.lfEscapement = 0;
		LGFDialog.lfOrientation = 0;
		LGFDialog.lfWeight = (CurrentAnnotation->GetBold())?FW_BOLD:FW_REGULAR;
		LGFDialog.lfItalic = CurrentAnnotation->GetItalic();
		LGFDialog.lfUnderline = CurrentAnnotation->GetUnderline();
		LGFDialog.lfStrikeOut = CurrentAnnotation->GetStrikeOut();
		LGFDialog.lfCharSet = DEFAULT_CHARSET;
		LGFDialog.lfOutPrecision = OUT_DEFAULT_PRECIS;
		LGFDialog.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		LGFDialog.lfQuality = DEFAULT_QUALITY;
		LGFDialog.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		memcpy(LGFDialog.lfFaceName, CurrentAnnotation->GetFontName(), min(CurrentAnnotation->GetFontName().GetLength(),LF_FACESIZE));
		LGFDialog.lfFaceName[min(CurrentAnnotation->GetFontName().GetLength(),LF_FACESIZE)] = 0;
		CFontDialog FNTDialog(&LGFDialog, CF_NOSCRIPTSEL | CF_EFFECTS | CF_SCREENFONTS);
		if (FNTDialog.DoModal() == IDOK) {
			if (m_AnnotationManager.SetFontFromDialog(FNTDialog))
				UpdateAllViews(NULL);
		}
	}
}

void CRxDoc::OnUpdateDtoolFont(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_AnnotationManager.GetSelectedIndex() != -1);	
}

BOOL CRxDoc::SelectNextElement(void) {
	if (m_DibManager.GetSelectedIndex() != -1) {
		if (!m_DibManager.SelectNextElement())
			if (!m_VectorManager.SelectFirstElement())
				if (!m_AnnotationManager.SelectFirstElement())
					m_DibManager.SelectFirstElement();
	} else if (m_VectorManager.GetSelectedIndex() != -1){
		if (!m_VectorManager.SelectNextElement())
			if (!m_AnnotationManager.SelectFirstElement())
				if (!m_DibManager.SelectFirstElement())
					m_VectorManager.SelectFirstElement();
	} else if (m_AnnotationManager.GetSelectedIndex() != -1) {
		if (!m_AnnotationManager.SelectNextElement())
			if (!m_DibManager.SelectFirstElement())
				if (!m_VectorManager.SelectFirstElement())
					m_AnnotationManager.SelectFirstElement();
	} else if (!m_DibManager.SelectFirstElement()) 
		if (!m_VectorManager.SelectFirstElement()) 
			if (!m_AnnotationManager.SelectFirstElement()) 
				return FALSE;
	OnSTNode();
	return TRUE;
}

BOOL CRxDoc::SelectPrevElement(void) {
	if (m_AnnotationManager.GetSelectedIndex() != -1) {
		if (!m_AnnotationManager.SelectPrevElement())
			if (!m_VectorManager.SelectLastElement())
				if (!m_DibManager.SelectLastElement())				
					m_AnnotationManager.SelectLastElement();
	} else if (m_VectorManager.GetSelectedIndex() != -1){
		if (!m_VectorManager.SelectPrevElement())
			if (!m_DibManager.SelectLastElement())
				if (!m_AnnotationManager.SelectLastElement())				
					m_VectorManager.SelectLastElement();	
	} else if (m_DibManager.GetSelectedIndex() != -1) {
		if (!m_DibManager.SelectPrevElement())
			if (!m_AnnotationManager.SelectLastElement())
				if (!m_VectorManager.SelectLastElement())				
					m_DibManager.SelectLastElement();	
	} else if (!m_AnnotationManager.SelectLastElement()) 
		if (!m_VectorManager.SelectLastElement()) 
			if (!m_DibManager.SelectLastElement()) 			
				return FALSE;
	OnSTNode();
	return TRUE;
}

void CRxDoc::OnDtoolDistance() {
	CurrentST = ID_DTOOL_DISTANCE;	
}

void CRxDoc::OnUpdateDtoolDistance(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(CurrentST == ID_DTOOL_DISTANCE);
}

void CRxDoc::OnDtoolMdist() {
	CurrentST = ID_DTOOL_MDIST;		
}

void CRxDoc::OnUpdateDtoolMdist(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(CurrentST == ID_DTOOL_MDIST);	
}

void CRxDoc::OnToolsAngletracerDefineextension() {
	if (m_ToolCervicales.GetExtension() && (m_ToolCervicales.GetExtension() == m_DibManager.GetSelected())) {
		m_ToolCervicales.SetExtension(NULL);
		UpdateAllViews(NULL);
	} else if (m_ToolCervicales.SetExtension(m_DibManager.GetSelected()))
		UpdateAllViews(NULL);
}

void CRxDoc::OnUpdateToolsAngletracerDefineextension(CCmdUI* pCmdUI) {
	if (m_ToolCervicales.GetExtension() && (m_ToolCervicales.GetExtension() == m_DibManager.GetSelected())) {
		CString m_Text = "Undefine &Extension";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_DibManager.GetSelectedIndex() != -1);	
	} else {
		CString m_Text = "Define &Extension";
		if (m_ToolCervicales.GetExtension()) m_Text += " (set)";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_DibManager.GetSelectedIndex() != -1);	
	}
}

void CRxDoc::OnToolsAngletracerDefineflexion() {
	if (m_ToolCervicales.GetFlexion() && (m_ToolCervicales.GetFlexion() == m_DibManager.GetSelected())) {
		m_ToolCervicales.SetFlexion(NULL);
		UpdateAllViews(NULL);
	} else if (m_ToolCervicales.SetFlexion(m_DibManager.GetSelected()))
		UpdateAllViews(NULL);
}

void CRxDoc::OnUpdateToolsAngletracerDefineflexion(CCmdUI* pCmdUI) {
	if (m_ToolCervicales.GetFlexion() && (m_ToolCervicales.GetFlexion() == m_DibManager.GetSelected())) {
		CString m_Text = "Undefine &Flexion";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_DibManager.GetSelectedIndex() != -1);	
	} else {
		CString m_Text = "Define &Flexion";
		if (m_ToolCervicales.GetFlexion()) m_Text = m_Text + " (set)";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_DibManager.GetSelectedIndex() != -1);	
	}
}

void CRxDoc::OnToolsAngletracerTracefront() {
	BeginWaitCursor();
	m_ToolCervicales.Trace(this);
	UpdateAllViews(NULL);
	EndWaitCursor();
}

void CRxDoc::OnUpdateToolsAngletracerTracefront(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_ToolCervicales.IsTracable());
}

void CRxDoc::OnUpdateDtoolColors(CCmdUI* pCmdUI) {
	//pCmdUI->Enable(m_VectorManager.GetSelectedIndex() != -1);
	pCmdUI->Enable();		
}

BOOL CRxDoc::SetTool(int Tool) {
	if (Tool != CurrentST) {
		CurrentST = Tool;
		return TRUE;
	} else return FALSE;
}

void CRxDoc::OnDtoolRotate() {
	SetTool(ID_DTOOL_ROTATE);
}

void CRxDoc::OnUpdateDtoolRotate(CCmdUI* pCmdUI) {
	pCmdUI->Enable((GetSelectedDIBIndex() != -1)||(m_VectorManager.GetSelectedIndex() != -1));
	pCmdUI->SetCheck(GetTool() == ID_DTOOL_ROTATE);
}

void CRxDoc::OnDtoolAlpha() {
	SetTool(ID_DTOOL_ALPHA);
}

void CRxDoc::OnUpdateDtoolAlpha(CCmdUI* pCmdUI) {
	pCmdUI->Enable();
	pCmdUI->SetCheck(GetTool() == ID_DTOOL_ALPHA);
}

void CRxDoc::OnOptionShowangles() {
	m_VectorManager.SetShowAngles(!m_VectorManager.GetShowAngles());
	UpdateAllViews(NULL);
}

void CRxDoc::OnUpdateOptionShowangles(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_VectorManager.GetShowAngles());		
}

void CRxDoc::OnOptionShowrot() {
	m_DibManager.SetShowContents(!m_DibManager.GetShowContents());	
}

void CRxDoc::OnUpdateOptionShowrot(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_DibManager.GetShowContents());			
}

BOOL CRxDoc::GetShowContents(void) const {
	return m_DibManager.GetShowContents();
}

BOOL CRxDoc::OnTimer(UINT nIDEvent) {
	return m_DibManager.OnTimer(nIDEvent);
}

void CRxDoc::OnImagesModeMonochrome() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->CnvMonochrome();
	UpdateAllViews(NULL);
	EndWaitCursor();
}

void CRxDoc::OnUpdateImagesModeMonochrome(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() > 1)
	);
}

void CRxDoc::OnImagesModeGrayscale() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->CnvGrayLevel(FALSE);
	UpdateAllViews(NULL);	
	EndWaitCursor();
}

void CRxDoc::OnUpdateImagesModeGrayscale(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() >= 4)
	);	
}

void CRxDoc::OnImagesModeLogarithmicgrayscale() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->CnvGrayLevel(TRUE);
	UpdateAllViews(NULL);	
	EndWaitCursor();
}

void CRxDoc::OnUpdateImagesModeLogarithmicgrayscale(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() >= 4)
	);
}

void CRxDoc::OnImagesModeFloydsteinberg() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->FloydSteinberg();
	UpdateAllViews(NULL);
	EndWaitCursor();
}

void CRxDoc::OnUpdateImagesModeFloydsteinberg(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() >= 4)
	);
}

CDib * CRxDoc::GetSelectedDIB(void) {
	return m_DibManager.GetSelected();
}

void CRxDoc::OnImagesTransformEqualize() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->EqualizeHistogram();	
	UpdateAllViews(NULL);
	EndWaitCursor();
}

void CRxDoc::OnUpdateImagesTransformEqualize(CCmdUI* pCmdUI) {
	/*
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() > 1)
	);
	*/
	pCmdUI->Enable(FALSE);
}

void CRxDoc::OnImagesInformation() {
	((CMainFrame *) theApp.m_pMainWnd)->ProcessView(this, theApp.p_InfoTemplate, RUNTIME_CLASS(CDibInformation), frmDestroy | frmCreate);
}

void CRxDoc::OnUpdateImagesInformation(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(((CMainFrame *) theApp.m_pMainWnd)->FindView(this, theApp.p_InfoTemplate, RUNTIME_CLASS(CDibInformation))!=NULL);		
}


BOOL CRxDoc::CanCloseFrame(CFrameWnd* pFrame) {	
	int v_Cnt = 0, i_Cnt = 0;
	CView* pView; POSITION pos = GetFirstViewPosition();
	while (pos != NULL)	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CRxView))) v_Cnt++;
		else if (pView->IsKindOf(RUNTIME_CLASS(CDibInformation))) i_Cnt++;
	}
	CView * a_View = pFrame->GetActiveView();
	if (a_View->IsKindOf(RUNTIME_CLASS(CDibInformation))) {
		((CMainFrame *) theApp.m_pMainWnd)->ProcessView(this, theApp.p_InfoTemplate, RUNTIME_CLASS(CDibInformation), frmDestroy);
		return FALSE;
	} else {
		while (((CMainFrame *) theApp.m_pMainWnd)->ProcessView(this, theApp.p_InfoTemplate, RUNTIME_CLASS(CDibInformation), frmDestroy) != NULL);
	}
	if (!CDocument::CanCloseFrame(pFrame)) {
		if (i_Cnt) ((CMainFrame *) theApp.m_pMainWnd)->ProcessView(this, theApp.p_InfoTemplate, RUNTIME_CLASS(CDibInformation), frmCreate);
		return FALSE;
	} else return TRUE;	
}

void CRxDoc::OnImagesTransformInvert() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->Invert();
	UpdateAllViews(NULL);	
	EndWaitCursor();
}

void CRxDoc::OnUpdateImagesTransformInvert(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1)		
	);
}

void CRxDoc::OnImagesTransformBlur() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->Blur();
	UpdateAllViews(NULL);	
	EndWaitCursor();	
}

void CRxDoc::OnUpdateImagesTransformBlur(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() > 1)
	);
}

void CRxDoc::OnImagesTransformBlurmore() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->BlurMore();
	UpdateAllViews(NULL);	
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesTransformBlurmore(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() > 1)
	);
}

void CRxDoc::OnImagesTransformSharpen() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->Sharpen();
	UpdateAllViews(NULL);	
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesTransformSharpen(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() > 1)
	);
}

void CRxDoc::OnImagesTransformSobeledgedetect() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->SobelEdgeDetect(FALSE, 20);
	UpdateAllViews(NULL);	
	EndWaitCursor();		
		
}

void CRxDoc::OnUpdateImagesTransformSobeledgedetect(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1)
	);
}

void CRxDoc::OnImagesTransformMedianfilter() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->MedianFilter(3, 3);
	UpdateAllViews(NULL);	
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesTransformMedianfilter(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1) &&
		(m_DibManager.GetSelected()->GetDepth() > 1)
	);
}

void CRxDoc::OnImagesAdjustconvolveThin() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->Thin();
	UpdateAllViews(NULL);
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesAdjustconvolveThin(CCmdUI* pCmdUI) {	
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1)
	);
}

void CRxDoc::OnImagesAdjustconvolveRobertsedgedetect() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->RobertsEdgeDetect();
	UpdateAllViews(NULL);	
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesAdjustconvolveRobertsedgedetect(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1)
	);	
}

void CRxDoc::OnImagesAdjustconvolveLaplacianedgedetect() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->LaplacianEdgeDetect(1);
	UpdateAllViews(NULL);	
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesAdjustconvolveLaplacianedgedetect(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1)
	);	
}

void CRxDoc::OnImagesSizerotateFliphorizontal() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->Flip(TRUE, FALSE);
	UpdateAllViews(NULL);	
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesSizerotateFliphorizontal(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1)
	);	
}

void CRxDoc::OnImagesSizerotateFlipvertical() {
	BeginWaitCursor();
	m_DibManager.GetSelected()->Flip(FALSE, TRUE);
	UpdateAllViews(NULL);	
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesSizerotateFlipvertical(CCmdUI* pCmdUI) {
	pCmdUI->Enable(
		(m_DibManager.GetSelectedIndex() != -1)
	);	
}

void CRxDoc::OnMedicCervicalesangletracerAssistedtracer() {
	m_ToolCervicales.AssistTrace();
}

void CRxDoc::OnUpdateMedicCervicalesangletracerAssistedtracer(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_ToolCervicales.IsAssistable());
}

void CRxDoc::OnMedicCervicalesangletracerDefineflexionvector() {
	if (m_ToolCervicales.GetExtensionVector() && (m_ToolCervicales.GetExtensionVector() == m_VectorManager.GetSelected())) {
		m_ToolCervicales.SetExtensionVector(NULL);
		UpdateAllViews(NULL);
	} else if (m_ToolCervicales.SetExtensionVector(m_VectorManager.GetSelected()))
		UpdateAllViews(NULL);	
}

void CRxDoc::OnUpdateMedicCervicalesangletracerDefineflexionvector(CCmdUI* pCmdUI) {
	if (m_ToolCervicales.GetExtensionVector() && (m_ToolCervicales.GetExtensionVector() == m_VectorManager.GetSelected())) {
		CString m_Text = "Undefine E&xtension Vector";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_VectorManager.GetSelectedIndex() != -1);	
	} else {
		CString m_Text = "Define E&xtension Vector";
		if (m_ToolCervicales.GetExtensionVector()) m_Text += " (set)";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_VectorManager.GetSelectedIndex() != -1);	
	}
}

void CRxDoc::OnMedicCervicalesangletracerDefineextensionvector() {
	if (m_ToolCervicales.GetFlexionVector() && (m_ToolCervicales.GetFlexionVector() == m_VectorManager.GetSelected())) {
		m_ToolCervicales.SetFlexionVector(NULL);
		UpdateAllViews(NULL);
	} else if (m_ToolCervicales.SetFlexionVector(m_VectorManager.GetSelected()))
		UpdateAllViews(NULL);		
}

void CRxDoc::OnUpdateMedicCervicalesangletracerDefineextensionvector(CCmdUI* pCmdUI) {
	if (m_ToolCervicales.GetFlexionVector() && (m_ToolCervicales.GetFlexionVector() == m_VectorManager.GetSelected())) {
		CString m_Text = "Undefine F&lexion Vector";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_VectorManager.GetSelectedIndex() != -1);	
	} else {
		CString m_Text = "Define F&lexion Vector";
		if (m_ToolCervicales.GetFlexionVector()) m_Text += " (set)";
		pCmdUI->SetText(m_Text);
		pCmdUI->Enable(m_VectorManager.GetSelectedIndex() != -1);	
	}
}

void CRxDoc::OnDtoolErase() {
	//SetTool(ID_DTOOL_ERASE);
}

void CRxDoc::OnUpdateDtoolErase(CCmdUI* pCmdUI) {
	//pCmdUI->Enable(GetSelectedDIBIndex() != -1);
	//pCmdUI->SetCheck(CurrentST == ID_DTOOL_ERASE);
	pCmdUI->Enable(FALSE);
}

void CRxDoc::OnImagesAdjustconvolveContour() {
	BeginWaitCursor();
	theApp.ShowProgress("Segmentating, please wait.");
	CDib m_Dib2; m_Dib2 = * m_DibManager.GetSelected();
	m_Dib2.MedianFilter(2, 2);
	m_Dib2.CnvMonochrome();	
	m_ToolCervicales.TraceContour(&m_Dib2, this);
	UpdateAllViews(NULL);
	theApp.HideProgress();
	EndWaitCursor();		
}

void CRxDoc::OnUpdateImagesAdjustconvolveContour(CCmdUI* pCmdUI) {
	pCmdUI->Enable(GetSelectedDIBIndex() != -1);
}

CAVector * CRxDoc::GetSelectedVector(void) {
	return m_VectorManager.GetSelected();
}

CAVector * CRxDoc::GetTailVector(void) {
	return m_VectorManager.GetTail();
}

void CRxDoc::OnUpdateToolsDefinemetric(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_VectorManager.GetSelectedIndex() != -1);
}

void CRxDoc::OnToolsDefinemetric() {
	CMetricDialog l_MetricDialog;
	l_MetricDialog.SetDistance(m_VectorManager.GetSelected()->GetDistance());
	l_MetricDialog.SetMetric(m_VectorManager.GetMetric());
	if (l_MetricDialog.DoModal() == IDOK) {
		m_VectorManager.SetMetric(l_MetricDialog.GetMetric());
		UpdateAllViews(NULL);
	}	
}
