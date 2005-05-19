// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include "Storage.h"
#include <assert.h>
#include <direct.h>

#define ERR_STR_FILENOTFOUND	"File '%s' not found.\0"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
	//m_offset = m_stopAt = m_length = 0;
	m_pFirst = m_pLast = NULL;
	memset(m_name, 0, sizeof(m_name));
	memset(m_szLastError, 0, sizeof(m_szLastError));
	memset(m_szPath, 0, sizeof(m_szPath));

	//char path[MAX_PATH] = {0};
	//getcwd(path, MAX_PATH);
	FILE *fp = fopen("engine.cfg", "r");
	if(fp)
	{
		fscanf(fp, "%s", m_szPath);
		//chdir(m_szPath);
		fclose(fp);
	}

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
	CInput *comp = NULL;

	// Check if a filename has been given
	char *plugin = getStorageName(filename);
	if(plugin)
	{
		//char path[MAX_PATH] = {0};
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

		char path[MAX_PATH] = {0};
		strcpy(path, m_szPath);
		if(path[strlen(path)] != '\\')
			path[strlen(path)] = '\\';
		strcat(path, plugin);

		comp = new CInput();
		if(!comp->Create(path))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, path);
			delete comp;
			return NULL;
		}
		if(!comp->Open(filename))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
			delete comp;
			return NULL;
		}

	}
	else
	{
		char *tmp = strrchr(filename, '\\');
		if(tmp)
			filename = tmp+1;

		char path[MAX_PATH] = {0};
		strcpy(path, m_szPath);
		if(path[strlen(path)] != '\\')
			path[strlen(path)] = '\\';
		strcat(path, filename);

		comp = new CInput();
		if(!comp->Create(path))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, path);
			delete comp;
			return NULL;
		}
	}

	add(comp, TRUE);
	return comp;
}

IFloopySoundOutput *CEngine::CreateOutput(char *filename, SOUNDFORMAT fmt)
{
	IFloopySoundOutput *comp = NULL;

	// Check if a filename has been given
	char *plugin = getStorageName(filename);
	if(plugin)
	{
		comp = new COutput(plugin, fmt);
		if(!comp->Open(filename))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
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
	BOOL result = FALSE;
	char *name = getStorageName(filename);
	if(name)
	{
		CStorage storage(this, name);
		result = storage.Save(filename);
		//if(!result)
		//	storage.GetLastError(m_szLastError, strlen(m_szLastError));
	}
	else
		sprintf(m_szLastError, "Adequate storage component not found for %s.\0", filename);
		//setLastError(ERR_STR_FILENOTFOUND, filename);
	return result;
}

char *CEngine::getStorageName(char *filename)
{
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

void CEngine::ResetParamAt(IFloopy *obj, int offset, int index)
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		if((obj == tmp->comp) && (tmp->bInput))
		{
			CInput *input = (CInput*)obj;
			input->ResetParamAt(offset, index);
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
/*
int CEngine::GetLastError()
{
	return 0;
}

BOOL CEngine::GetLastError(char *str, int len)
{
	int l = sizeof(m_szLastError);
	memcpy(str, m_szLastError, (len>l?l:len));
	return TRUE;
}

void CEngine::setLastError(char *err, char *str)
{
	memset(m_szLastError, 0, sizeof(m_szLastError));
	sprintf(m_szLastError, err, str);
}
*/