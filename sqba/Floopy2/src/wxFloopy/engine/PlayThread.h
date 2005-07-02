// PlayThread.h: interface for the CPlayThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYTHREAD_H__808A0B2C_B783_4248_88AE_BCD81B4AF7C5__INCLUDED_)
#define AFX_PLAYTHREAD_H__808A0B2C_B783_4248_88AE_BCD81B4AF7C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayThread : public wxThread  
{
public:
	CPlayThread();
	virtual ~CPlayThread();

};

#endif // !defined(AFX_PLAYTHREAD_H__808A0B2C_B783_4248_88AE_BCD81B4AF7C5__INCLUDED_)
