#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <GL/gl.h>

struct color
{
	float fRed;
	float fGreen;
	float fBlue;
};

class CObject
{
public:
	CObject();
	CObject(CObject *pParent);
	virtual ~CObject();

    virtual void DrawFrame();

	float Left()	{ return m_fLeft; }
	float Top()		{ return m_fTop; }
	float Right()	{ return m_fRight; }
	float Bottom()	{ return m_fBottom; }
	float Width()	{ return m_fRight - m_fLeft; }
	float Height()	{ return m_fTop - m_fBottom; }

private:
	void init(CObject *pParent);

protected:
	CObject *m_pParent;
	float m_fLeft, m_fTop, m_fRight, m_fBottom;
	color m_Color;
};

#endif // _OBJECT_H_
