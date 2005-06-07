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

CEngine::CEngine(HMODULE hModule)
{
	m_hModule = hModule;

	//m_offset = m_stopAt = m_length = 0;
	m_pFirst = m_pLast = NULL;
	memset(m_szDisplayname, 0, sizeof(m_szDisplayname));
	memset(m_szLastError, 0, sizeof(m_szLastError));
	memset(m_szPath, 0, sizeof(m_szPath));
	memset(m_szFileName, 0, sizeof(m_szFileName));

	m_callback = NULL;

	if(hModule)
	{
		//char path[MAX_PATH] = {0};
		//getcwd(m_szPath, MAX_PATH);
		GetModuleFileName(hModule, m_szPath, MAX_PATH);
		char *tmp = strrchr(m_szPath, '\\');
		if(tmp)
			*(tmp+1) = '\0';
		//GetModuleFileName(GetModuleHandle(NULL), m_szPath, MAX_PATH);
		/*FILE *fp = fopen("engine.cfg", "r");
		if(fp)
		{
			fscanf(fp, "%s", m_szPath);
			//chdir(m_szPath);
			fclose(fp);
		}*/
	}
}

CEngine::~CEngine()
{
	if(m_source)
		delete m_source;

	tComponent *tmp;
	while(m_pFirst)
	{
		tmp = m_pFirst->next;
		delete m_pFirst;
		m_pFirst = tmp;
	}
}

tComponent *CEngine::add(IFloopy *obj, enumObjType type)
{
	if(NULL == m_pLast)
	{
		m_pFirst = new tComponent;
		memset(m_pFirst, 0, sizeof(tComponent));
		m_pLast = m_pFirst;
		m_pFirst->obj = obj;
		m_pFirst->type = type;
	}
	else
	{
		tComponent *tmp = new tComponent;
		memset(tmp, 0, sizeof(tComponent));
		tmp->type = type;
		tmp->prev = m_pLast;
		tmp->obj = obj;
		m_pLast->next = tmp;
		m_pLast = tmp;
	}
	return m_pLast;
}

IFloopySoundInput *CEngine::CreateInput(char *filename)
{
	IFloopySoundInput *obj = NULL;
	enumObjType type = TYPE_INPUT;

	// Check if a filename has been given
	char *plugin = getPluginName(filename);
	if(plugin)
	{
		char path[MAX_PATH] = {0};
		if(strlen(m_szPath) > 0)
		{
			strcpy(path, m_szPath);
			if(path[strlen(path)-1] != '\\')
				path[strlen(path)] = '\\';
		}
		strcat(path, plugin);

		if(0==strcmpi(plugin, "xml_expat"))
		{
			CEngine *engine = new CEngine(m_hModule);
			if(!engine->Open(filename))
			{
				sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
				delete engine;
				return NULL;
			}
			//type = TYPE_ENGINE;
			obj = new CInput(m_callback);
			if(!((CInput*)obj)->Create(engine))
			{
				//setLastError(ERR_STR_FILENOTFOUND, filename);
				sprintf(m_szLastError, ERR_STR_FILENOTFOUND, path);
				delete obj;
				return NULL;
			}
		}
		else
		{
			obj = new CInput(m_callback);
			if(!((CInput*)obj)->Create(path))
			{
				//setLastError(ERR_STR_FILENOTFOUND, filename);
				sprintf(m_szLastError, ERR_STR_FILENOTFOUND, path);
				delete obj;
				return NULL;
			}
			if(!obj->Open(filename))
			{
				//setLastError(ERR_STR_FILENOTFOUND, filename);
				sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
				delete obj;
				return NULL;
			}
		}
	}
	else
	{
		char *tmp = strrchr(filename, '\\');
		if(tmp)
			filename = tmp+1;

		char path[MAX_PATH] = {0};
		if(strlen(m_szPath) > 0)
		{
			strcpy(path, m_szPath);
			if(path[strlen(path)] != '\\')
				path[strlen(path)] = '\\';
		}
		strcat(path, filename);

		obj = new CInput(m_callback);
		if(!((CInput*)obj)->Create(path))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, path);
			delete obj;
			return NULL;
		}
	}

	add(obj, type);
	return obj;
}

