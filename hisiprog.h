/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _HISIPROG_H_
#define _HISIPROG_H_

#include "macro.h"
#include <stdbool.h>

/* crc16.c */
uint16_t crc16(const uint8_t *src, unsigned int len);

/* termios.c */
extern int termios_open(char *path);
extern void termios_close(void);
extern int termios_setup(unsigned int speed, int databits, int stopbits, char parity);
extern int termios_read(void *data, unsigned long len);
extern int termios_write(const void *data, unsigned long len);

/* hisiprog.c */
extern int hisiprog_flash(void *fwmap, size_t fwsize);
extern int hisiprog_entry(uint32_t fwsize, uint32_t address);
extern int hisiprog_leave(uint8_t time);

#endif  /* _HISIPROG_H_ */
