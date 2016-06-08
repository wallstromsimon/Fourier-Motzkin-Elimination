# for power.ludat.lth.se
#CFLAGS	= -m64 -O3 -g -Wall -Wextra -Werror -std=c99 -mcpu=970 -mtune=970 -maltivec

# the following works on any machine
CFLAGS	= -O3 -g -Wall -Wextra -Werror -std=c99


CC	= gcc
OUT	= fm
OBJS	= main.o swada_fm2.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)
	./fm
	size swada_fm2.o

clean:
	rm -f $(OUT) $(OBJS)
