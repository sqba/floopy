// Storage.h: interface for the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGE_H__0D941E22_780C_456C_9734_8E1B9C86089A__INCLUDED_)
#define AFX_STORAGE_H__0D941E22_780C_456C_9734_8E1B9C86089A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "expat/xmlparse/xmlparse.h"
#include "../../ifloopy.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


class CStorage  
{
public:
	CStorage(IFloopySoundEngine *engine);
	virtual ~CStorage();

	bool Load(char *filename);
	bool Save(char *filename);
	void Close();

private:
	void writeParameter(IFloopySoundInput *input, int index, bool &root);
	void writeParameters(IFloopySoundInput *input);
	void writeInput(IFloopySoundInput *input);
	void writeProperties(IFloopySoundInput *input);
	void writeTabs();

private:
	IFloopySoundEngine *m_pEngine;
	FILE *m_pFile;
	int m_iLevel;
};

#endif // !defined(AFX_STORAGE_H__0D941E22_780C_456C_9734_8E1B9C86089A__INCLUDED_)
