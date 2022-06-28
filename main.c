/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include "hisiprog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define DEFAULTS_PORT   "/dev/ttyUSB0"
#define DEFAULTS_SPEED  115200

static const struct option options[] = {
    {"help",    no_argument,        0,  'h'},
    {"port",    required_argument,  0,  'p'},
    {"speed",   required_argument,  0,  's'},
    { }, /* NULL */
};

static __noreturn void usage(void)
{
    printf("Usage: hisiprog [options] [address] [file]\n");
    printf("\t-h, --help            display this message\n");
    printf("\t-p, --port <device>   set device path\n");
    printf("\t-s, --speed <freq>    increase the download speed\n");
    exit(1);
}

static __noreturn void version(void)
{
    printf("hisiprog 1.0\n");
    printf("Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>\n");
    printf("License GPLv2+: GNU GPL version 2 or later.\n");
    exit(1);
}

int main(int argc, char *const argv[])
{
    char arg, *firmware, *port = DEFAULTS_PORT;
    unsigned long speed = DEFAULTS_SPEED;
    int ret, fd, index = 0;
    uint32_t address;
    struct stat stat;
    void *fwmap;

    while ((arg = getopt_long(argc, argv, "p:s:vh", options, NULL)) != -1) {
        switch (arg) {
            case 'p':
                index += 2;
                port = optarg;
                break;

            case 's':
                index += 2;
                speed = strtoul(optarg, NULL, 0);
                break;

            case 'v':
                version();

            case 'h': default:
                usage();
        }
    }

    if (argc - index < 2)
        usage();

    address = strtoul(argv[index + 1], NULL, 0);
    firmware = argv[index + 2];

    if ((ret = termios_open(port)))
        err(ret, "failed to open port");

    if ((ret = termios_setup(speed, 8, 1, 'N')))
        err(ret, "failed to setup port");

    if ((fd = open(firmware, O_RDONLY)) < 0)
        err(fd, "failed to open firmware");

    if ((ret = fstat(fd, &stat)) < 0)
        err(ret, "failed to fstat frameware");

    fwmap = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (fwmap == MAP_FAILED)
        err(1, "failed to mmap frameware");

    if ((ret = hisiprog_entry(stat.st_size, address)) < 0)
        err(ret, "failed to starting prog cycle: %d", ret);

    if ((ret = hisiprog_flash(fwmap, stat.st_size)) < 0)
        err(ret, "failed to programming frameware");

    if ((ret = hisiprog_leave(ret)) < 0)
        err(ret, "failed to starting prog cycle");

    return 0;
}
