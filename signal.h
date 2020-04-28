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
//#define RECV_ERROR "!"

#endif // SIGNAL_H
