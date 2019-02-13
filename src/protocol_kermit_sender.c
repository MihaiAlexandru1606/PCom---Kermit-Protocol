#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "lib.h"
#include "utils.h"

/**
 * Functia pentru trimiterea pachetelor se incerca transmiterea pachetului pana
 * cand se primeste ack pentru pachetul respectiv sau nu se primeste nimic de
 * 3 ori, daca nu se primeste nimic de 3 ori se considera transmisiunea corupta
 * si se trimite mesajul de erorare si se inchide sender-ul
 * Pentru este ultimul pachet este un caz, daca nu se primeste numic de 2 ori iese
 * din functie si se condsidera ca informatile utile au fost transmise
 * @param transmitted pachetul pe care vrem sa-l trmitem
 * @param seq numarul de secventa
 * @param is_last daca este ultimul
 */
static void send_package(msg *transmitted, Byte seq, Bool is_last) {
    msg *confirm = NULL;
    int numberOfAttempt = 0;

    while (numberOfAttempt < MAX_TRY) {

        /**
         * daca este ultimul pachet si nu s-a primit raspuns
         */
        if (is_last == TRUE) {
            if (numberOfAttempt == MAX_TRY - 1)
                break;
        }

        send_message(transmitted);
        confirm = receive_message_timeout(TIMEOUT);

        /**
         * daca pachetul este pierdut
         */
        if (confirm == NULL) {
            numberOfAttempt++;
            continue;
        }

        Package received;
        msg_to_package(*confirm, &received);
        /** daca este corect si este ack pentru pachetul trimis  atunci se iese din
         * while stiind ca pachetul a fost primit corect
         * nu mai trebui sa existe un caz pentru nack, dat fiind daca se primeste
         * un nack nu este indepliminta conditia si se repeta din nou
         * */
        if (received.type == Y && received.seq == seq) {

            unsigned short check = crc16_ccitt(&received, received.len - 1);
            if (check == received.check) {
                free(confirm);
                break;
            }
        }
        free(confirm);
    }

    if (numberOfAttempt == MAX_TRY) {
        /* incazul in care am incercat de MAX_TRY si nu am reusit sa transimitem
         * mesajul, se cosidera ca legatura este corupta, se inchide legatura
         * si se transimite un mesaj de erore
         */
        Package error;
        msg err;

        create_package_E(&error, UCHAR_MAX);
        package_to_msg(&error, &err);
        send_message(&err);
        fprintf(stderr, "Transmission Corrupt Sender\n");
        exit(EXIT_FAILURE);
    }

}

/**
 * Functia pentru trimiterea unui pachet de tipul S
 * @param timeout timpul limita in secunde
 * @param maxl dimensiunea maxima a campului data
 */
static void send_init(Byte timeout, Byte maxl) {
    Package package;
    msg transmitted;

    create_package_S(&package, timeout, maxl);
    package_to_msg(&package, &transmitted);
    send_package(&transmitted, 0, FALSE);
}

/**
 * Functia pentru trimitera pachetului F
 * @param fileName numele fisierului
 * @param seq numarul de secventa
 */
static void send_package_F(char *fileName, Byte seq) {
    Package package_F;
    char new_name[MAXL];
    msg transmitted;

    sprintf(new_name, "recv_%s", fileName);
    create_package_F(&package_F, new_name, seq);
    package_to_msg(&package_F, &transmitted);
    send_package(&transmitted, seq, FALSE);
}

/**
 * Functia de transmiterea a pachetelor D
 * @param data informatia
 * @param len_data lungimea informatie
 * @param seq numarul de secventa
 */
static void send_package_D(const Byte *data, Byte len_data, Byte seq) {
    Package send;
    msg transmitted;

    create_package_D(&send, data, len_data, seq);
    package_to_msg(&send, &transmitted);
    send_package(&transmitted, seq, FALSE);
}


/**
 * functia pentru transmitrea pachetului de tip Z
 * @param seq numarul de secvenata
 */
static void send_package_Z(Byte seq) {
    Package send;
    msg transmitted;

    create_package_Z(&send, seq);
    package_to_msg(&send, &transmitted);
    send_package(&transmitted, seq, FALSE);
}

/**
 * functia pentru trimiterea pachetului de tip B
 * @param seq numarul de secventa
 */
static void send_package_B(Byte seq) {
    Package send;
    msg transmitted;


    create_package_B(&send, seq);
    package_to_msg(&send, &transmitted);
    send_package(&transmitted, seq, TRUE);
}

/**
 * Functai prntru trimiterea fisiserelor. Se trimite mai intai setarile
 * sender-ului, apoi pentru fiecare fisier in parte se trimite file header,
 * datele si EOF, apoi un EOT
 * @param filesName numele fisierelor
 * @param numberFiles numarul de fisere
 * @param timeout timpul limita
 * @param maxl valorea maxima de date transmise, data din Package
 */
void send_file(char **filesName, int numberFiles, int timeout, int maxl) {
    unsigned char numberSeq = 1; /* pentru ca primul este send-init */
    send_init(timeout, maxl); /** trimiterea pachetului de S, de init */
    int index_file;
    int file_descriptor;
    __off_t size_file = 0;

    for (index_file = 1; index_file <= numberFiles; index_file++) {

        file_descriptor = open(filesName[index_file], O_RDONLY);
        CHECK_ERROR(file_descriptor, SEN_ERROR);

        struct stat fileStat;
        CHECK_ERROR(fstat(file_descriptor, &fileStat), SEN_ERROR);
        size_file = fileStat.st_size;

        /** send file header */
        send_package_F(filesName[index_file], numberSeq);
        numberSeq = (numberSeq + 1) % MOD;

        /** trimiterea datelor */
        while (size_file != 0) {
            Byte buffer[MAXL];
            ssize_t size_read = read(file_descriptor, buffer, MAXL);
            CHECK_ERROR(size_read, SEN_ERROR);
            size_file -= size_read;

            send_package_D(buffer, size_read, numberSeq);
            numberSeq = (numberSeq + 1) % MOD;
        }

        CHECK_ERROR(close(file_descriptor), SEN_ERROR);
        send_package_Z(numberSeq);
        numberSeq = (numberSeq + 1) % MOD;
    }

    send_package_B(numberSeq);
}

