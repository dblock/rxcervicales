/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef _C_DIB_HISTOGRAM
#define _C_DIB_HISTOGRAM

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDib;

class CDibHistogram {
private:
	CDib * m_Dib;
	unsigned int m_Average; // average value
	unsigned int m_Max; // maximum value
	unsigned int m_Min; // minimum value
	unsigned char m_Histogram[256]; // histogram values
	double m_Cumulative[256]; // cumulative law
	unsigned long m_Integral; // surface of histogram	
public:
	/*
		constructors
		*/
	CDibHistogram();
	virtual ~CDibHistogram();
	CDibHistogram(CDib * i_Dib);
	CDibHistogram(const CDibHistogram&);
	CDibHistogram& operator=(const CDibHistogram&);
	/*
		operators
		*/
	int operator==(const CDibHistogram&) const;
	double PDF(int) const; // probability density function
	/*
		DIB
		*/
	BOOL SetDib(CDib * i_Dib);
	inline CDib * GetDib(void) const { return m_Dib; };	
	void BuildHistogram(void);
	/*
		Query
		*/
	inline unsigned int GetAverageLevel(void) const { return m_Average; }
	inline unsigned int GetMinLevel(void) const { return m_Min; }
	inline unsigned int GetMaxLevel(void) const { return m_Max; }
	inline unsigned int GetHistogramAt(int Index) const { return m_Histogram[Index]; }
	inline unsigned long GetHistogramIntegral(void) const { return m_Integral; }	
	inline unsigned int operator[](int Index) const { return m_Histogram[Index]; }
	inline double GetCumulativeAt(int Index) const { return m_Cumulative[Index];}
	double GetMoment(int i_Deg) const; // first order moment of degree n
	double GetCenteredMoment(int i_Deg) const; // first order centered moment of degree n
	double GetVariance(void) const; // image variance (cm O(1) deg(2))
	double GetSkewness(void) const; // skewness
	double GetKurtosis(void) const; // kurtosis
};

#endif 