IFloopySoundOutput *CEngine::CreateOutput(char *filename, SOUNDFORMAT fmt)
{
	COutput *obj = NULL;

	// Check if a filename has been given
	char *plugin = getPluginName(filename);
	if(plugin)
	{
		obj = new COutput();
		if(!obj->Create(plugin, fmt) || !obj->Open(filename))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
			delete obj;
			return NULL;
		}
	}
	else
	{
		obj = new COutput();
		if(!obj->Create(filename, fmt))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
			delete obj;
			return NULL;
		}
	}

	add(obj, TYPE_OUTPUT);

	return obj;
}

BOOL CEngine::Open(char *filename)
{
	BOOL bResult = FALSE;

	char *plugin = getPluginName(filename);
	if(plugin)
	{
		char path[MAX_PATH] = {0};
		if (strlen(m_szPath) > 0)
		{
			strcpy(path, m_szPath);
			if(path[strlen(path)-1] != '\\')
				path[strlen(path)-1] = '\\';
		}
		strcat(path, plugin);

		CStorage storage(this, path);
		bResult = storage.Load(filename);

		if(bResult)
			strcpy(m_szFileName, filename);
	}
	return bResult;
}

BOOL CEngine::Save(char *filename)
{
	if(!filename && m_szFileName)
		filename = m_szFileName;

	if(!filename)
		return FALSE;

	BOOL result = FALSE;
	char *name = getPluginName(filename);
	if(name)
	{
		CStorage storage(this, name);
		result = storage.Save(filename);
		//if(!result)
		//	storage.GetLastError(m_szLastError, strlen(m_szLastError));

		// Save all child engines
		if(result)
			saveChildEngines();
	}
	else
		sprintf(m_szLastError, "Adequate storage objonent not found for %s.\0", filename);
		//setLastError(ERR_STR_FILENOTFOUND, filename);

	return result;
}

void CEngine::saveChildEngines()
{
	// Save all child engines
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		if(tmp->type == TYPE_ENGINE)
		{
			CEngine *engine = (CEngine*)tmp->obj;
			engine->Save(NULL);
		}
		tmp = tmp->next;
	}
}

char *CEngine::getPluginName(char *filename)
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
		if(obj == tmp->obj)
		{
			switch(tmp->type)
			{
			case TYPE_INPUT:
				CInput *input = (CInput*)obj;
				input->SetParamAt(offset, index, value);
			}
			return;
		}
		tmp = tmp->next;
	}
}

BOOL CEngine::ResetParamAt(IFloopy *obj, int offset, int index)
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		if(obj == tmp->obj)
		{
			switch(tmp->type)
			{
			case TYPE_INPUT:
				CInput *input = (CInput*)obj;
				return input->ResetParamAt(offset, index);
			}
			return FALSE;
		}
		tmp = tmp->next;
	}
	return FALSE;
}

void CEngine::EnableAt(IFloopy *obj, int offset, BOOL bEnable)
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		if(obj == tmp->obj)
		{
			switch(tmp->type)
			{
			case TYPE_INPUT:
				CInput *input = (CInput*)obj;
				input->EnableAt(offset, bEnable);
			}
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
		switch(tmp->type)
		{
		case TYPE_INPUT:
			((CInput*)tmp->obj)->Close();
			break;
		case TYPE_ENGINE:
			((CEngine*)tmp->obj)->Close();
		}
		tmp = tmp->next;
	}
}

char *CEngine::GetLastErrorDesc()
{
	return m_szLastError;
}

char *CEngine::GetDisplayName()
{
	return m_szDisplayname;
}

void CEngine::SetDisplayName(char *name, int len)
{
	memcpy(m_szDisplayname, name, (len < NAME_LEN ? len : NAME_LEN));
}

void CEngine::RegisterUpdateCallback(UpdateCallback func)
{
	m_callback = func;
}
/*
int CEngine::GetSize()
{
	int stb = samplesToBytes();
	CInput *src = (CInput*)GetSource();
	int start = src->getStartOffset();
	int end = src->getEndOffset();
	return (end - start) / stb;
}

int CEngine::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	//assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
		return (fmt->bitsPerSample / 8) * fmt->channels;
	else
		return 0;
}

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

void CEngine::dump(FILE *fp)
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		tmp->obj->dump(fp);
		tmp = tmp->next;
	}
}
*/
/*
enumObjType CEngine::createObject(char *filename)
{
}
*/
