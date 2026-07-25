#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/netmgr.h>
#include "des.h"

/* ---- function-pointer machinery: each state returns the next state ---- */
typedef struct state_result state_result_t;
typedef state_result_t (*state_func_t)(void);
struct state_result {
	state_func_t next;
};

/* ---- persistent controller context (the "outputs" and saved inputs) ---- */
static int input_chid; /* channel we receive events from des_inputs on   */
static int display_coid; /* connection to des_display, to send status     */

static direction_t direction = DIR_NONE;
static int person_id = 0;
static int weight = 0;
static int left_locked = 1, left_open = 0;
static int right_locked = 1, right_open = 0;
static int occupied = 0;

/* forward declarations of all state handlers */
static state_result_t state_idle(void);
static state_result_t state_scanned(void);
static state_result_t state_a_unlocked(void);
static state_result_t state_a_open(void);
static state_result_t state_weighed(void);
static state_result_t state_a_closed(void);
static state_result_t state_a_locked(void);
static state_result_t state_b_unlocked(void);
static state_result_t state_b_open(void);
static state_result_t state_b_closed(void);

/* ---- helpers ---- */

/* blocks until a request arrives from des_inputs, replies immediately (ack) */
static void get_event(des_request_t *req) {
	int rcvid;
	des_reply_t reply = { .ack = 1 };

	for (;;) {
		rcvid = MsgReceive(input_chid, req, sizeof(*req), NULL);
		if (rcvid == -1) {
			if (errno == EINTR)
				continue;
			perror("MsgReceive");
			continue;
		}
		if (rcvid == 0)
			continue; /* pulse, ignore */
		MsgReply(rcvid, EOK, &reply, sizeof(reply));
		return;
	}
}

/* sends the current Moore output (door/occupancy status + message) to des_display */
static void send_status(const char *msg) {
	des_status_t status;
	des_status_reply_t reply;

	status.left_locked = left_locked;
	status.left_open = left_open;
	status.right_locked = right_locked;
	status.right_open = right_open;
	status.occupied = occupied;
	status.person_id = person_id;
	status.weight = weight;
	snprintf(status.message, sizeof(status.message), "%s", msg);

	if (MsgSend(display_coid, &status, sizeof(status), &reply, sizeof(reply))
			== -1) {
		perror("MsgSend to display");
	}
}

/* ================= STATE HANDLERS ================= */
/* Moore style: emit output for this state first, then wait until the ONE
 * expected event arrives. Anything else is ignored (loop continues). */

static state_result_t state_idle(void) {
	left_locked = 1;
	left_open = 0;
	right_locked = 1;
	right_open = 0;
	occupied = 0;
	direction = DIR_NONE;
	person_id = 0;
	weight = 0;
	send_status("waiting for scan");

	des_request_t req;
	for (;;) {
		get_event(&req);
		if (req.event == EV_LS) {
			person_id = req.person_id;
			direction = DIR_ENTERING;
			break;
		} else if (req.event == EV_RS) {
			person_id = req.person_id;
			direction = DIR_EXITING;
			break;
		}
		/* anything else: illegal here, ignore and keep waiting */
	}
	occupied = 1;
	state_result_t r = { .next = state_scanned };
	return r;
}

static state_result_t state_scanned(void) {
	send_status("waiting for guard to unlock A");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_GLU : EV_GRU;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		left_locked = 0;
	else
		right_locked = 0;

	state_result_t r = { .next = state_a_unlocked };
	return r;
}

static state_result_t state_a_unlocked(void) {
	send_status("A unlocked - waiting for A to open");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_LO : EV_RO;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		left_open = 1;
	else
		right_open = 1;

	state_result_t r = { .next = state_a_open };
	return r;
}

static state_result_t state_a_open(void) {
	send_status("A open - waiting for weight");

	des_request_t req;
	for (;;) {
		get_event(&req);
		if (req.event == EV_WS) {
			weight = req.weight;
			break;
		}
	}

	state_result_t r = { .next = state_weighed };
	return r;
}

static state_result_t state_weighed(void) {
	send_status("weight recorded - waiting for A to close");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_LC : EV_RC;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		left_open = 0;
	else
		right_open = 0;

	state_result_t r = { .next = state_a_closed };
	return r;
}

static state_result_t state_a_closed(void) {
	send_status("A closed - waiting for guard to lock A");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_GLL : EV_GRL;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		left_locked = 1;
	else
		right_locked = 1;

	state_result_t r = { .next = state_a_locked };
	return r;
}

static state_result_t state_a_locked(void) {
	send_status("A locked - waiting for guard to unlock B");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_GRU : EV_GLU;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		right_locked = 0;
	else
		left_locked = 0;

	state_result_t r = { .next = state_b_unlocked };
	return r;
}

static state_result_t state_b_unlocked(void) {
	send_status("B unlocked - waiting for B to open");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_RO : EV_LO;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		right_open = 1;
	else
		left_open = 1;

	state_result_t r = { .next = state_b_open };
	return r;
}

static state_result_t state_b_open(void) {
	send_status("B open - waiting for B to close");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_RC : EV_LC;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		right_open = 0;
	else
		left_open = 0;

	state_result_t r = { .next = state_b_closed };
	return r;
}

static state_result_t state_b_closed(void) {
	send_status("B closed - waiting for guard to lock B");

	des_request_t req;
	event_type_t expected = (direction == DIR_ENTERING) ? EV_GRL : EV_GLL;
	for (;;) {
		get_event(&req);
		if (req.event == expected)
			break;
	}
	if (direction == DIR_ENTERING)
		right_locked = 1;
	else
		left_locked = 1;

	state_result_t r = { .next = state_idle }; /* cycle complete */
	return r;
}

/* ================= MAIN ================= */

int main(int argc, char *argv[]) {
	pid_t display_pid;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <des_display_pid>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	display_pid = (pid_t) atoi(argv[1]);

	printf("The controller is running as process_id %d.\n", getpid());
	fflush(stdout);

	/* channel to receive events from des_inputs */
	input_chid = ChannelCreate(0);
	if (input_chid == -1) {
		perror("ChannelCreate");
		exit(EXIT_FAILURE);
	}

	/* connection to des_display's channel (its first channel = chid 1) */
	display_coid = ConnectAttach(ND_LOCAL_NODE, display_pid, 1,
			_NTO_SIDE_CHANNEL, 0);
	if (display_coid == -1) {
		perror("ConnectAttach to des_display");
		exit(EXIT_FAILURE);
	}

	state_func_t current = state_idle;
	for (;;) {
		state_result_t result = current();
		current = result.next;
	}

	return EXIT_SUCCESS;
}
