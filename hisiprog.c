/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include "hisiprog.h"
#include <string.h>

#define HISIPROG_ENTRYFRAME 0xfe, 0x00, 0xff, 0x01
#define HISIPROG_FLASHFRAME 0xda
#define HISIPROG_LEAVEFRAME 0xed
#define HISIPROG_FLASHSUCCE 0xaa
#define HISIPROG_MAXDATALEN 0x400

static inline int hisiprog_xfer(void *data, size_t size)
{
    uint16_t checksum = crc16(data, size);
    uint8_t checkframe[2], result;
    int retval;

    checkframe[0] = (checksum >> 8) & 0xff;
    checkframe[1] = (checksum >> 0) & 0xff;

    if ((0 > (retval = termios_write(data, size))) ||
        (0 > (retval = termios_write(checkframe, sizeof(checkframe)))) ||
        (0 > (retval = termios_read(&result, 1))))
        return retval;

    if (result != HISIPROG_FLASHSUCCE)
        return -EBUSY;

    return 0;
}

int hisiprog_flash(void *fwmap, size_t fwsize)
{
    uint8_t time, flashframe[HISIPROG_MAXDATALEN + 3];
    size_t xfer;
    int retval;

    for (time = 0; (xfer = min(fwsize, HISIPROG_MAXDATALEN)); fwsize -= xfer) {
        flashframe[0] = HISIPROG_FLASHFRAME;
        flashframe[1] = time & 0xff;
        flashframe[2] = ~time & 0xff;

        memcpy(flashframe + 3, fwmap, xfer);
        retval = hisiprog_xfer(flashframe, xfer + 3);
        if (retval)
            return retval;

        fwmap += xfer;
        time++;
    }

    return time;
}

int hisiprog_entry(uint32_t fwsize, uint32_t address)
{
    uint8_t entryframe[12] = {
        HISIPROG_ENTRYFRAME,
        (fwsize  >> 24) & 0xff, (fwsize  >> 16) & 0xff,
        (fwsize  >>  8) & 0xff, (fwsize  >>  0) & 0xff,
        (address >> 24) & 0xff, (address >> 16) & 0xff,
        (address >>  8) & 0xff, (address >>  0) & 0xff,
    };
    return hisiprog_xfer(entryframe, sizeof(entryframe));
}

int hisiprog_leave(uint8_t time)
{
    uint8_t leaveframe[3] = {
        HISIPROG_LEAVEFRAME,
        time & 0xff,
        ~time & 0xff,
    };
    return hisiprog_xfer(leaveframe, sizeof(leaveframe));
}
