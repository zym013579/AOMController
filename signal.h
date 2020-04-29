#ifndef SIGNAL_H
#define SIGNAL_H

//#define R_TTL 0x00

#define SEND_START "(1)"
#define SEND_PAUSE "(2)"
#define SEND_CONNECTION_TEST "(3)"
#define SEND_CHOOSE_FREQ(x) ("(4," << x << ")")

#define RECV_MAX_LENGTH 1
#define RECV_MIN_LENGTH 1
#define RECV_OK "!"
//#define RECV_ERROR "~"

#define DEFAULT_VOL_QUANTIFY_LEVEL 4096
#define DEFAULT_MIN_DELTA_TIME 1
#define DEFAULT_REALTIME_QUANTIFY true

#endif // SIGNAL_H
