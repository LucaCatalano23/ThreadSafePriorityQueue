CC = gcc

CFLAGS = -Wall -g

INCLUDES =

LFLAGS = -lpthread

LIBS = 

SRCS = ElemType.c emAlloc.c emCircularBuffer.c emQueue.c main.c PresentazioneEsame.c

OUTDIR = ./build

OBJS = $(SRCS:.c=.o)

OUTOBJS = $(addprefix $(OUTDIR)/, $(OBJS))

MAIN = esame.out

.PHONY: depend clean

all:    $(MAIN)

$(MAIN): $(OUTDIR) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OUTOBJS) $(LFLAGS) $(LIBS)

$(OUTDIR):
	mkdir ./build

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $(OUTDIR)/$@

clean:
	rm -f $(MAIN)
	rm -rf $(OUTDIR)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

run:
	./$(MAIN)

