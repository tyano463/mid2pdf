CC		:= $(CROSS_COMPILE)gcc
CFLAGS	:= -g -O0 -Wall
LDFLAGS	:= 
EXE		:= mid2pdf
OBJS	:= main.o \
	musicxml.o \
	musixtex.o \
	dlog.o \
	misc.o \


all:$(EXE)

$(EXE):$(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(EXE)
