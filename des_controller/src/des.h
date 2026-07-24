#ifndef DES_H_
#define DES_H_

/* ---- Event types sent des_inputs -> des_controller ---- */
typedef enum {
	EV_LS, /* left scan  */
	EV_RS, /* right scan */
	EV_WS, /* weight scale */
	EV_LO, /* left door opened */
	EV_RO, /* right door opened */
	EV_LC, /* left door closed */
	EV_RC, /* right door closed */
	EV_GLU, /* guard left unlock */
	EV_GLL, /* guard left lock */
	EV_GRU, /* guard right unlock */
	EV_GRL, /* guard right lock */
	EV_INVALID /* unrecognized input string (never sent, caught in des_inputs) */
} event_type_t;

/* ---- direction of current cycle, tracked by des_controller ---- */
typedef enum {
	DIR_NONE, DIR_ENTERING, /* scanned at left, A=left, B=right */
	DIR_EXITING /* scanned at right, A=right, B=left */
} direction_t;

/* ---- Request message: des_inputs -> des_controller ---- */
typedef struct {
	event_type_t event;
	int person_id; /* valid only for EV_LS / EV_RS */
	int weight; /* valid only for EV_WS */
} des_request_t;

typedef struct {
	int ack;
} des_reply_t;

/* ---- Status message: des_controller -> des_display ---- */
typedef struct {
	int left_locked;
	int left_open;
	int right_locked;
	int right_open;
	int occupied;
	int person_id;
	int weight;
	char message[128];
} des_status_t;

typedef struct {
	int ack;
} des_status_reply_t;

#endif /* DES_H_ */
