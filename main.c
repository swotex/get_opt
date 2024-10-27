#include "get_options.h"

int main(int argc, char **argv)
{
	ADD_ARG(.help="un test", .short_flag="-t", .type="str");
	ADD_ARG(.help="usdffst", .long_flag="-tutu", .type="int");
	ADD_ARG(.help="verbose mode", .short_flag="-v", .type="bool");
	ADD_ARG(.help="un test2", .arg_noflag="URL", .type="str");
	ADD_ARG(.help="choose a port", .short_flag="-p", .type="str");

	parseArgs(argv);
}