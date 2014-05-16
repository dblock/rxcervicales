#ifndef _DEFUNS_H
#define _DEFUNS_H

#ifndef BOOL
#define BOOL int
#endif

#define PROPERTY(_FieldType, _Field) \
	private: \
		_FieldType m_##_Field; \
	public: \
	BOOL Set##_Field(_FieldType _Value) { \
		if (m_##_Field != _Value) { \
			m_##_Field = _Value; \
			return TRUE; \
		} else return FALSE; \
	} \
	inline _FieldType Get##_Field(void) const { \
		return m_##_Field; \
	} 


typedef struct _CRedimension {
	int tX, tY;
	double tFontScale;
	CPoint& TranslatePoint(CPoint& Pt) const {
		Pt.x += tX;
		Pt.y += tY;
		return Pt;
	}
	CRect& TranslateRect(CRect& Rect) const {
		Rect.left += tX;
		Rect.top += tY;
		Rect.right += tX;
		Rect.bottom += tY;
		return Rect;
	}
	CPoint GetPoint(const CPoint& Pt) const {
		return CPoint(Pt.x + tX, Pt.y + tY);
	}
	CRect GetRect(const CRect& Rect) const {
		CRect Result = Rect;
		return TranslateRect(Result);
	}
} CRedimension;

#endif
