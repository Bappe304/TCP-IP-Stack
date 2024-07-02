#include "graph.h"
#include "CommandParser/libcli.h"
#include "nwcli.c"
#include "comm.c"

extern graph_t *build_first_topo();
graph_t *topo = NULL;

int main(int argc, char **argv)
{
	nw_init_cli();

	topo = build_first_topo();

	/*Give some time for the receiver thread to start*/
	sleep(2);

	node_t *snode = get_node_by_node_name(topo, "R0_re");
	interface_t *senderintf = get_node_if_by_name(snode, "eth0/0");

	char msg[] = "Hello, how are you\0";
	send_pkt_out(msg, strlen(msg), senderintf);

	start_shell();
	return 0;
}
