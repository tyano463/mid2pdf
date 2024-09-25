CC			:= $(CROSS_COMPILE)gcc
CFLAGS		:= -g -O0 -Wall -DDEBUG
LDFLAGS		:= -lglib-2.0 -lsmf -lm -Wl,-rpath,/usr/local/lib
INCLUDES	:= -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sysprof-6 -pthread
EXE			:= mid2pdf
OBJS		:= main.o \
		mididata.o \
		musixtex.o \
		dlog.o \
		misc.o \

CFLAGS += $(INCLUDES)


all:$(EXE)

$(EXE):$(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(EXE)
