#ifndef GET_OPTIONS
# define GET_OPTIONS
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <stdbool.h>

# define NOT_FLAG 0
# define FLAG_NOARG 1
# define FLAG_ARG 2

typedef struct
{
    char *short_flag; //short or long needed
    char *long_flag;
    char *arg_noflag;
	// bool argReq;
	char *type; //needed
	char *metavar; //optional
	void *default_arg; //optional ?
	char *help; //needed
} ArgConfig;

typedef struct s_arg
{
    ArgConfig flag;
	void *arg;
} t_arg;

// si le type est bool/NULL/none -> ne require pas d'arg --- ######## si il y a un default de set -> arg no obligatoire ###### enfaite non. le default sert de fois si pas de flag




# define ADD_ARG(...) add_arg_dispatch((ArgConfig){__VA_ARGS__})

void add_arg_dispatch(ArgConfig config);

void parseArgs(char **argv);

void clearAll(void) ; //freeAll

#endif