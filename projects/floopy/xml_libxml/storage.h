#ifndef CSTORAGE_H_
#define CSTORAGE_H_

#include "../ifloopy.h"
#include "../platform.h"
#include <stdio.h>

class CStorage
{
public:
	CStorage(IFloopySoundEngine *engine);
	virtual ~CStorage();

	bool Load(char *filename);
	bool Save(char *filename);

// Saving
private:
	void writeParameter(FILE *fp, IFloopySoundInput *input, int index, bool &root);
	void writeParameters(FILE *fp, IFloopySoundInput *input);
	void writeInput(FILE *fp, IFloopySoundInput *input);
	void writeProperties(FILE *fp, IFloopySoundInput *input);
	void writeIndentation(FILE *fp);

private:
	IFloopySoundEngine *m_pEngine;
	int m_iIndent;
};

#endif /*CSTORAGE_H_*/
