#include "graph.h"
#include "CommandParser/libcli.h"
#include "nwcli.c"

extern graph_t *build_first_topo();
graph_t *topo = NULL;

int main(int argc, char **argv)
{
	nw_init_cli();
	topo = build_first_topo();
	start_shell();
	return 0;
}
