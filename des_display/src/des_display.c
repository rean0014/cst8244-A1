#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include "../../des_controller/src/des.h"

static const char* lock_str(int locked) {
	return locked ? "locked" : "unlocked";
}
static const char* open_str(int open_) {
	return open_ ? "open" : "closed";
}

int main(void) {
	int chid;
	int rcvid;
	des_status_t status;
	des_status_reply_t reply = { .ack = 1 };

	printf("The display is running as process_id %d.\n", getpid());
	fflush(stdout);

	chid = ChannelCreate(0);
	if (chid == -1) {
		perror("ChannelCreate");
		exit(EXIT_FAILURE);
	}

	printf("[status update : initial startup]\n");
	fflush(stdout);

	for (;;) {
		rcvid = MsgReceive(chid, &status, sizeof(status), NULL);
		if (rcvid == -1) {
			if (errno == EINTR)
				continue;
			perror("MsgReceive");
			continue;
		}
		if (rcvid == 0) {
			/* pulse - not used in this assignment, ignore */
			continue;
		}

		printf(
				"[status update] LEFT: %s/%s   RIGHT: %s/%s   occupied=%d   person_id=%d   weight=%d\n",
				lock_str(status.left_locked), open_str(status.left_open),
				lock_str(status.right_locked), open_str(status.right_open),
				status.occupied, status.person_id, status.weight);
		printf("    -> %s\n", status.message);
		fflush(stdout);

		MsgReply(rcvid, EOK, &reply, sizeof(reply));
	}

	return EXIT_SUCCESS;
}
