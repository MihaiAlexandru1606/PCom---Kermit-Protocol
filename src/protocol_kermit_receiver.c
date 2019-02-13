#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lib.h"
#include "utils.h"

/**
 * Functia care "executa comanta" afarenata fiecarui tip de pachete
 */
static void run_package(Package package, int *file_descriptor, Byte *timeout, 
                        Byte *maxl, Bool *finish) {

    if (package.type == S) {
        Data_S dataS;
        memcpy(&dataS, package.data, sizeof(Data_S));
        *timeout = dataS.time;
        *maxl = dataS.maxl;
    } else if (package.type == F) {
        *file_descriptor = open((char *) package.data, O_CREAT | O_TRUNC | O_WRONLY,
                                0777);
        CHECK_ERROR(*file_descriptor, REC_ERROR);
    } else if (package.type == D) {
        int data_len = package.len - SIZE_SEQ - SIZE_TYPE - SIZE_CHECK - SIZE_MARK;
        assert(write(*file_descriptor, package.data, data_len) == data_len);
    } else if (package.type == Z) {
        assert(close(*file_descriptor) == 0);
    } else if (package.type == B) {
        *finish = TRUE;
    } else if (package.type == E) {
        exit(EXIT_FAILURE);
    }
}

void recev_files() {
    int file_descriptor;
    Byte numberSeq = 255;
    int timeout = TIMEOUT;
    Byte time_out, maxl;
    Bool finsh = FALSE;

    while (TRUE) {
        msg *received = NULL;
        int numberOfAttempt = 0;
        Package received_package;
        Package transmitted;
        msg send;

        if (numberSeq == 255) {
            numberOfAttempt = 1; /** se asteapta de maxim 2 ori */
        }
        while (numberOfAttempt < MAX_TRY) {
            received = receive_message_timeout(timeout);

            /** daca nu am primit nimic */
            if (received == NULL) {
                /** retrimitera utimul pachet */
                send_message(&send);
                numberOfAttempt++;
                continue;
            }

            msg_to_package(*received, &received_package);
            unsigned short check = crc16_ccitt(&received_package,
                                               received_package.len - 1);
            if (check == received_package.check) {
                if (received_package.seq == numberSeq) {

                    create_package_Y(&transmitted, numberSeq);
                    package_to_msg(&transmitted, &send);
                    send_message(&send);

                    free(received);
                    break;
                }
                /** daca mesajul este corect */
                run_package(received_package, &file_descriptor, &time_out, &maxl,
                            &finsh);

                if (numberSeq == 255) { /** pentru primul pachet */
                    create_package_ACK_S(&transmitted, 0, time_out, maxl);
                } else {
                    create_package_Y(&transmitted, (numberSeq + 1) % MOD);
                }

                package_to_msg(&transmitted, &send);
                send_message(&send);

                free(received);
                numberSeq = (numberSeq + 1) % MOD; /**actualizare numarul seq*/
                break;
            }

            free(received);

            create_package_N(&transmitted, (unsigned char) ((numberSeq + 1) % MOD));
            package_to_msg(&transmitted, &send);
            send_message(&send);
        }

        if (finsh == TRUE) {
            break;
        }

        if (numberOfAttempt == MAX_TRY) {
            /* in cazul in care am incercat de MAX_TRY si nu am reusit sa transimitem
             * mesajul de inint se cosidera ca legatura este corupta si se transimite
             * un mesaj de erore
             */
            Package error;
            msg err;

            create_package_E(&error, UCHAR_MAX);
            package_to_msg(&error, &err);
            send_message(&err);
            fprintf(stderr, "Transmission Corrupt Receiver\n");
            exit(EXIT_FAILURE);
        }
    }

}
