// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include "Storage.h"
#include <assert.h>
#include <direct.h>

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

tComponent *CEngine::add(IFloopy *comp, BOOL bInput)
{
	if(NULL == m_pLast)
	{
		m_pFirst = new tComponent;
		memset(m_pFirst, 0, sizeof(tComponent));
		m_pLast = m_pFirst;
		m_pFirst->comp = comp;
		m_pFirst->bInput = bInput;
	}
	else
	{
		tComponent *tmp = new tComponent;
		memset(tmp, 0, sizeof(tComponent));
		tmp->bInput = bInput;
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
		char path[MAX_PATH] = {0};
		/*FILE *fp = fopen("engine.cfg", "r");
		if(fp)
		{
			fscanf(fp, "%s", path);
		}

		if(strlen(path) > 0)
		{
			if(path[strlen(path)-1] != '\\')
				strcat(path, "\\");
		}
		fclose(fp);*/

		/*strcat(path, "D:\\sqba\\Projekti\\Floopy!\\Floopy2\\Debug\\");
		strcat(path, plugin);
		comp = new CInput(path);*/

		comp = new CInput(plugin);
		if(!comp->Open(filename))
		{
			delete comp;
			return NULL;
		}

	}
	else
	{
		char *tmp = strrchr(filename, '\\');
		if(tmp)
			filename = tmp+1;

		comp = new CInput(filename);
	}

	add(comp, TRUE);
	return comp;
}

IFloopySoundOutput *CEngine::CreateOutput(char *filename, WAVFORMAT fmt)
{
	IFloopySoundOutput *comp = NULL;

	// Check if a filename has been given
	char *plugin = getStorageName(filename);
	if(plugin)
	{
		comp = new COutput(plugin, fmt);
		if(!comp->Open(filename))
		{
			delete comp;
			return NULL;
		}
	}
	else
		comp = new COutput(filename, fmt);

	add(comp, FALSE);

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
	char path[MAX_PATH] = {0};
	FILE *fp = fopen("engine.cfg", "r");
	if(fp)
	{
		fscanf(fp, "%s", path);
		chdir(path);
		fclose(fp);
	}

	char *ext = strrchr(filename, '.');
	if(ext)
	{
		ext += 1;

		if(0 == strcmpi(ext, "test"))
			return "test";
		if(0 == strcmpi(ext, "xml"))
			return "xml_expat";
		if(0 == strcmpi(ext, "hz"))
			return "tonegen";
		if(0 == strcmpi(ext, "wav"))
			return "wavfile";
		if(0 == strcmpi(ext, "svg"))
			return "svgfile";
		if(0 == strcmpi(ext, "mp3"))
			return "mp3file";
	}
	return NULL;
}

void CEngine::SetParamAt(IFloopy *obj, int offset, int index, float value)
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		if((obj == tmp->comp) && (tmp->bInput))
		{
			CInput *input = (CInput*)obj;
			input->SetParamAt(offset, index, value);
			return;
		}
		tmp = tmp->next;
	}
}

void CEngine::EnableAt(IFloopy *obj, int offset, BOOL bEnable)
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		if((obj == tmp->comp) && (tmp->bInput))
		{
			CInput *input = (CInput*)obj;
			input->EnableAt(offset, bEnable);
			return;
		}
		tmp = tmp->next;
	}
}

void CEngine::Close()
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		if(tmp->bInput)
		{
			CInput *input = (CInput*)tmp->comp;
			input->Close();
		}
		tmp = tmp->next;
	}
}
