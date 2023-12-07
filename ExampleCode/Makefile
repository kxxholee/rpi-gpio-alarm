ARCH ?= arm64

ifeq ($(ARCH),arm64)
CC = aarch64-linux-gnu-gcc
else
CC = gcc
endif

INCDIRS = $(shell find . -type d)

INCFLAGS = $(addprefix -I,$(INCDIRS))

CFLAGS = -Wall -g $(INCFLAGS)

SRCS = $(shell find . -name '*.c')
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) main
