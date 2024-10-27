#include "get_options.h"

static t_arg *AllArgs = NULL;
static int ArgsCnt = 0;


// ajouter des verification de config obligatoire
void add_arg_dispatch(ArgConfig config) {
    // printf("Short arg: %s\n", config.short_flag);
    // printf("Long arg: %s\n", config.long_flag? config.long_flag : "N/A");
    // printf("Metavar: %s\n", config.metavar ? config.metavar : "N/A");
    // printf("Help: %s\n", config.help ? config.help : "N/A");
    // printf("Default: %p\n", config.default_arg);
    // printf("ARG: %s\n", config.arg_noflag);
    
	t_arg *temp = realloc(AllArgs, (ArgsCnt + 1) * sizeof(t_arg));
    if (temp == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return;
    }
    AllArgs = temp;
    
    // Ajoute le nouvel élément et incrémente le compteur
    AllArgs[ArgsCnt].flag = config;
	AllArgs[ArgsCnt].arg = config.default_arg;
    ArgsCnt++;
}

static int strFind(char *src, char find, int start)
{
	for (int i = start; src[i]; i++)
	{
		if (src[i] == find)
		{
			return (i);
		}
	}
	return (-1);
}

// void remove_substring(char *str, const char *substr) {
//     char *pos = strstr(str, substr);  // Localise la première occurrence de substr dans str

//     if (pos != NULL) {
//         size_t len = strlen(substr);
//         // Décale le reste de la chaîne pour écraser la sous-chaîne
//         memmove(pos, pos + len, strlen(pos + len) + 1);  // +1 pour inclure le '\0' final
//     }
// }

static char *argv_inline(char **argv)
{
	int len = 0;
	char *line;

	for (int i = 1; argv[i]; i++)
	{
		len += strlen(argv[i]);
		if (argv[i + 1])
			len++;
	}
	line = malloc((len + 1) * sizeof(char));
	if (!line)
		return (NULL);
	bzero(line, len + 1);
	for (int i = 1; argv[i]; i++)
	{
		strcat(line, argv[i]);
		if (argv[i + 1])
			strcat(line, " ");
	}
	return(line);
}

static bool isneededArg(char *type)
{
	if (strcmp(type, "bool") == 0)
		return (false);
	if (strcmp(type, "none") == 0)
		return (false);
	if (strcmp(type, "NULL") == 0)
		return (false);
	return (true);
}

static int isvalid_flag(char *str, int flag_len)
{
	for (int i = 0; i < ArgsCnt; i++)
	{
		if (AllArgs[i].flag.short_flag && strncmp(str, AllArgs[i].flag.short_flag, flag_len) == 0)
		{
			if (isneededArg(AllArgs[i].flag.type) == true)
				return (FLAG_ARG);
			return (FLAG_NOARG);
		}
		if (AllArgs[i].flag.long_flag && strncmp(str, AllArgs[i].flag.long_flag, flag_len) == 0)
		{
			if (isneededArg(AllArgs[i].flag.type) == true)
				return (FLAG_ARG);
			return (FLAG_NOARG);
		}
	}
	return (NOT_FLAG);
}

static void addArg(char *flag, int flag_len, char *arg)
{
	for (int i = 0; i < ArgsCnt; i++)
	{
		if (AllArgs[i].flag.short_flag && strncmp(flag, AllArgs[i].flag.short_flag, flag_len) == 0)
		{
			if ((!arg || isvalid_flag(arg, strlen(arg)) == true) && isneededArg(AllArgs[i].flag.type)) //change needearg to swith bool
			{
				flag[flag_len] = 0;
				printf("Error besoin d'un arg pour : %s\n", flag);
				return ;
			}
			else if (strcmp(AllArgs[i].flag.type, "bool") == 0)
				AllArgs[i].arg = "true";
			else
				AllArgs[i].arg = arg;
			return ;
		}
		if (AllArgs[i].flag.long_flag && strncmp(flag, AllArgs[i].flag.long_flag, flag_len) == 0)
		{
			if ((!arg || isvalid_flag(arg, strlen(arg)) == true) && isneededArg(AllArgs[i].flag.type))
			{
				flag[flag_len] = 0;
				printf("Error besoin d'un arg pour : %s\n", flag);
				return ;
			}
			else if (strcmp(AllArgs[i].flag.type, "bool") == 0)
				AllArgs[i].arg = "true";
			else
				AllArgs[i].arg = arg;
			return ;
		}
	}
}

