/* The start-up settings file ini/NNNN.ini (core/ini.c). */
#ifndef KF_INI_H
#define KF_INI_H

#include "common.h"

/* Reads ini/NNNN.ini for `number` (ini/9999.ini when it is missing) and applies every key it knows. */
void Ini_Load(int number);

#endif
