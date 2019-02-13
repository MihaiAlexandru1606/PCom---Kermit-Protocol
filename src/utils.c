#include "lib.h"
#include <stdlib.h>
#include <string.h>

void msg_to_package(msg message, Package *package) {
    int len_soh_data = message.len - SIZE_CHECK - SIZE_MARK;
    int size_check_mark = SIZE_CHECK + SIZE_MARK;

    memset(package, 0, sizeof(Package));
    memcpy(package, message.payload, len_soh_data);
    memcpy(&package->check, message.payload + len_soh_data, size_check_mark);
}

void package_to_msg(Package *aPackage, msg *message) {
    int size_check_mark = SIZE_CHECK + SIZE_MARK;
    int len = (*aPackage).len + SIZE_SOH + SIZE_LEN;
    int len_soh_data = len - SIZE_CHECK - SIZE_MARK;

    (*message).len = len;
    memset(message->payload, 0, MSGSIZE);
    memcpy(message->payload, aPackage, len_soh_data);
    memcpy(message->payload + len_soh_data, &aPackage->check, size_check_mark);
}

static void create_package(Package *package, Type_Package typePackage, Byte seq,
                           const void *data, int len_data) {
    int len = SIZE_SOH + SIZE_LEN + SIZE_SEQ + SIZE_TYPE + len_data + SIZE_CHECK
              + SIZE_MARK;

    memset(package, 0, sizeof(Package));
    (*package).soh = SOH;
    (*package).len = (unsigned char) (len - SIZE_SOH - SIZE_LEN);
    (*package).seq = seq;
    (*package).type = typePackage;
    if (len_data != 0)
        memcpy(package->data, data, len_data);
    (*package).check = crc16_ccitt(package, len - SIZE_CHECK - SIZE_MARK);
    (*package).mark = CR;
}

static void write_data_S(Data_S *dataS, Byte timeout, Byte maxl) {
    memset(dataS, 0, sizeof(Data_S));
    (*dataS).maxl = maxl;
    (*dataS).time = timeout;
    (*dataS).eol = CR;
}

void create_package_S(Package *package, Byte timeout, Byte maxl) {
    Data_S dataS;
    write_data_S(&dataS, timeout, maxl);
    create_package(package, S, 0, &dataS, sizeof(Data_S));
}

void create_package_F(Package *package, const char *nameFile, Byte seq) {
    create_package(package, F, seq, nameFile, (int) strlen(nameFile));
}

void create_package_D(Package *package, const void *data, int len_data, Byte seq) {
    create_package(package, D, seq, data, len_data);
}

void create_package_Z(Package *package, Byte seq) {
    create_package(package, Z, seq, NULL, 0);
}

void create_package_B(Package *package, Byte seq) {
    create_package(package, B, seq, NULL, 0);
}

void create_package_Y(Package *package, Byte seq) {
    create_package(package, Y, seq, NULL, 0);
}

void create_package_ACK_S(Package *package, Byte seq, Byte timeout, Byte maxl) {
    Data_S dataS;
    write_data_S(&dataS, timeout, maxl);
    create_package(package, Y, seq, &dataS, sizeof(Data_S));
}

void create_package_N(Package *package, Byte seq) {
    create_package(package, N, seq, NULL, 0);
}

void create_package_E(Package *package, Byte seq) {
    create_package(package, E, seq, NULL, 0);
}
