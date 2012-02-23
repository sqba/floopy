#include "object.h"

CObject::CObject()
{
	init( NULL );
}

CObject::CObject(CObject *pParent)
{
	init( pParent );
}

CObject::~CObject()
{
}

void CObject::init(CObject *pParent)
{
	m_pParent	= NULL;
	m_fLeft		= -100;
	m_fTop		= 60.f;
	m_fRight	= 100.f;
	m_fBottom	= -60.f;

	m_Color.fRed	= 0.2;
	m_Color.fGreen	= 0.2;
	m_Color.fBlue	= 1.0;
}

void CObject::DrawFrame()
{
	glNewList(1, GL_COMPILE_AND_EXECUTE);

	float fThickness = 10.f;

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glColor3f(m_Color.fRed, m_Color.fGreen, m_Color.fBlue);

	glBegin(GL_QUADS);
		// left face
		glVertex3f(m_fLeft, m_fBottom, fThickness);
		glVertex3f(m_fLeft, m_fBottom, -fThickness);
		glVertex3f(m_fLeft, m_fTop, -fThickness);
		glVertex3f(m_fLeft, m_fTop, fThickness);

		// top side face
		glVertex3f(m_fLeft, m_fTop, fThickness);
		glVertex3f(m_fRight, m_fTop, fThickness);
		glVertex3f(m_fRight, m_fTop, -fThickness);
		glVertex3f(m_fLeft, m_fTop, -fThickness);

		// right face
		glVertex3f(m_fRight, m_fTop, fThickness);
		glVertex3f(m_fRight, m_fTop, -fThickness);
		glVertex3f(m_fRight, m_fBottom, -fThickness);
		glVertex3f(m_fRight, m_fBottom, fThickness);

		// bottom side face
		glVertex3f(m_fLeft, m_fBottom, fThickness);
		glVertex3f(m_fRight, m_fBottom, fThickness);
		glVertex3f(m_fRight, m_fBottom, -fThickness);
		glVertex3f(m_fLeft, m_fBottom, -fThickness);
	glEnd(); // GL_QUADS

	glEndList();
}
