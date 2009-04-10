// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#ifdef WIN32
#include <direct.h>
#endif

#include "engine.h"
#include "storage.h"
#include "input.h"
#include "output.h"
#include "../platform.h"
#include "util.h"

#define DEFAULT_XML_STORAGE		"std.xml"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine(LIB_HANDLE hModule)
{
	init(hModule, NULL);
}

CEngine::CEngine(LIB_HANDLE hModule, COutputCache *pOutputCache)
{
	init(hModule, pOutputCache);
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

	if(m_pOutputCache != NULL)
	{
		delete m_pOutputCache;
		m_pOutputCache = NULL;
	}
}

void CEngine::init(LIB_HANDLE hModule, COutputCache *pOutputCache)
{
	m_hModule = hModule;
	m_pOutputCache = pOutputCache;

	m_pFirst = m_pLast = NULL;

	memset(m_szDisplayname,	0, sizeof(m_szDisplayname));
	memset(m_szLastError,	0, sizeof(m_szLastError));
	memset(m_szFileName,	0, sizeof(m_szFileName));

	m_callback = NULL;

	// Default values
	m_format.frequency		= 44100;
	m_format.format			= WAVE_FORMAT_PCM;
	m_format.channels		= 2;
	m_format.bitsPerSample	= 16;

	m_red = m_green = m_blue = 256;
}

