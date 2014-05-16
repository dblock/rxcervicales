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
#include "ObjectManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CObjectManager::CObjectManager() {
	m_Selected = -1;
	m_Color = (COLORREF) 0;
	m_Rotating = FALSE;
}

CObjectManager::~CObjectManager() {

}

int CObjectManager::GetSelectedIndex(void) const {
	return m_Selected;
}

COLORREF CObjectManager::GetColor(void) const {
	return m_Color;
}

BOOL CObjectManager::SetColor(COLORREF newColor){
	if (m_Color != newColor){
		m_Color = newColor;
		return TRUE;
	} else return FALSE;
}

char * CObjectManager::GetCursor(void) const {
	return m_Cursor;
}

BOOL CObjectManager::SetCursor(char * newCursor) {
	if (newCursor != m_Cursor) {
		m_Cursor = newCursor;
		return TRUE;
	} else return FALSE;
}

BOOL CObjectManager::GetRotating(void) const {
	return m_Rotating;
}

BOOL CObjectManager::SetRotating(BOOL Value) {
	if (m_Rotating != Value) {
		m_Rotating = Value;
		return TRUE;
	} else return FALSE;
}

