#include <stdlib.h>
#ifndef LIB
#define LIB

/*
 * Contantele pentru program
 */
#define TIMEOUT     5000
#define MAXL        250
#define MSGSIZE     1400
#define MAX_TRY     3
#define MOD         64
#define SEN_ERROR   "Sender error"
#define REC_ERROR   "Recever error"

#define SOH         0x01

/*define-uri pentru dimensiuni */
#define SIZE_SOH    1
#define SIZE_LEN    1
#define SIZE_SEQ    1
#define SIZE_TYPE   1
#define SIZE_CHECK  2
#define SIZE_MARK   1

/* define-uri pentru valori implicite */
#define CR          0x0D

#define TRUE        1
#define FALSE       0
#define Bool        int

#define Byte        unsigned char
#define Word        unsigned short

/*
 * Enumeratia pentru tipul mesajului, semnificatia :
 * S -> Send-init
 * F -> File Header
 * D -> Data
 * Z -> End Of File
 * B -> End Of Transation
 * Y -> ACK
 * N -> NACK
 * E -> Error
 */
typedef enum {
    S = 'S',
    F = 'F',
    D = 'D',
    Z = 'Z',
    B = 'B',
    Y = 'Y',
    N = 'N',
    E = 'E'
} Type_Package;

#pragma pack(1)

/**
 * Structirile pentru tipurile msg, Package, Data_S
 * Data_S rerezinta structura pentru data din pachetul S
 */
typedef struct {
    int len;
    char payload[MSGSIZE];
} msg;

typedef struct {
    Byte soh;
    Byte len;
    Byte seq;
    Byte type;
    Byte data[MAXL];
    Word check;
    Byte mark;
} Package;

typedef struct {
    Byte maxl;
    Byte time;
    Byte npad;
    Byte padc;
    Byte eol;
    Byte qctl;
    Byte qbin;
    Byte chkt;
    Byte rept;
    Byte capa;
    Byte r;

} Data_S;

#pragma pack()

#define CHECK_ERROR(ARG, MSG)   \
        if( ARG < 0){           \
            perror(MSG);        \
            exit(EXIT_FAILURE); \
        }

void init(char *remote, int remote_port);

void set_local_port(int port);

void set_remote(char *ip, int port);

int send_message(const msg *m);

int recv_message(msg *r);

msg *receive_message_timeout(int timeout); //timeout in milliseconds

unsigned short crc16_ccitt(const void *buf, int len);

#endif
