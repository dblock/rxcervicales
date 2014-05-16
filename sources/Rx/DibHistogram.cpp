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
#include "DibHistogram.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "CDib.h"
#include <math.h>

CDibHistogram::CDibHistogram(void) {
	m_Dib = NULL;
}

CDibHistogram::CDibHistogram(CDib * i_Dib){
	m_Dib = NULL;
	SetDib(i_Dib);
}

CDibHistogram::~CDibHistogram() {

}

CDibHistogram::CDibHistogram(const CDibHistogram& Rhs) {
	(* this)=Rhs;
}

CDibHistogram& CDibHistogram::operator=(const CDibHistogram& Rhs){
	memcpy(m_Histogram, Rhs.m_Histogram, sizeof(char) * 256);
	memcpy(m_Cumulative, Rhs.m_Cumulative, sizeof(int) * 256);
	m_Average = Rhs.m_Average;
	m_Min = Rhs.m_Min;
	m_Max = Rhs.m_Max;
	m_Dib = Rhs.m_Dib;
	m_Integral = Rhs.m_Integral;
	return (* this);
}

BOOL CDibHistogram::SetDib(CDib * i_Dib) {
	if (i_Dib != m_Dib) {
		m_Dib = i_Dib;		
		return TRUE;
	} else return FALSE;
}

void CDibHistogram::BuildHistogram(void) {
	if (!m_Dib) return;
	memset(m_Histogram, 0, 256);
	m_Integral = 0;
	unsigned int l_Pix;
	m_Min = UINT_MAX;
	m_Max = 0;
	for (int x=0;x<m_Dib->m_lpBMIH->biWidth;x++)
		for (int y=0;y<m_Dib->m_lpBMIH->biHeight;y++) {
			l_Pix = m_Dib->GetGray(x, y);
			if (l_Pix > m_Max) m_Max = l_Pix;
			if (l_Pix < m_Min) m_Min = l_Pix;
			m_Integral += l_Pix;
			m_Histogram[l_Pix]++;			
		}
	if ((!m_Dib->m_lpBMIH->biHeight)||(!m_Dib->m_lpBMIH->biWidth)) {
		m_Average = 0;
	} else m_Average = m_Integral / (m_Dib->m_lpBMIH->biHeight * m_Dib->m_lpBMIH->biWidth);	
	m_Integral  = 0;
	double m_PDF = 0;
	int i;
	for (i=0;i<256;i++) {
		m_Integral +=m_Histogram[i];
		m_PDF += PDF(i);
		m_Cumulative[i] = m_PDF;
	}
	for (i=0;i<256;i++) m_Cumulative[i] /= m_Cumulative[255];	
}

int CDibHistogram::operator==(const CDibHistogram& Rhs) const {
	if (m_Dib == Rhs.m_Dib) {
		return 1;
	} else return 0;
}

double CDibHistogram::PDF(int Index) const {	
	return (((double) m_Histogram[Index]) / 256);
}

double CDibHistogram::GetMoment(int i_Deg) const {
	double l_Result = 0;
	for (int i=0;i<256;i++) {
		l_Result += pow(i, i_Deg) * PDF(i);
	}
	return l_Result;
}

double CDibHistogram::GetCenteredMoment(int i_Deg) const {
	double l_Result = 0;
	for (int i=0;i<256;i++) {
		l_Result += pow(i-m_Average, i_Deg) * PDF(i);
	}
	return l_Result;
}

double CDibHistogram::GetVariance(void) const {
	return GetCenteredMoment(2);
}

double CDibHistogram::GetSkewness(void) const {
	return GetCenteredMoment(3);
}

double CDibHistogram::GetKurtosis(void) const {
	return GetCenteredMoment(4);
}