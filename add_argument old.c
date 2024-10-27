#include "get_options.h"

static ArgConfig *AllArgs = NULL;
static int ArgsCnt = 0;

void add_arg_dispatch(ArgConfig config) {
    // printf("Short arg: %s\n", config.short_flag);
    // printf("Long arg: %s\n", config.long_flag? config.long_flag : "N/A");
    // printf("Metavar: %s\n", config.metavar ? config.metavar : "N/A");
    // printf("Help: %s\n", config.help ? config.help : "N/A");
    // printf("Default: %p\n", config.default_arg);
    // printf("ARG: %s\n", config.arg_noflag);
    
	ArgConfig *temp = realloc(AllArgs, (ArgsCnt + 1) * sizeof(ArgConfig));
    if (temp == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return;
    }
    AllArgs = temp;
    
    // Ajoute le nouvel élément et incrémente le compteur
    AllArgs[ArgsCnt] = config;
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

void testPrintStruct(t_fa_parse *flags)
{
	while(flags)
	{
		printf(": %s and bool : %d\n", flags->flag, flags->valid_flag);
		flags = flags->next;
	}
}

void ptest()
{
	for (int i = 0; i < ArgsCnt; i++)
	{
		printf("bah ouais : %s\n", AllArgs[i].help);
	}
}

void	add_back(t_fa_parse **lst, t_fa_parse *new)
{
	t_fa_parse	*tmp;

	if (*lst == NULL)
		*lst = new;
	else
	{
		tmp = *lst;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

static t_fa_parse *getFlags(char *line)
{
	int pos;
	int flagLen;
	t_fa_parse *flags = NULL;

	pos = strFind(line, '-', 0);

	while (pos != -1)
	{
		flagLen = 0;
		if (pos == 0 || isspace(line[pos - 1]))
		{
			for (int i = 0; line[pos + i] && !isspace(line[pos + i]); i++)
				flagLen++;
			char *new_flag = malloc(flagLen + 1);
			if (!new_flag)
				return (flags);
			strncpy(new_flag, line + pos, flagLen);
			new_flag[flagLen] = 0;
			t_fa_parse *tmp = malloc(sizeof(t_fa_parse));
			if (!tmp)
			{
				free(new_flag);
				return (flags);
			}
			tmp->flag = new_flag;
			tmp->next = NULL;
			tmp->valid_flag = false;
			add_back(&flags, tmp);
		}
		else
			flagLen++;
		pos = strFind(line, '-', (pos + flagLen));
	}
	return (flags);
}

char *argv_inline(char **argv)
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

int isValid_single_flag(t_fa_parse *flags)
{
	while (flags)
	{
		for (int i = 0; i < ArgsCnt; i++)
		{
			if (AllArgs[i].long_flag && strcmp(flags->flag, AllArgs[i].long_flag) == 0)
			{
				flags->valid_flag = true;
				break ;
			}
			if (AllArgs[i].short_flag && strcmp(flags->flag, AllArgs[i].short_flag) == 0)
			{
				flags->valid_flag = true;
				break ;
			}
		}
		flags = flags->next;
	}
}

int isValid_composed_flag(t_fa_parse *flags)
{
	
}

static void freeStructFlags()
{
	free(AllArgs);
}

static void freeStructArgs(t_fa_parse **flags)
{
	t_fa_parse *tmp;

	while (*flags)
	{
		tmp = *flags;
		*flags = (*flags)->next;
		free(tmp->flag);
		free(tmp);
	}
}

// auto print -h and usage if not requirement
void parseArgs(char **argv)
{
	char *line = argv_inline(argv);
	t_fa_parse *flags = getFlags(line);
	free(line);

	isValid_single_flag(flags);
	isValid_composed_flag(flags);
	
	testPrintStruct(flags);
	ptest();

	freeStructFlags();
	freeStructArgs(&flags);
}

	
