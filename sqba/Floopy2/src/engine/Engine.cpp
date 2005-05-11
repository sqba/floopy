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
	m_pFirst = m_pLast = NULL;
}

CEngine::~CEngine()
{
	if(m_source)
		delete m_source;

	tComponent *tmp;
	while(m_pFirst)
	{
		//printf("%d %d %f", first->offset, first->index, first->value);
		tmp = m_pFirst->next;
		delete m_pFirst;
		m_pFirst = tmp;
	}
}

tComponent *CEngine::add(IFloopy *comp)
{
	if(NULL == m_pLast)
	{
		m_pFirst = new tComponent;
		memset(m_pFirst, 0, sizeof(tComponent));
		m_pLast = m_pFirst;
		m_pFirst->comp = comp;
	}
	else
	{
		tComponent *tmp = new tComponent;
		memset(tmp, 0, sizeof(tComponent));
		tmp->prev = m_pLast;
		tmp->comp = comp;
		m_pLast->next = tmp;
		m_pLast = tmp;
	}
	return m_pLast;
}
/*
void CEngine::dump(FILE *fp)
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		tmp->comp->dump(fp);
		tmp = tmp->next;
	}
}
*/
IFloopySoundInput *CEngine::CreateInput(char *filename)
{
	IFloopySoundInput *comp = NULL;

	// Check if a filename has been given
	char *plugin = getStorageName(filename);
	if(plugin)
	{
		comp = new CInput(plugin);
		comp->Open(filename);
	}
	else
		comp = new CInput(filename);

	if(comp->GetComponent())
	{
		add(comp);
		return comp;
	}
	else
		return NULL;
}

IFloopySoundOutput *CEngine::CreateOutput(char *filename, WAVFORMAT fmt)
{
	IFloopySoundOutput *comp = NULL;

	// Check if a filename has been given
	char *plugin = getStorageName(filename);
	if(plugin)
	{
		comp = new COutput(plugin, fmt);
		comp->Open(filename);
	}
	else
		comp = new COutput(filename, fmt);

	add(comp);

	return comp;
}

BOOL CEngine::Open(char *filename)
{
	/*if(0 == strcmpi(filename, "test"))
	{
		CStorage storage(this, "TEST");
		return storage.Load(filename);
	}*/

	char *name = getStorageName(filename);
	if(name)
	{
		CStorage storage(this, name);
		return storage.Load(filename);
	}
/*
	char *ext = strrchr(filename, '.');
	int i = filename+strlen(filename) - ext;
	ext = filename + i;

	// Check extension and then select apropriate plugin!
	if(0 == strcmpi(ext, "test"))
	{
		CStorage storage(this, "test");
		return storage.Load(filename);
	}
	if(0 == strcmpi(ext, "xml"))
	{
		CStorage storage(this, "xml_expat");
		return storage.Load(filename);
	}
*/
	return FALSE;
}

BOOL CEngine::Save(char *filename)
{
	/*if(0 == strcmpi(filename, "test"))
	{
		CStorage storage(this, "TEST");
		return storage.Save(filename);
	}*/

	char *name = getStorageName(filename);
	if(name)
	{
		CStorage storage(this, name);
		return storage.Save(filename);
	}
/*
	// Check extension and then select apropriate plugin!
	char *ext = strrchr(filename, '.');
	int i = filename+strlen(filename) - ext;
	ext = filename + i;

	// Check extension and then select apropriate plugin!
	if(0 == strcmpi(ext, "test"))
	{
		CStorage storage(this, "test");
		return storage.Save(filename);
	}
	if(0 == strcmpi(ext, "xml"))
	{
		CStorage storage(this, "xml_expat");
		return storage.Save(filename);
	}
*/
	return FALSE;
}

char *CEngine::getStorageName(char *filename)
{
	char *ext = strrchr(filename, '.');
	if(ext)
	{
		//int i = filename+strlen(filename) - ext;
		ext += 1;

		if(0 == strcmpi(ext, "test"))
			return "test";
		if(0 == strcmpi(ext, "xml"))
			return "xml_expat";
		if(0 == strcmpi(ext, "hz"))
			return "tonegen";
		if(0 == strcmpi(ext, "wav"))
			return "wavfile";
	}
	return NULL;
}
