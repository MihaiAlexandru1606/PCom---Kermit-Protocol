#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lib.h"
#include "protocol_kermit_sender.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char** argv) {
    init(HOST, PORT);

    send_file(argv,argc - 1, TIMEOUT, MAXL);

    return 0;
}
