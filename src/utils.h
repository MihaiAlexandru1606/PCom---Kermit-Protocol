#include "lib.h"

#ifndef UTILS_H
#define UTILS_H

void package_to_msg(Package *aPackage, msg *message);

void msg_to_package(msg message, Package *package);

void create_package_S(Package *package, Byte timeout, Byte maxl);

void create_package_F(Package *package, const char *nameFile, Byte seq);

void create_package_D(Package *package, const void *data, int len_data, Byte seq);

void create_package_Z(Package *package, Byte seq);

void create_package_B(Package *package, Byte seq);

void create_package_Y(Package *package, Byte seq);

void create_package_ACK_S(Package *package, Byte seq, Byte timeout, Byte maxl);

void create_package_N(Package *package, Byte seq);

void create_package_E(Package *package, Byte seq);

#endif