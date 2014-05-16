/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_VECTOR_OBJECT_MANAGER_H
#define C_VECTOR_OBJECT_MANAGER_H

class CView;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CObjectManager {
protected:
	int m_Selected;
	COLORREF m_Color;
	char * m_Cursor;
	BOOL m_Rotating;	
public:
	CObjectManager();
	CObjectManager(const CObjectManager&);
	virtual ~CObjectManager();
	/* properties */
	int GetSelectedIndex(void) const;
	BOOL SetColor(COLORREF);
	COLORREF GetColor(void) const;
	char * GetCursor(void) const;
	BOOL SetCursor(char *);
	BOOL GetRotating(void) const;
	BOOL SetRotating(BOOL Value = TRUE);
};

#endif
