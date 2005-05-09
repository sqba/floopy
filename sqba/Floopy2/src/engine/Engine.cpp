// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include "Storage.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
	//m_offset = m_stopAt = m_length = 0;
}

CEngine::~CEngine()
{
	if(m_source)
		delete m_source;
}

IFloopySoundInput  *CEngine::CreateInput(char *plugin)
{
	return new CInput(plugin);
}

IFloopySoundOutput *CEngine::CreateOutput(char *plugin, WAVFORMAT fmt)
{
	return new COutput(plugin, fmt);
}

BOOL CEngine::Open(char *filename)
{
	if(0 == strcmpi(filename, "test"))
	{
		CStorage storage(this, "TEST");
		return storage.Load(filename);
	}

	// Check extension and then select apropriate plugin!
	CStorage storage(this, "xml_expat");
	return storage.Load(filename);
}

BOOL CEngine::Save(char *filename)
{
	if(0 == strcmpi(filename, "test"))
	{
		CStorage storage(this, "TEST");
		return storage.Save(filename);
	}

	// Check extension and then select apropriate plugin!
	CStorage storage(this, "xml_expat");
	return storage.Save(filename);
}

