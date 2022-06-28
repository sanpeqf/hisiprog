# SPDX-License-Identifier: GPL-2.0-or-later
head := hisiprog.h macro.h
obj := hisiprog.o main.o termios.o crc16.o

%.o:%.c $(head)
	@ echo -e "  \e[32mCC\e[0m	" $@
	@ gcc -o $@ -c $< -g -Wall -Werror

hisiprog: $(obj)
	@ echo -e "  \e[34mMKELF\e[0m	" $@
	@ gcc -o $@ $^ -g

clean:
	@ rm -f $(obj) hisiprog
