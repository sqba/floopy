
/**
 * Vraca numericku vrednost parametra komandne linije.
 * \param argc [in] broj argumenata
 * \param argv [out] parametri komandne linije
 * \param arg [in] trazeni parametar
 * \param def [in] default
 * \return vrednost parametra ili def, ako parametar nije nadjen
 */
int GetArg(int argc, char* argv[], char *arg, int def)
{
	int i, result=def;
	for(i=1; i<argc; i++) {
		if(argv[i][0] == '-') {
			char *c = argv[i]+1;
			if( (i<argc-1) && (argv[i+1][0] != '-'))
				i++;
			if( 0 == stricmp(c, arg) ) {
				if(i<argc) {
					if(argv[i][0] == '-')
						result = -1;
					else
						result = atoi(argv[i]);
					break;
				}
				result = -1;
				break;
			}
		}
	}
	return result;
}

/**
 * Vraca tekstualnu vrednost parametra komandne linije.
 * \param argc [in] broj argumenata
 * \param argv [out] parametri komandne linije
 * \param arg [in] trazeni parametar
 * \param def [in] default
 * \return vrednost parametra ili def, ako parametar nije nadjen
 */
char *GetArg(int argc, char* argv[], char *arg, char *def)
{
	int i;
	char *result = def;
	for(i=1; i<argc; i++) {
		if(argv[i][0] == '-') {
			char *c = argv[i]+1;
			if( (i<argc-1) && (argv[i+1][0] != '-'))
				i++;
			if( 0 == stricmp(c, arg) ) {
				if(i<argc) {
					if(argv[i][0] == '-')
						result = NULL;
					else
						result = argv[i];
					break;
				}
				result = NULL;
				break;
			}
		}
	}
	return result;
}
