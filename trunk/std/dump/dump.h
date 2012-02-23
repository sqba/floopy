
#include <stdio.h>
#include "../../ifloopy.h"
#include "../../platform.h"


class CDump : public IFloopyEngineStorage
{
public:
	CDump();
	virtual ~CDump();

	bool Load(IFloopySoundEngine*, const char*) { return false; }
	bool Save(IFloopySoundEngine*, const char*);
	const char *GetExtension() { return ""; }

private:
	void dump_indentation();
	void dump_properties(IFloopyProperty *obj);
	void dump_timeline(IFloopyTimeline *obj);
	void dump_timeline(IFloopyTimeline *obj, int param);
	void dump_parameters(IFloopyParam *obj);
	void dump_info(IFloopyDisplay *obj);
	void dump_inputs(IFloopySoundMixer *obj);
	void dump_input(IFloopySoundInput *obj);

private:
	FILE *m_fp;
	int m_iLevel;
};
