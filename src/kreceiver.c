#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib.h"
#include "protocol_kermit_receiver.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc, char** argv) {
    init(HOST, PORT);

    recev_files();

    return 0;
}
