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
#include "VectorObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CVectorObject::CVectorObject() {
	m_Selected = FALSE;
	m_Color = (COLORREF) 0;	
}

CVectorObject::~CVectorObject(){

}

CVectorObject::CVectorObject(const CVectorObject& Rhs) {
	(* this) = Rhs;
}

CVectorObject& CVectorObject::operator=(const CVectorObject& Rhs){	
	m_Bound = Rhs.m_Bound;
	m_Selected = Rhs.m_Selected;
	m_Color = Rhs.m_Color;
	m_SelPoint = Rhs.m_SelPoint;	
	return (* this);
}

BOOL CVectorObject::GetSelected(void) const {
	return m_Selected;
}

BOOL CVectorObject::Select(BOOL Value) {	
	if (m_Selected != Value){
		m_Selected = Value;				
		return TRUE;
	} else return FALSE;	
}

CRect CVectorObject::Bound(){
	return m_Bound;
}
