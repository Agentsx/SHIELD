include ../Makefile.bas

all: clear

OBJS = clearing.o

LIBRARY_PATH = -L$(ROOTPATH)/db -ldb \
	           -L$(ROOTPATH)/libs -lsqlite3 \
	           -L$(ROOTPATH)/libs -lzlog \
	           -L$(ROOTPATH)/utils -lutils

clear: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBRARY_PATH) -o $@
	cp clear ..

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.PHONY:clean

clean:
	rm -f $(OBJS)
