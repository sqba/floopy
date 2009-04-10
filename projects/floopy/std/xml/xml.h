
#include <string>
#include <vector>
#include "../../ifloopy.h"
#include "../../platform.h"
#include "tinyxml/tinyxml.h"


class CXml : public IFloopyEngineStorage
{
public:
	CXml();
	virtual ~CXml();

	bool Load(IFloopySoundEngine *engine, const char *filename);
	bool Save(IFloopySoundEngine *engine, const char *filename);
	const char *GetExtension();

private:
	IFloopySoundInput *load_input(IFloopySoundInput *parent, TiXmlElement* pElement);
	IFloopySoundInput *create_input(IFloopySoundInput *parent, TiXmlElement* pElement);
	bool set_name(IFloopySoundInput *input, TiXmlElement* pElement);
	bool open_source(IFloopySoundInput *input, TiXmlElement* pElement);
	bool set_properties(IFloopySoundInput *input, TiXmlElement* pElement);
	bool set_timeline(IFloopySoundInput *input, TiXmlElement* pElement);
	void set_param(IFloopySoundInput *input, float offset, const char *param, float value);
	void set_color(IFloopySoundInput *input, TiXmlElement* pElement);
	bool IsMixer(IFloopySoundInput *input);
	bool IsFilter(IFloopySoundInput *input);
	std::vector<std::string> tokenize(const std::string& str,const std::string& delimiters);

private:
	char m_szPath[MAX_PATH];
	IFloopySoundEngine *m_pEngine;
};
