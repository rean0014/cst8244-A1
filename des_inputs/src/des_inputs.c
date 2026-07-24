#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/netmgr.h>
#include "../../des_controller/src/des.h"

#define LINE_MAX_LEN 64

/* maps the typed command string to an event_type_t, or EV_INVALID */
static event_type_t parse_event(const char *s) {
	if (strcmp(s, "ls") == 0)
		return EV_LS;
	if (strcmp(s, "rs") == 0)
		return EV_RS;
	if (strcmp(s, "ws") == 0)
		return EV_WS;
	if (strcmp(s, "lo") == 0)
		return EV_LO;
	if (strcmp(s, "ro") == 0)
		return EV_RO;
	if (strcmp(s, "lc") == 0)
		return EV_LC;
	if (strcmp(s, "rc") == 0)
		return EV_RC;
	if (strcmp(s, "glu") == 0)
		return EV_GLU;
	if (strcmp(s, "gll") == 0)
		return EV_GLL;
	if (strcmp(s, "gru") == 0)
		return EV_GRU;
	if (strcmp(s, "grl") == 0)
		return EV_GRL;
	return EV_INVALID;
}

static int read_line(char *buf, size_t len) {
	if (fgets(buf, (int) len, stdin) == NULL)
		return -1;
	buf[strcspn(buf, "\r\n")] = '\0'; /* strip trailing newline */
	return 0;
}

int main(int argc, char *argv[]) {
	pid_t controller_pid;
	int controller_coid;
	char line[LINE_MAX_LEN];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <des_controller_pid>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	controller_pid = (pid_t) atoi(argv[1]);

	/* connection to des_controller's first channel (chid 1) */
	controller_coid = ConnectAttach(ND_LOCAL_NODE, controller_pid, 1,
			_NTO_SIDE_CHANNEL, 0);
	if (controller_coid == -1) {
		perror("ConnectAttach to des_controller");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		printf(
				"Enter the event type (ls= left scan, rs= right scan, ws= weight scale, "
						"lo =left open, ro=right open, lc = left closed, rc = right closed , "
						"gru = guard right unlock, grl = guard right lock, gll=guard left lock, "
						"glu = guard left unlock)\n");
		fflush(stdout);

		if (read_line(line, sizeof(line)) == -1)
			break;

		if (strcmp(line, "exit") == 0) {
			ConnectDetach(controller_coid);
			return EXIT_SUCCESS;
		}

		event_type_t ev = parse_event(line);
		if (ev == EV_INVALID) {
			printf("Unrecognized event '%s' - please try again.\n", line);
			continue;
		}

		des_request_t req;
		req.event = ev;
		req.person_id = 0;
		req.weight = 0;

		if (ev == EV_LS || ev == EV_RS) {
			printf("Enter the person_id\n");
			fflush(stdout);
			if (read_line(line, sizeof(line)) == -1)
				break;
			req.person_id = atoi(line);
		} else if (ev == EV_WS) {
			printf("Enter the weight\n");
			fflush(stdout);
			if (read_line(line, sizeof(line)) == -1)
				break;
			req.weight = atoi(line);
		}

		des_reply_t reply;
		if (MsgSend(controller_coid, &req, sizeof(req), &reply, sizeof(reply))
				== -1) {
			perror("MsgSend to des_controller");
		}
	}

	ConnectDetach(controller_coid);
	return EXIT_SUCCESS;
}