tComponent *CEngine::add(IFloopyObject *obj, enumObjType type)
{
	obj->SetEngine( this );

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

IFloopySoundInput *CEngine::CreateInput(const char *filename)
{
	IFloopySoundInput *obj = NULL;
	enumObjType type = TYPE_INPUT;

	// Check if a filename has been given
	const char *plugin = get_plugin_name(filename);
	if(plugin)
	{
		if(0==strcmpi(plugin, DEFAULT_XML_STORAGE))
		{
			CEngine *engine = new CEngine(m_hModule);
			if(!engine->Open(filename))
			{
				sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
				delete engine;
				return NULL;
			}
			obj = new CInput(m_hModule, m_callback, m_pOutputCache);
			if(!((CInput*)obj)->Create(engine))
			{
				//setLastError(ERR_STR_FILENOTFOUND, filename);
				sprintf(m_szLastError, "Failed to create engine input");
				delete engine;
				delete obj;
				return NULL;
			}
		}
		else
		{
			obj = new CInput(m_hModule, m_callback, m_pOutputCache);
			if(!((CInput*)obj)->Create(plugin))
			{
				//setLastError(ERR_STR_FILENOTFOUND, filename);
				sprintf(m_szLastError, ERR_STR_FILENOTFOUND, plugin);
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
		obj = new CInput(m_hModule, m_callback, m_pOutputCache);
		if(!((CInput*)obj)->Create(filename))
		{
			//setLastError(ERR_STR_FILENOTFOUND, filename);
			sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
			delete obj;
			return NULL;
		}
	}

	add(obj, type);
	return obj;
}

IFloopySoundOutput *CEngine::CreateOutput(const char *filename, SOUNDFORMAT fmt)
{
	COutput *obj = new COutput(m_hModule);
	if(NULL == obj)
		return NULL;

	// Check if a filename has been given
	const char *plugin = get_plugin_name(filename);
	if( !plugin )
	{
		plugin = filename;
		filename = NULL;
		// And what if the component doesn's have the output interface?
	}

	if(!obj->Create(plugin, fmt))
	{
		//setLastError(ERR_STR_FILENOTFOUND, filename);
		sprintf(m_szLastError, ERR_STR_FILENOTFOUND, filename);
		delete obj;
		return NULL;
	}

	if( filename && !obj->Open(filename) )
	{
		strcpy(m_szLastError, obj->GetLastErrorDesc());
		delete obj;
		return NULL;
	}

	add(obj, TYPE_OUTPUT);

	return obj;
}

IFloopySoundInput *CEngine::CreateTrack(const char *name)
{
	SOUNDFORMAT *fmt1 = GetFormat();

	IFloopySoundInput *input = CreateInput(name);
	if(NULL == input)
		return NULL;
	SOUNDFORMAT *pfmt2 = input->GetFormat();

	//if(fmt1->format != pfmt2->format)
	//{
	//	return NULL;
	//}

	if(pfmt2->format != WAVE_FORMAT_PCM)
	{
		return NULL;
	}

	if((fmt1->channels == 1) && (pfmt2->channels == 2))
	{
		// Stereo2mono
		return NULL;
	}

	if(fmt1->frequency != pfmt2->frequency)
	{
		// Find adequate resampling component instead
		return NULL;
	}

	if(fmt1->bitsPerSample == 16 && pfmt2->bitsPerSample == 8)
	{
		IFloopySoundFilter *conv = (IFloopySoundFilter*)CreateInput("libstd.8to16bit");
		if(conv)
		{
			conv->SetSource(input);
			input = conv;
		}
	}
	else if(pfmt2->bitsPerSample > 16)
	{
		return NULL;
	}

	/*if(fmt1->bitsPerSample != pfmt2->bitsPerSample)
	{
		// Find adequate conversion component instead
		//"An error occurred loading the file. The specified file contains an unsupported bits per sample size."
		return NULL;
	}*/

	if(SIZE_INFINITE != input->GetSize())
	{
		IFloopySoundFilter *loop = (IFloopySoundFilter*)CreateInput("libstd.loop");
		if(loop)
		{
			loop->SetSource(input);
			input = loop;
		}
	}

	IFloopySoundFilter *volume = (IFloopySoundFilter*)CreateInput("libstd.volume");
	if(volume)
	{
		volume->SetSource(input);
		input = volume;
	}

	IFloopySoundFilter *track = (IFloopySoundFilter*)CreateInput("libstd.track");
	if(track)
	{
		track->SetSource(input);
		input = track;
	}

	if((fmt1->channels == 2) && (pfmt2->channels == 1))
	{
		IFloopySoundFilter *filter = (IFloopySoundFilter*)CreateInput("libstd.mono2stereo");
		if(filter)
		{
			filter->SetSource(input);
			input = filter;
		}
	}

	return input;
}

bool CEngine::Open(const char *filename)
{
	bool bResult = false;

	const char *plugin = get_plugin_name(filename);
	if(plugin)
	{
		if(0==strcmpi(plugin, DEFAULT_XML_STORAGE))
		{
			CStorage storage(m_hModule, this, plugin);
			bResult = storage.Load(filename);
		}
		else
		{
			char *sep = strrchr(plugin, '.');
			if(sep)
			{
				m_source = CreateInput( filename );
				bResult = (NULL != m_source);
			}
			else
			{
				CStorage storage(m_hModule, this, plugin);
				bResult = storage.Load(filename);
			}
		}

		if(bResult)
			strcpy(m_szFileName, filename);
	}
	return bResult;
}

bool CEngine::Save(const char *filename)
{
	if(!filename && m_szFileName)
		filename = m_szFileName;

	if(!filename)
		return false;

	bool result = false;
	const char *name = get_plugin_name(filename);
	if( name )
	{
		CStorage storage(m_hModule, this, name);
		result = storage.Save(filename);
		//if(!result)
		//	storage.GetLastError(m_szLastError, strlen(m_szLastError));

		// Save all child engines
		if(result)
			saveChildEngines();
	}
	else
		sprintf(m_szLastError, "Adequate storage component not found for %s.", filename);
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

const char *CEngine::get_plugin_name(const char *filename)
{
	char *ext = strrchr(filename, '.');
	if(ext)
	{
		ext++;

		if(0 == strcmpi(ext, "xml"))
			return DEFAULT_XML_STORAGE;
		if(0 == strcmpi(ext, "wav"))
			return "std.wavfile";
		if(0 == strcmpi(ext, "svg"))
			return "svg.svgout";
		if(0 == strcmpi(ext, "mp3"))
			return "mp3file";
		if(0 == strcmpi(ext, "txt"))
			return "std.dump";
		//if(0 == strcmpi(ext, "hz"))
		//	return "libstd.tonegen";
	}
	return NULL;
}

void CEngine::Close()
{
	tComponent *tmp = m_pFirst;
	while(tmp)
	{
		switch((int)tmp->type)
		{
		case TYPE_INPUT:
			((CInput*)tmp->obj)->Close();
			break;
		case TYPE_ENGINE:
			((CEngine*)tmp->obj)->Close();
		}
		tmp = tmp->next;
	}

	m_red = m_green = m_blue = 256;
}

const char *CEngine::GetLastErrorDesc()
{
	return m_szLastError;
}

const char *CEngine::GetDisplayName()
{
	return m_szDisplayname;
}

void CEngine::SetDisplayName(const char *name, int len)
{
	memset(m_szDisplayname, 0, MAX_FNAME);
	memcpy(m_szDisplayname, name, (len < MAX_FNAME ? len : MAX_FNAME));
}

void CEngine::RegisterUpdateCallback(UpdateCallback func)
{
	m_callback = func;
}

int CEngine::EmptyBuffer(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = GetFormat();

	if(fmt)
	{
		switch(fmt->bitsPerSample)
		{
		case 8:
			memset(data, 128, size);
			return size;
		case 16:
			memset(data, 0, size);
			return size;
		}
	}

	return 0;
}

bool CEngine::GetColor(UINT *r, UINT *g, UINT *b)
{
	if(m_red<256 && m_green<256 && m_blue<256)
	{
		*r = m_red;
		*g = m_green;
		*b = m_blue;
		return true;
	}
	return false;
}

void CEngine::SetColor(UINT r, UINT g, UINT b)
{
	m_red	= r;
	m_green	= g;
	m_blue	= b;
}