static void get_single_flag(char *line)
{
	int pos = strFind(line, '-', 0);

	while (pos != -1)
	{
		int flagLen = 0;
		for (int i = 0; line[pos + i] && !isspace(line[pos + i]); i++)
			flagLen++;
		if (pos == 0 || isspace(line[pos - 1]))
		{
			int needearg = isvalid_flag(line + pos, flagLen);
			if (needearg != NOT_FLAG)
			{
				if (needearg == FLAG_NOARG)
				{
					addArg(line + pos, flagLen, NULL);
					memmove(line + pos, line + pos + flagLen, strlen(line + pos + flagLen) + 1);
					pos = 0;
				}
				else
				{
					int posArg = pos + flagLen;
					int argLen = 0;

					while (line[posArg] && isspace(line[posArg]))
						posArg++;
					while (line[posArg + argLen] && !isspace(line[posArg + argLen]))
						argLen++;
					if (line[posArg] == 0)
					{
						line[pos + flagLen] = 0;
						printf("Error this flag need arg : %s\n", line + pos);
						return ;
					}
					char *tmp = malloc((argLen + 1) * sizeof(char));
					if (!tmp)
						return ;
					bzero(tmp, argLen + 1);
					strncpy(tmp, line + posArg, argLen);
					addArg(line + pos, flagLen, tmp);
					memmove(line + pos, line + posArg + argLen, strlen(line + posArg + argLen) + 1);
					pos = 0;
				}
			}
		}
		pos = strFind(line, '-', (pos + flagLen));
	}
}

static void get_composed_flag(char *line)
{
	int pos = strFind(line, '-', 0);
	char takeOne[2];

	takeOne[0] = '-';
	takeOne[2] = 0;
	while (pos != -1)
	{
		int flagLen = 0;
		bool flagHasDeleted = false;
		for (int i = 0; line[pos + i] && !isspace(line[pos + i]); i++)
			flagLen++;
		if (pos == 0 || isspace(line[pos - 1]))
		{
			for (int i = 1; i < flagLen; i++)
			{
				takeOne[1] = line[pos + i];
				int isValid = isvalid_flag(takeOne, 2);
				if (isValid == NOT_FLAG)
				{
					printf("Error is not a flag : %s\n", takeOne);
					return ;
				}
				if (isValid == FLAG_NOARG)
				{
					addArg(takeOne, 2, NULL);
					continue ;
				}
				if (isValid == FLAG_ARG)
				{
					int posArg = i + 1;
					int argLen = 0;
					while (line[pos + posArg] && isspace(line[pos + posArg]))
						posArg++;
					if (line[posArg] == 0)
					{
						line[pos + flagLen] = 0;
						printf("Error this flag need arg : %s\n", takeOne);
						return ;
					}
					while (line[pos + posArg + argLen] && !isspace(line[pos + posArg + argLen]))
						argLen++;
					char *tmp = malloc((argLen + 1) * sizeof(char));
					if (!tmp)
						return ;
					bzero(tmp, argLen + 1);
					strncpy(tmp, line + pos + posArg, argLen);
					addArg(takeOne, 2, tmp);
					memmove(line + pos, line + pos + posArg + argLen, strlen(line + pos + posArg + argLen) + 1);
					flagHasDeleted = true;
					pos = 0;
					break ;
				}
			}
			if (flagHasDeleted == false)
				memmove(line + pos, line + pos + flagLen, strlen(line + pos + flagLen) + 1);
			flagHasDeleted = false;
			pos = 0;
			flagLen = 0;
		}
		pos = strFind(line, '-', (pos + flagLen));
	}
}

static void get_main_arg(char *line)
{
	int pos = 0;
	for (int i = 0; i < ArgsCnt && line[pos]; i++)
	{
		if (AllArgs[i].flag.arg_noflag)
		{
			while (line[pos] && isspace(line[pos]))
				pos++;
			if (!line[pos])
			{
				printf("Error need argument : %s\n", AllArgs[i].flag.arg_noflag);
				return ;
			}
			int argLen = 0;
			while (line[pos + argLen] && !isspace(line[pos + argLen]))
				argLen++;
			char *tmp = malloc(argLen + 1);
			if(!tmp)
				return ;
			bzero(tmp, argLen + 1);
			strncpy(tmp, line + pos, argLen);
			AllArgs[i].arg = tmp;
			pos += argLen;
		}
	}
	while (line[pos] && isspace(line[pos]))
		pos++;
	if (line[pos])
	{
		printf("Error unrecognized arguments : %s\n", line + pos);
	}
}

void ptest()
{
	for (int i = 0; i < ArgsCnt; i++)
	{
		printf("bah ouais : %s et %s\n", AllArgs[i].flag.help, AllArgs[i].arg);
		// free(AllArgs[i].arg);
	}
}

// auto print -h and usage if not requirement
void parseArgs(char **argv)
{
	char *line = argv_inline(argv);
	get_single_flag(line);
	printf("line final : %s\n", line);
	get_composed_flag(line);
	get_main_arg(line);
	free(line);

	
	ptest();
	// tmp
	free(AllArgs);

}

	
