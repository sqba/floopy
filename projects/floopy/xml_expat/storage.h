
#ifndef _XML_STORAGE_H_
#define _XML_STORAGE_H_


//#include "expat/xmlparse/xmlparse.h"
#include <expat.h>
#include "../ifloopy.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../platform.h"

class CStorage
{
public:
	CStorage(IFloopySoundEngine *engine);
	virtual ~CStorage();

	bool Load(char *filename);
	bool Save(char *filename);
	void Close();

// Saving
private:
	void writeParameter(IFloopySoundInput *input, int index, bool &root);
	void writeParameters(IFloopySoundInput *input);
	void writeInput(IFloopySoundInput *input);
	void writeProperties(IFloopySoundInput *input);
	void writeIndentation();

// Loading
private:
	struct tSessionInfo
	{
		struct tTimeline
		{
			IFloopySoundInput *obj;
			char *data;
		} objects[1024];	// Temporary solution

		int iIndex;
		int iBuffLen;
		int iBuffEnd;
		int level;

		const char *pElement;
		char pPath[MAX_PATH];
		char *pBuff;

		XML_Parser parser;
		IFloopySoundEngine	*pEngine;
		IFloopySoundInput	*pInput;
		IFloopySoundInput	*buff[20]; //temp

		bool bInitialized;
		//char filename[MAX_PATH];
	};

	static void startElement(void *userData, const char *name, const char **atts);
	static void endElement(void *userData, const char *name);
	static void elementData(void *userData, const char *data, int len);
	static void loadColor(char *str, UINT *r, UINT *g, UINT *b);
	static bool IsFilter(IFloopySoundInput *input);
	static void loadTimelines(tSessionInfo *si);
	static void loadTimeline(tSessionInfo *si, IFloopySoundInput *input, char *data);
	static void loadInputNode(tSessionInfo *si, const char **atts);
	static void loadPropertiesNode(tSessionInfo *si, const char **atts);

private:
	IFloopySoundEngine *m_pEngine;
	FILE *m_pFile;
	int m_iIndent;
};

#endif // _XML_STORAGE_H_
